#include "localized_file_dialog.h"
#include "localization.h"
// SPDX-License-Identifier: GPL-3.0-only
#include "accelerating_spinbox.h"
#include "studio_panel.h"
#include "channel_meter_view.h"
#include "studio_name_text.h"
#include "audio_error_text.h"
#include "responsive_ui.h"
#include "wav.h"
#include <QCheckBox>
#include <QButtonGroup>
#include <QDialog>
#include <QTabWidget>
#include <QScrollArea>
#include <QScreen>
#include <QWheelEvent>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSaveFile>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <numbers>

namespace soundcurrent::studio {
namespace {
class MixerGainSpin final : public soundcurrent::AcceleratingDoubleSpinBox {
protected:
    void wheelEvent(QWheelEvent *event) override {event->ignore();}
};
QDoubleSpinBox *spin(double low, double high, double value, double step = .1) {
    auto *s = new soundcurrent::AcceleratingDoubleSpinBox; s->setRange(low, high); s->setValue(value); s->setSingleStep(step); return s;
}
QSlider *slider(int low, int high, int value) {
    auto *s = new QSlider(Qt::Horizontal); s->setRange(low, high); s->setValue(value); return s;
}
QTableWidget *table(const QStringList &headers) {
    auto *t = new QTableWidget(0, headers.size()); t->setHorizontalHeaderLabels(headers);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers); t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setMaximumHeight(190); return t;
}
void cell(QTableWidget *t, int row, int column, const QString &value) { t->setItem(row, column, new QTableWidgetItem(value)); }
std::filesystem::path path(const QString &s) {
#ifdef _WIN32
    return std::filesystem::path(s.toStdWString());
#else
    return std::filesystem::path(s.toStdString());
#endif
}
}
StudioPanel::StudioPanel(bool persist, QWidget *parent, bool testControls) : QWidget(parent), persist_(persist), testControls_(testControls) {
    if (persist) try {
        const auto data = QSettings().value("studioSession").toByteArray();
        if (!data.isEmpty() && data.size() <= 8 * 1024 * 1024) session_ = Session::parse(QJsonDocument::fromJson(data).object());
    } catch (const std::exception &) {} // Malformed saved state falls back to dry stereo.
    // Serialized setups include transient rejection staging. Only an
    // explicit editing-mode preference may keep normal startup offline.
    if (persist) session_.offline = QSettings().value("studioOfflineEditing", false).toBool();
    auto *root = new QVBoxLayout(this);
    auto *layoutBox = new QGroupBox(SC_TR("Channels and routing")); auto *layoutForm = new QFormLayout(layoutBox);
    layoutForm->setRowWrapPolicy(QFormLayout::WrapLongRows);layoutForm->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout_ = new QComboBox; layout_->addItems({SC_TR("Stereo"), SC_TR("Mono"), "5.1", "7.1", SC_TR("16 channels"), SC_TR("Custom")});
    count_ = new soundcurrent::AcceleratingSpinBox; count_->setRange(1, int(maxChannels)); count_->setAccessibleName(SC_TR("Studio channel count"));
    auto *layoutRow = new soundcurrent::ui::ResponsiveRow; layoutRow->addWidget(layout_,1); layoutRow->addWidget(count_);
    layoutForm->addRow(SC_TR("Layout"), layoutRow);
    modePanel_=new QWidget;modePanel_->setObjectName("studioPlaybackMode");
    auto *modeLayout=new QVBoxLayout(modePanel_);modeLayout->setContentsMargins(0,0,0,0);
    auto *modeActions=new QHBoxLayout;
    auto *modes=new QButtonGroup(this);modes->setExclusive(true);
    live_=new QPushButton(SC_TR("Live"));live_->setCheckable(true);live_->setObjectName("studioLiveEditing");live_->setMinimumSize(96,40);
    live_->setToolTip(SC_TR("Changes apply immediately"));modes->addButton(live_);modeActions->addWidget(live_);
    offline_ = new QPushButton(SC_TR("Offline"));offline_->setCheckable(true);offline_->setObjectName("studioOfflineEditing");offline_->setMinimumSize(96,40);modes->addButton(offline_);
    offline_->setAccessibleName(SC_TR("Offline editing — keep current playback unchanged"));
    offline_->setToolTip(SC_TR("Offline editing — keep current playback unchanged"));modeActions->addWidget(offline_);
    apply_=new QPushButton(SC_TR("Apply settings"));apply_->setObjectName("applyStudioSettings");apply_->setMinimumSize(140,40);modeActions->addWidget(apply_);modeActions->addStretch(1);
    modeLayout->addLayout(modeActions);
    modeHint_=new QLabel;modeHint_->setWordWrap(true);modeLayout->addWidget(modeHint_);
    root->addWidget(modePanel_);
    connect(apply_,&QPushButton::clicked,this,[this]{if(!locked_)live_->setChecked(true);});
    auto *ioHint = new QLabel(SC_TR("Offline rendering and silent meter tests support all channels."));
    ioHint->setWordWrap(true); layoutForm->addRow(ioHint);
    root->addWidget(layoutBox);
    editing_ = new QWidget; auto *editRoot = new QVBoxLayout(editing_); editRoot->setContentsMargins(0,0,0,0);
    roomBalanceBox_=new QGroupBox(SC_TR("Balance"));
    auto *roomForm=new QVBoxLayout(roomBalanceBox_);
    roomBalance_=new RoomBalancePad;roomForm->addWidget(roomBalance_);
    stereoBalanceControls_=new QWidget;auto *stereoRow=new QHBoxLayout(stereoBalanceControls_);
    stereoRow->addWidget(new QLabel(SC_TR("L")));stereoBalance_=slider(-100,100,0);stereoBalance_->setObjectName("dialogStereoBalance");stereoBalance_->setAccessibleName(SC_TR("Left right balance"));stereoRow->addWidget(stereoBalance_,1);stereoRow->addWidget(new QLabel(SC_TR("R")));
    stereoBalanceValue_=new QLabel(SC_TR("Center"));stereoBalanceValue_->setMinimumWidth(62);stereoRow->addWidget(stereoBalanceValue_);roomForm->addWidget(stereoBalanceControls_);
    centerBalance_=new QPushButton(SC_TR("Center"));centerBalance_->setObjectName("centerDialogBalance");
    roomBalance_->onEdited=[this](double x,double y){change([=](Session &s){s.roomBalanceX=x;s.roomBalanceY=y;});};
    connect(stereoBalance_,&QSlider::valueChanged,this,[this](int value){balance_=value;stereoBalanceValue_->setText(soundcurrent::i18n::balancePositionText(value));if(onStereoBalanceChanged)onStereoBalanceChanged(value);if(preview_->isChecked())startPreview();});
    connect(centerBalance_,&QPushButton::clicked,this,[this]{
        stereoBalance_->setValue(0);roomBalance_->setPosition(0,0);
        const bool wasLocked=locked_;locked_=false;
        change([](Session &s){s.roomBalanceX=0;s.roomBalanceY=0;});
        setLocked(wasLocked); // Center is an explicit reset, including in test mode.
    });
    editRoot->addWidget(roomBalanceBox_);
    pages_=new QTabWidget;pages_->setObjectName("studioWorkPages");pages_->setUsesScrollButtons(true);pages_->setElideMode(Qt::ElideRight);pages_->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);editRoot->addWidget(pages_);
    auto *effectsPage=new QWidget;auto *effectsLayout=new QVBoxLayout(effectsPage);pages_->addTab(effectsPage,SC_TR("Effects"));
    auto *inputRow=new soundcurrent::ui::ResponsiveRow;inputRow->addWidget(new QLabel(SC_TR("Input gain")));
    preGain_=slider(-6000,2400,0);preGain_->setSingleStep(50);preGain_->setPageStep(100);preGain_->setObjectName("inputGain");preGain_->setAccessibleName(SC_TR("Input gain"));inputRow->addWidget(preGain_,1);
    preGainValue_=new QLabel;preGainValue_->setMinimumWidth(76);inputRow->addWidget(preGainValue_);effectsLayout->addWidget(inputRow);
    connect(preGain_,&QSlider::valueChanged,this,[this](int gain){change([&](Session &s){s.engine.preGainDb=gain/100.;});preGainValue_->setText(QLocale().toString(gain/100.,'f',2)+SC_TR(" dB"));});
    mixer_=table({SC_TR("Channel"),SC_TR("Gain"),SC_TR("Mute"),SC_TR("Solo")});mixer_->setObjectName("channelMixer");mixer_->setMaximumHeight(480);pages_->addTab(mixer_,SC_TR("Mixer"));
    auto *channelPage=new QWidget;auto *channelLayout=new QVBoxLayout(channelPage);pages_->addTab(channelPage,SC_TR("Selected channel"));
    auto *fx = new QGroupBox(SC_TR("Effects")); auto *fxForm = new QFormLayout(fx);
    fxForm->setRowWrapPolicy(QFormLayout::WrapLongRows);fxForm->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    preset_ = new QComboBox; preset_->setAccessibleName(SC_TR("Studio effect preset"));
    preset_->addItems({SC_TR("Dry"), SC_TR("Slapback echo"), SC_TR("Rhythmic echo"), SC_TR("Small room"), SC_TR("Warm hall"), SC_TR("Large hall"), SC_TR("Echo and space"), SC_TR("Custom")});
    fxForm->addRow(SC_TR("Effect preset"), preset_);
    bypass_ = new QCheckBox(SC_TR("Bypass Studio processing")); headroom_ = new QCheckBox(SC_TR("Automatic EQ headroom"));
    auto *powerRow = new soundcurrent::ui::ResponsiveRow; powerRow->addWidget(bypass_); powerRow->addWidget(headroom_); fxForm->addRow(powerRow);
    delay_ = new QCheckBox(SC_TR("Delay / echo")); delayMs_ = spin(1,2000,250,1); delayMs_->setSuffix(" ms");
    feedback_ = spin(0,.9,.35,.01); wetDelay_ = slider(0,100,20); wetDelay_->setAccessibleName(SC_TR("Delay wet mix percent"));
    fxForm->addRow(delay_); fxForm->addRow(SC_TR("Delay time"), delayMs_); fxForm->addRow(SC_TR("Feedback"), feedback_); fxForm->addRow(SC_TR("Delay wet mix"), wetDelay_);
    reverb_ = new QCheckBox(SC_TR("Reverb")); decay_ = spin(.1,10,1.5); decay_->setSuffix(" s");
    damping_ = spin(0,.95,.4,.01); wetReverb_ = slider(0,100,15); wetReverb_->setAccessibleName(SC_TR("Reverb wet mix percent"));
    fxForm->addRow(reverb_); fxForm->addRow(SC_TR("Decay"), decay_); fxForm->addRow(SC_TR("Damping"), damping_); fxForm->addRow(SC_TR("Reverb wet mix"), wetReverb_);
    auto *delayLabel=qobject_cast<QLabel *>(fxForm->labelForField(wetDelay_));
    auto *reverbLabel=qobject_cast<QLabel *>(fxForm->labelForField(wetReverb_));
    delayLabel->setText(QString(SC_TR("Delay wet mix · %1%")).arg(QLocale().toString(wetDelay_->value())));
    reverbLabel->setText(QString(SC_TR("Reverb wet mix · %1%")).arg(QLocale().toString(wetReverb_->value())));
    connect(wetDelay_,&QSlider::valueChanged,this,[delayLabel](int v){delayLabel->setText(QString(SC_TR("Delay wet mix · %1%")).arg(QLocale().toString(v)));});
    connect(wetReverb_,&QSlider::valueChanged,this,[reverbLabel](int v){reverbLabel->setText(QString(SC_TR("Reverb wet mix · %1%")).arg(QLocale().toString(v)));});
    enhancements_=new soundcurrent::EnhancementControls(true);
    effectsLayout->addWidget(enhancements_);
    enhancements_->onEdited=[this]{change([this](Session &s){s.engine.enhancements=enhancements_->settings();});if(!rebuilding_)preset_->setCurrentIndex(7);};
    effectsLayout->addWidget(fx);effectsLayout->addStretch();
    auto *channelBox = new QGroupBox(SC_TR("Selected channel")); auto *ch = new QFormLayout(channelBox);
    ch->setRowWrapPolicy(QFormLayout::WrapLongRows);ch->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    channel_ = new QComboBox; channel_->setAccessibleName(SC_TR("Studio selected channel")); name_ = new QLineEdit; name_->setMaxLength(80);
    trim_ = slider(-120,48,0); trim_->setAccessibleName(SC_TR("Channel gain in half dB steps"));
    mute_ = new QCheckBox(SC_TR("Mute")); solo_ = new QCheckBox(SC_TR("Solo")); auto *muteRow = new QHBoxLayout; muteRow->addWidget(mute_); muteRow->addWidget(solo_);
    ch->addRow(SC_TR("Channel"), channel_); ch->addRow(SC_TR("Name"), name_); ch->addRow(SC_TR("Trim"), trim_); ch->addRow(muteRow);
    auto *trimLabel=qobject_cast<QLabel *>(ch->labelForField(trim_));
    trimLabel->setText(QString(SC_TR("Trim · %1 dB")).arg(QLocale().toString(trim_->value()/2.0,'f',1)));
    connect(trim_,&QSlider::valueChanged,this,[trimLabel](int v){trimLabel->setText(QString(SC_TR("Trim · %1 dB")).arg(QLocale().toString(v/2.0,'f',1)));});
    filters_ = table({SC_TR("Type"), "Hz", "dB", "Q"}); filters_->setAccessibleName(SC_TR("Selected channel EQ filters")); ch->addRow(filters_);
    filterType_ = new QComboBox; filterType_->addItems({SC_TR("Peaking"), SC_TR("Low shelf"), SC_TR("High shelf"), SC_TR("High pass"), SC_TR("Low pass")});
    filterHz_ = spin(20,20000,1000,10); filterDb_ = spin(-24,24,0,.5); filterQ_ = spin(.1,20,1);
    ch->addRow(SC_TR("Filter type"), filterType_); ch->addRow(SC_TR("Frequency"), filterHz_); ch->addRow(SC_TR("Gain"), filterDb_); ch->addRow("Q", filterQ_);
    auto *add = new QPushButton(SC_TR("Add filter")), *replace = new QPushButton(SC_TR("Update selected")), *remove = new QPushButton(SC_TR("Remove selected"));
    auto *filterButtons = new soundcurrent::ui::ResponsiveRow; filterButtons->addWidget(add); filterButtons->addWidget(replace); filterButtons->addWidget(remove); ch->addRow(filterButtons);
    routes_ = table({SC_TR("Input channel"), SC_TR("Gain / polarity")}); routes_->setAccessibleName(SC_TR("Routes into selected output channel")); ch->addRow(routes_);
    routeInput_ = new soundcurrent::AcceleratingSpinBox; routeInput_->setRange(1,2); routeGain_ = spin(-4,4,1,.05);
    ch->addRow(SC_TR("Input channel"), routeInput_); ch->addRow(SC_TR("Linear route gain (negative = invert)"), routeGain_);
    auto *route = new QPushButton(SC_TR("Set route")), *unroute = new QPushButton(SC_TR("Remove selected route")), *identity = new QPushButton(SC_TR("Reset all routing"));
    auto *routeButtons = new soundcurrent::ui::ResponsiveRow; routeButtons->addWidget(route); routeButtons->addWidget(unroute); routeButtons->addWidget(identity); ch->addRow(routeButtons);
    channelLayout->addWidget(channelBox);channelLayout->addStretch();root->addWidget(editing_);
    auto *tools = new soundcurrent::ui::ResponsiveRow; auto *save = new QPushButton(SC_TR("Save Studio setup")), *open = new QPushButton(SC_TR("Open Studio setup"));
    undo_ = new QPushButton(SC_TR("Undo Studio change")); tools->addWidget(save); tools->addWidget(open); tools->addWidget(undo_); root->addWidget(tools);
    meterBank_=new QComboBox;meterBank_->setAccessibleName(SC_TR("Studio channel output levels"));meterBank_->setObjectName("channelMeterBank");
    meterView_=new ChannelMeterView;
    connect(meterBank_,&QComboBox::currentIndexChanged,this,[this](int bank){if(!rebuilding_)meterView_->setBank(bank);});
    preview_ = new QCheckBox(SC_TR("Test channel meters with a silent generated signal"));
    meters_ = table({SC_TR("Channel"), SC_TR("Peak")}); meters_->setAccessibleName(SC_TR("Studio channel output levels"));
    auto *renderBox = new QGroupBox(SC_TR("Offline WAVE rendering")); auto *rf = new QFormLayout(renderBox);
    rf->setRowWrapPolicy(QFormLayout::WrapLongRows);rf->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    tail_ = spin(0,30,3,.5); tail_->setSuffix(" s"); rf->addRow(SC_TR("Effect tail"), tail_);
    auto *renderRow = new soundcurrent::ui::ResponsiveRow; render_ = new QPushButton(SC_TR("Render audio file…")); cancel_ = new QPushButton(SC_TR("Cancel render")); cancel_->setEnabled(false);
    renderRow->addWidget(render_); renderRow->addWidget(cancel_); rf->addRow(renderRow);
    progress_ = new QProgressBar; progress_->setRange(0,100); rf->addRow(progress_);
    pages_->addTab(renderBox,SC_TR("Offline WAVE rendering"));
    monitorDialog_=new QDialog(this);monitorDialog_->setObjectName("channelMonitoring");monitorDialog_->setWindowModality(Qt::ApplicationModal);monitorDialog_->setWindowTitle(SC_TR("Channel meters")+(testControls_?QStringLiteral(" · ")+SC_TR("Test mode"):QString()));
    auto *monitorRoot=new QVBoxLayout(monitorDialog_);auto *monitorScroll=new QScrollArea;monitorScroll->setWidgetResizable(true);monitorRoot->addWidget(monitorScroll);
    auto *monitorPage=new QWidget;auto *monitorLayout=new QVBoxLayout(monitorPage);monitorScroll->setWidget(monitorPage);
    meterSource_=new QLabel;meterSource_->setObjectName("channelMeterSource");meterSource_->setWordWrap(true);monitorLayout->addWidget(meterSource_);
    monitorLayout->addWidget(meterBank_);monitorLayout->addWidget(meterView_);monitorLayout->addWidget(preview_);monitorLayout->addWidget(roomBalanceBox_);monitorLayout->addWidget(centerBalance_);monitorLayout->addWidget(meters_);
    showMeters_=new QPushButton(SC_TR("Channel meters"));showMeters_->setObjectName("studioShowMeters");layoutForm->addRow(showMeters_);connect(showMeters_,&QPushButton::clicked,this,[this]{showMonitoring();});
    status_ = new QLabel; status_->setWordWrap(true);root->insertWidget(1,status_);root->addStretch();
    connect(count_, &QSpinBox::valueChanged, this, [this](int count) {
        if (rebuilding_) return;
        if(monitorDialog_->isVisible()){const QSignalBlocker blocked(count_);count_->setValue(int(session_.engine.channels.size()));return;}
        change([&](Session &s) { Session next(count); next.engine.delay=s.engine.delay; next.engine.reverb=s.engine.reverb; next.engine.enhancements=s.engine.enhancements;next.roomBalanceX=s.roomBalanceX;next.roomBalanceY=s.roomBalanceY;
            next.engine.preGainDb=s.engine.preGainDb;next.engine.automaticHeadroom=s.engine.automaticHeadroom; next.engine.bypass=s.engine.bypass; next.offline = s.offline;
            for (int c=0;c<std::min(count,int(s.engine.channels.size()));++c) { next.engine.channels[c]=s.engine.channels[c]; if(s.defaultNameRole(c).isEmpty()){next.names[c]=s.names[c]; next.nameProvenance[std::size_t(c)]=s.nameProvenance[std::size_t(c)];} next.solo[c]=s.solo[c]; }
            s=std::move(next); }); rebuild();
    });
    connect(layout_, &QComboBox::activated, this, [this](int i) { constexpr int counts[]{2,1,6,8,16}; if(i<5) count_->setValue(counts[i]); });
    connect(channel_, &QComboBox::currentIndexChanged, this, [this] { if (!rebuilding_) loadChannel(); });
    connect(offline_, &QPushButton::toggled, this, [this](bool on) {
        if (rebuilding_ || locked_) return;
        if (persist_) QSettings().setValue("studioOfflineEditing", on);
        change([&](Session &s){s.offline=on;});
    });
    connect(name_, &QLineEdit::editingFinished, this, [this] { if(name_->text().trimmed().isEmpty()) return;
        const int channel = channel_->currentIndex();
        if (name_->text().trimmed() == channelNameText(session_,channel)) return;
        change([&](Session &s){s.setCustomName(channel,name_->text().trimmed());}); rebuild(); });
    connect(trim_, &QSlider::valueChanged, this, [this](int v){ change([&](Session &s){s.engine.channels[channel_->currentIndex()].gainDb=v/2.0;}); trim_->setToolTip(QLocale().toString(v/2.0,'f',1)+SC_TR(" dB")); });
    connect(mute_, &QCheckBox::toggled, this, [this](bool v){change([&](Session &s){s.engine.channels[channel_->currentIndex()].muted=v;});});
    connect(solo_, &QCheckBox::toggled, this, [this](bool v){change([&](Session &s){s.solo[channel_->currentIndex()]=v;});});
    const auto effects = [this] { change([&](Session &s) {
        s.engine.delay={delay_->isChecked(),delayMs_->value(),feedback_->value(),wetDelay_->value()/100.0};
        s.engine.reverb={reverb_->isChecked(),decay_->value(),damping_->value(),wetReverb_->value()/100.0};
        s.engine.bypass=bypass_->isChecked(); s.engine.automaticHeadroom=headroom_->isChecked(); });
        if(!rebuilding_) preset_->setCurrentIndex(7);
    };
    for(auto *b:{delay_,reverb_,bypass_,headroom_}) connect(b,&QCheckBox::toggled,this,effects);
    for(auto *b:{delayMs_,feedback_,decay_,damping_}) connect(b,&QDoubleSpinBox::valueChanged,this,effects);
    for(auto *b:{wetDelay_,wetReverb_}) connect(b,&QSlider::valueChanged,this,effects);
    connect(preset_, &QComboBox::activated, this, [this](int i){effectPreset(i);});
    const auto editFilter = [this](bool replace) { const int row=filters_->currentRow();
        change([&](Session &s){auto &b=s.engine.channels[channel_->currentIndex()].bands;
            const EqBand filter{filterHz_->value(),filterDb_->value(),filterQ_->value(),static_cast<FilterType>(filterType_->currentIndex())};
            if(replace && row>=0) b[std::size_t(row)]=filter;
            else if(!replace && b.size()+shared_.size()<kMaxProcessingBands) b.push_back(filter);
            else throw std::runtime_error(SC_TR("Select a filter to update, or remove filters before adding more").toStdString()); }); loadChannel(); };
    connect(add,&QPushButton::clicked,this,[editFilter]{editFilter(false);}); connect(replace,&QPushButton::clicked,this,[editFilter]{editFilter(true);});
    connect(remove,&QPushButton::clicked,this,[this]{ const int row=filters_->currentRow(); if(row<0)return;
        change([&](Session &s){auto &b=s.engine.channels[channel_->currentIndex()].bands; b.erase(b.begin()+row);}); loadChannel();});
    connect(filters_,&QTableWidget::cellClicked,this,[this](int row,int){const auto &b=session_.engine.channels[channel_->currentIndex()].bands[std::size_t(row)];
        filterType_->setCurrentIndex(int(b.type));filterHz_->setValue(b.frequency);filterDb_->setValue(b.gainDb);filterQ_->setValue(b.q);});
    connect(route,&QPushButton::clicked,this,[this]{change([&](Session &s){s.routing[std::size_t(channel_->currentIndex())*s.engine.channels.size()+std::size_t(routeInput_->value()-1)]=routeGain_->value();});loadChannel();});
    connect(unroute,&QPushButton::clicked,this,[this]{const int row=routes_->currentRow();if(row<0)return;const auto input=routes_->item(row,0)->data(Qt::UserRole).toInt();
        change([&](Session &s){s.routing[std::size_t(channel_->currentIndex())*s.engine.channels.size()+std::size_t(input)]=0;});loadChannel();});
    connect(identity,&QPushButton::clicked,this,[this]{change([](Session &s){s.routing=Session(s.engine.channels.size()).routing;});loadChannel();});
    connect(save,&QPushButton::clicked,this,[this]{saveProfile();});connect(open,&QPushButton::clicked,this,[this]{openProfile();});connect(undo_,&QPushButton::clicked,this,[this]{undo();});
    connect(preview_,&QCheckBox::toggled,this,[this](bool on){if(on){startPreview();meterSource_->setText(SC_TR("Test channel meters with a silent generated signal"));}else {previewEngine_.reset();setLiveLevels({});}});
    connect(render_,&QPushButton::clicked,this,[this]{render();});connect(cancel_,&QPushButton::clicked,this,[this]{cancelJob_=true;});
    connect(&timer_,&QTimer::timeout,this,[this]{tick();});timer_.start(20);rebuild();
}
StudioPanel::~StudioPanel() { cancelJob_=true; if(renderJob_.valid())renderJob_.wait(); }
void StudioPanel::setShared(std::span<const EqBand> bands,double gain,int balance) { shared_.assign(bands.begin(),bands.end());sharedGain_=gain;balance_=balance;const QSignalBlocker blocked(stereoBalance_);stereoBalance_->setValue(balance);stereoBalanceValue_->setText(soundcurrent::i18n::balancePositionText(balance));if(preview_->isChecked())startPreview(); }
void StudioPanel::change(const std::function<void(Session &)> &fn) {
    if(rebuilding_||locked_)return;
    const auto before=session_;
    try {fn(session_);AudioEngine validate(48000,session_.engine.channels.size());std::string error;
        if(!validate.configure(session_.effective(shared_,sharedGain_,balance_),&error))throw std::runtime_error(error);
        commit(before);
    } catch(const std::exception &e){session_=before;status_->setText(soundcurrent::i18n::audioErrorText(QString::fromUtf8(e.what())));rebuild();}
}
void StudioPanel::commit(const Session &before) {
    if(session_.json()==before.json())return;
    if(history_.size()==64)history_.erase(history_.begin());history_.push_back(before);undo_->setEnabled(!locked_);
    if(persist_)QSettings().setValue("studioSession",QJsonDocument(session_.json()).toJson(QJsonDocument::Compact));
    if(preview_->isChecked())startPreview();
    if(onChanged)onChanged();
    updateApplyState();syncMixer();
}
void StudioPanel::setLocked(bool locked) { locked_=locked;for(int i=0;i<3;++i)pages_->widget(i)->setEnabled(!locked);roomBalanceBox_->setEnabled(!locked);updateApplyState();count_->setEnabled(!locked);layout_->setEnabled(!locked);offline_->setEnabled(!locked);live_->setEnabled(!locked);undo_->setEnabled(!locked&&!history_.empty()); }
void StudioPanel::undo() { if(locked_||history_.empty())return;const bool wasOffline=session_.offline;session_=history_.back();history_.pop_back();rebuild();if(persist_){QSettings().setValue("studioSession",QJsonDocument(session_.json()).toJson(QJsonDocument::Compact));if(wasOffline!=session_.offline)QSettings().setValue("studioOfflineEditing",session_.offline);}if(preview_->isChecked())startPreview();if(onChanged)onChanged(); }
void StudioPanel::rebuild() {
    rebuilding_=true;const int selected=std::clamp(channel_->currentIndex(),0,int(session_.engine.channels.size())-1);
    count_->setValue(int(session_.engine.channels.size()));layout_->setCurrentIndex(count_->value()==2?0:count_->value()==1?1:count_->value()==6?2:count_->value()==8?3:count_->value()==16?4:5);
    live_->setChecked(!session_.offline);offline_->setChecked(session_.offline);channel_->clear();for(int c=0;c<int(session_.engine.channels.size());++c)channel_->addItem(channelNameText(session_,c));channel_->setCurrentIndex(selected);routeInput_->setMaximum(count_->value());
    preGain_->setValue(int(std::lround(session_.engine.preGainDb*100)));
    preGainValue_->setText(QLocale().toString(session_.engine.preGainDb,'f',2)+SC_TR(" dB"));
    updateApplyState();
    rebuildMixer();
    enhancements_->setSettings(session_.engine.enhancements);
    roomBalanceBox_->setVisible(session_.hasRoomLayout()||count_->value()==2);roomBalance_->setVisible(session_.hasRoomLayout());stereoBalanceControls_->setVisible(count_->value()==2);
    roomBalance_->setPosition(session_.roomBalanceX,session_.roomBalanceY);
    meterView_->setSession(session_);
    meterBank_->clear();
    for(int first=0;first<count_->value();first+=64)meterBank_->addItem(QString("%1–%2").arg(QLocale().toString(first+1)).arg(QLocale().toString(std::min(first+64,count_->value()))));
    meterBank_->setVisible(count_->value()>64);
    const auto &d=session_.engine.delay;const auto &r=session_.engine.reverb;
    delay_->setChecked(d.enabled);delayMs_->setValue(d.milliseconds);feedback_->setValue(d.feedback);wetDelay_->setValue(int(std::lround(d.mix*100)));
    reverb_->setChecked(r.enabled);decay_->setValue(r.decaySeconds);damping_->setValue(r.damping);wetReverb_->setValue(int(std::lround(r.mix*100)));
    bypass_->setChecked(session_.engine.bypass);headroom_->setChecked(session_.engine.automaticHeadroom);undo_->setEnabled(!locked_&&!history_.empty());
    preset_->setCurrentIndex(!d.enabled && !r.enabled && !session_.engine.enhancements.active() ? 0 : 7);
    meters_->setRowCount(count_->value());for(int c=0;c<count_->value();++c){cell(meters_,c,0,QString("%1 · %2").arg(QLocale().toString(c+1)).arg(channelNameText(session_,c)));cell(meters_,c,1,SC_TR("−∞ dBFS"));}
    meters_->setFixedHeight(std::min(190,meters_->horizontalHeader()->sizeHint().height()+count_->value()*meters_->verticalHeader()->defaultSectionSize()+2*meters_->frameWidth()));
    rebuilding_=false;loadChannel();
}
void StudioPanel::loadChannel() {
    rebuilding_=true;const int c=std::max(0,channel_->currentIndex());const auto &s=session_.engine.channels[std::size_t(c)];
    name_->setText(channelNameText(session_,c));trim_->setValue(int(std::lround(s.gainDb*2)));trim_->setToolTip(QLocale().toString(s.gainDb,'f',1)+SC_TR(" dB"));mute_->setChecked(s.muted);solo_->setChecked(session_.solo[std::size_t(c)]);
    filters_->setRowCount(int(s.bands.size()));for(int r=0;r<int(s.bands.size());++r){const auto &b=s.bands[std::size_t(r)];cell(filters_,r,0,filterType_->itemText(int(b.type)));cell(filters_,r,1,QLocale().toString(b.frequency,'g',6));cell(filters_,r,2,QLocale().toString(b.gainDb,'g',6));cell(filters_,r,3,QLocale().toString(b.q,'g',6));}
    routes_->setRowCount(0);for(int in=0;in<count_->value();++in){const auto weight=session_.routing[std::size_t(c)*std::size_t(count_->value())+std::size_t(in)];if(weight==0)continue;
        const int row=routes_->rowCount();routes_->insertRow(row);cell(routes_,row,0,QString("%1 · %2").arg(QLocale().toString(in+1)).arg(channelNameText(session_,in)));routes_->item(row,0)->setData(Qt::UserRole,in);cell(routes_,row,1,QLocale().toString(weight,'f',3));}
    rebuilding_=false;syncMixer();
}
void StudioPanel::updateApplyState() {
    modePanel_->setStyleSheet(QStringLiteral("QPushButton { padding: 8px 20px; border-radius: 4px; } "
        "QPushButton#studioLiveEditing:checked { background-color: #55d7c3; color: #101820; font-weight: 700; } "
        "QPushButton#studioOfflineEditing:checked { background-color: #f4c46a; color: #101820; font-weight: 700; }"));
    showMeters_->setEnabled(monitoringAvailable());
    apply_->setVisible(session_.offline);apply_->setEnabled(!locked_&&session_.offline);
    modeHint_->setText(session_.offline?SC_TR("Offline editing. Current playback keeps its last live Studio setup."):SC_TR("Changes apply immediately"));
}
void StudioPanel::showMonitoring() {
    if(!monitoringAvailable())return;
    const auto available=screen()->availableGeometry();
    const int width=std::max(1,std::min(session_.engine.channels.size()>2?780:560,available.width()-40));
    auto *scroll=monitorDialog_->findChild<QScrollArea *>();auto *content=scroll->widget();content->ensurePolished();content->layout()->activate();
    int height=content->layout()->totalHeightForWidth(width-40);if(height<0)height=content->layout()->sizeHint().height();
    const int maximumHeight=std::max(1,available.height()-60);
    monitorDialog_->resize(width,std::clamp(height+40,std::min(220,maximumHeight),maximumHeight));
    monitorDialog_->show();monitorDialog_->raise();monitorDialog_->activateWindow();
}
void StudioPanel::rebuildMixer() {
    mixer_->clearContents();mixer_->setRowCount(int(session_.engine.channels.size()));
    for(int c=0;c<mixer_->rowCount();++c) {
        const auto name=channelNameText(session_,c);cell(mixer_,c,0,name);
        auto *gain=new MixerGainSpin;gain->setRange(-60,24);gain->setSingleStep(.5);gain->setValue(session_.engine.channels[std::size_t(c)].gainDb);gain->setDecimals(1);gain->setSuffix(SC_TR(" dB"));gain->setAccessibleName(name+QStringLiteral(" · ")+SC_TR("Gain"));mixer_->setCellWidget(c,1,gain);
        auto *mute=new QCheckBox;mute->setChecked(session_.engine.channels[std::size_t(c)].muted);mute->setAccessibleName(name+QStringLiteral(" · ")+SC_TR("Mute"));mixer_->setCellWidget(c,2,mute);
        auto *solo=new QCheckBox;solo->setChecked(session_.solo[std::size_t(c)]);solo->setAccessibleName(name+QStringLiteral(" · ")+SC_TR("Solo"));mixer_->setCellWidget(c,3,solo);
        connect(gain,&QDoubleSpinBox::valueChanged,this,[this,c](double v){if(rebuilding_||locked_)return;if(c>=int(session_.engine.channels.size()))return;change([&](Session &s){s.engine.channels[std::size_t(c)].gainDb=v;});if(channel_->currentIndex()==c)loadChannel();});
        connect(mute,&QCheckBox::toggled,this,[this,c](bool v){if(rebuilding_||locked_)return;if(c>=int(session_.engine.channels.size()))return;change([&](Session &s){s.engine.channels[std::size_t(c)].muted=v;});if(channel_->currentIndex()==c)loadChannel();});
        connect(solo,&QCheckBox::toggled,this,[this,c](bool v){if(rebuilding_||locked_)return;if(c>=int(session_.engine.channels.size()))return;change([&](Session &s){s.solo[std::size_t(c)]=v;});if(channel_->currentIndex()==c)loadChannel();});
    }
}
void StudioPanel::syncMixer() {
    for(int c=0;c<std::min(mixer_->rowCount(),int(session_.engine.channels.size()));++c) {
        auto *gain=qobject_cast<QDoubleSpinBox *>(mixer_->cellWidget(c,1));auto *mute=qobject_cast<QCheckBox *>(mixer_->cellWidget(c,2));auto *solo=qobject_cast<QCheckBox *>(mixer_->cellWidget(c,3));
        if(!gain||!mute||!solo)continue;
        const QSignalBlocker b1(gain),b2(mute),b3(solo);
        gain->setValue(session_.engine.channels[std::size_t(c)].gainDb);mute->setChecked(session_.engine.channels[std::size_t(c)].muted);solo->setChecked(session_.solo[std::size_t(c)]);
    }
}
void StudioPanel::effectPreset(int i) {
    if(i==7)return;change([i](Session &s){s.engine.delay=DelaySettings{};s.engine.reverb=ReverbSettings{};s.engine.enhancements={};
        if(i==1)s.engine.delay={true,90,.15,.25};if(i==2)s.engine.delay={true,375,.45,.25};
        if(i==3)s.engine.reverb={true,.5,.6,.18};if(i==4)s.engine.reverb={true,1.8,.55,.2};
        if(i==5)s.engine.reverb={true,4,.35,.25};if(i==6){s.engine.delay={true,250,.3,.15};s.engine.reverb={true,1.5,.4,.15};}});rebuild();preset_->setCurrentIndex(i);
}
void StudioPanel::saveProfile() {
    const auto filename=soundcurrent::i18n::FileDialogs::getSaveFileName(this,SC_TR("Save Studio setup"),{},SC_TR("Studio setup (*.scstudio)"));if(filename.isEmpty())return;
    QSaveFile file(filename);if(!file.open(QIODevice::WriteOnly)){status_->setText(SC_TR("Cannot save setup"));return;}file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner);
    file.write(QJsonDocument(session_.json()).toJson());status_->setText(file.commit()?SC_TR("Studio setup saved."):SC_TR("Cannot finish saving setup."));
}
void StudioPanel::openProfile() {
    if(locked_)return;const auto filename=soundcurrent::i18n::FileDialogs::getOpenFileName(this,SC_TR("Open Studio setup"),{},SC_TR("Studio setup (*.scstudio)"));if(filename.isEmpty())return;
    try {QFile file(filename);if(!file.open(QIODevice::ReadOnly)||file.size()>8*1024*1024)throw std::runtime_error(SC_TR("Setup cannot be read or exceeds 8 MiB").toStdString());
        auto next=Session::parse(QJsonDocument::fromJson(file.readAll()).object());next.offline=true;if(persist_)QSettings().setValue("studioOfflineEditing",true);const auto before=session_;session_=std::move(next);rebuild();commit(before);status_->setText(SC_TR("Studio setup loaded for offline review. Uncheck offline editing to use it live."));
    }catch(const std::exception &e){status_->setText(soundcurrent::i18n::audioErrorText(QString::fromUtf8(e.what())));}
}
void StudioPanel::startPreview() {
    try {previewEngine_=std::make_unique<AudioEngine>(48000,session_.engine.channels.size());std::string error;
        if(!previewEngine_->configure(session_.effective(shared_,sharedGain_,balance_),&error))throw std::runtime_error(error);
        previewRouter_=std::make_unique<ChannelRouter>(count_->value(),count_->value());previewRouter_->setMatrix(session_.routing);
        raw_.resize(960*session_.engine.channels.size());processed_.resize(raw_.size());previewFrame_=0;
    }catch(const std::exception &e){previewEngine_.reset();status_->setText(soundcurrent::i18n::audioErrorText(QString::fromUtf8(e.what())));}
}
void StudioPanel::setLiveLevels(std::span<const float> levels) {
    if(preview_->isChecked())return;
    if(!levels.empty() && levels.size()!=session_.engine.channels.size()) {
        meterSource_->setText(SC_TR("Live layout differs from staged layout"));
        updateDisplayedLevels({});return;
    }
    meterSource_->setText(levels.empty()?SC_TR("Equalizer is off. Your audio uses its normal output."):SC_TR("Meters show live playback"));
    updateDisplayedLevels(levels);
}
void StudioPanel::updateDisplayedLevels(std::span<const float> levels) {
    meterView_->setLevels(levels);
    for(int c=0;c<meters_->rowCount();++c){const float value=std::size_t(c)<levels.size()?levels[std::size_t(c)]:0;
        auto *item=meters_->item(c,1);item->setText(value>1e-8?QLocale().toString(20*std::log10(value),'f',1)+SC_TR(" dBFS"):SC_TR("−∞ dBFS"));
        item->setForeground(value>=1?QColor("#ff6868"):value>=.7?QColor("#ffc66d"):QColor("#55d7c3"));}
}
void StudioPanel::liveStatus(const QString &message,bool rejected) {
    // Mode guidance already has a dedicated, always-visible label. Keep the
    // operational status line for distinct results and errors instead of
    // rendering the same guidance twice.
    status_->setText(message == modeHint_->text() ? QString() : message);
    if(rejected) {
        session_.offline=true;updateApplyState();offline_->blockSignals(true);offline_->setChecked(true);offline_->blockSignals(false);
        if(persist_)QSettings().setValue("studioSession",QJsonDocument(session_.json()).toJson(QJsonDocument::Compact));
    }
}
void StudioPanel::tick() {
    if((isVisible()||monitorDialog_->isVisible())&&previewEngine_&&preview_->isChecked()){
        const auto n=session_.engine.channels.size();for(std::size_t f=0;f<960;++f)for(std::size_t c=0;c<n;++c)
            raw_[f*n+c]=float(.05*std::sin(2*std::numbers::pi*(110+10*c)*(previewFrame_+f)/48000.0));
        previewFrame_+=960;previewRouter_->process(raw_,processed_);previewEngine_->process(processed_);
        updateDisplayedLevels(previewEngine_->channelPeaks());
        meterSource_->setText(SC_TR("Test channel meters with a silent generated signal"));
    }
    if(renderJob_.valid()) {progress_->setValue(jobProgress_);if(renderJob_.wait_for(std::chrono::seconds(0))==std::future_status::ready){status_->setText(renderJob_.get());render_->setEnabled(true);cancel_->setEnabled(false);}}
}
void StudioPanel::render() {
    if(renderJob_.valid())return;const auto input=soundcurrent::i18n::FileDialogs::getOpenFileName(this,SC_TR("Input WAVE file"),{},SC_TR("WAVE audio (*.wav)"));if(input.isEmpty())return;
    const auto output=soundcurrent::i18n::FileDialogs::getSaveFileName(this,SC_TR("New rendered WAVE file"),{},SC_TR("WAVE audio (*.wav)"));if(output.isEmpty())return;
    renderFiles(input,output);
}
void StudioPanel::renderFiles(const QString &input,const QString &output) {
    if(renderJob_.valid())return;
    const auto s=session_;const auto shared=shared_;const double gain=sharedGain_,tail=tail_->value();const int balance=balance_;
    cancelJob_=false;jobProgress_=0;render_->setEnabled(false);cancel_->setEnabled(true);status_->setText(SC_TR("Rendering…"));
    renderJob_=std::async(std::launch::async,[this,s,shared,gain,tail,balance,input,output]() -> QString {
        try {const auto final=path(output);if(std::filesystem::exists(final))throw std::runtime_error(SC_TR("Output already exists; select a new filename").toStdString());
            QTemporaryDir staging(QFileInfo(output).absolutePath()+"/.soundcurrent-render-XXXXXX");
            if(!staging.isValid())throw std::runtime_error(SC_TR("Cannot create output staging directory").toStdString());
            WaveReader reader(path(input));const auto source=reader.format();const auto n=s.engine.channels.size();
            if(source.channels>n)throw std::runtime_error(SC_TR("Input has more channels than the Studio layout; choose a matching or larger layout").toStdString());
            AudioEngine engine(int(source.sampleRate),n);std::string error;if(!engine.configure(s.effective(shared,gain,balance),&error))throw std::runtime_error(error);
            ChannelRouter router(n,n);router.setMatrix(s.routing);WaveFormat format=source;format.channels=unsigned(n);
            if(n!=source.channels||s.routing!=Session(n).routing)format.channelMask=0;
            format.frames+=std::uint64_t(std::llround(tail*source.sampleRate));const auto temporary=path(staging.filePath("audio.wav"));WaveWriter writer(temporary,format);
            std::vector<float> inputBlock(1024*source.channels),padded(1024*n),processed(1024*n);std::uint64_t clips=0;
            for(std::uint64_t position=0;position<format.frames;) {
                if(cancelJob_)throw std::runtime_error(SC_TR("Render cancelled; no output file published").toStdString());const auto frames=std::size_t(std::min<std::uint64_t>(1024,format.frames-position));
                std::fill(inputBlock.begin(),inputBlock.end(),0.0f);reader.read(std::span(inputBlock).first(frames*source.channels));std::fill(padded.begin(),padded.end(),0.0f);
                for(std::size_t f=0;f<frames;++f)std::copy_n(inputBlock.data()+f*source.channels,source.channels,padded.data()+f*n);
                auto block=std::span(processed).first(frames*n);router.process(std::span(padded).first(frames*n),block);clips+=engine.process(block).clippedSamples;writer.write(block);
                position+=frames;jobProgress_=int(position*100/std::max<std::uint64_t>(1,format.frames));
            }
            writer.finish();if(cancelJob_)throw std::runtime_error(SC_TR("Render cancelled; no output file published").toStdString());std::filesystem::create_hard_link(temporary,final);
            return SC_TR("Rendered %1 channels. Clipped samples: %2. %3").arg(QLocale().toString(qulonglong(n))).arg(QLocale().toString(qulonglong(clips))).arg(output);
        }catch(const std::exception &e){return SC_TR("Render: %1").arg(soundcurrent::i18n::audioErrorText(QString::fromUtf8(e.what())));}
    });
}
void StudioPanel::selfTestRenderErrors() {
    QTemporaryDir files;
    if (!files.isValid() || renderJob_.valid()) qFatal("Render diagnostic fixture unavailable");
    const auto check = [this, &files](const QString &input, const char *source) {
        const auto output = files.filePath("unpublished.wav");
        renderFiles(input, output);
        const auto result = renderJob_.get();
        const auto expected = SC_TR("Render: %1").arg(soundcurrent::i18n::audioErrorText(QString::fromUtf8(source)));
        if (result != expected || QFile::exists(output))
            qFatal("Localized render failure or output preservation failed: %s", source);
    };
    check(files.filePath("missing.wav"), "Cannot open input WAVE file");
    const auto malformed = files.filePath("malformed.wav");
    {
        QFile file(malformed);
        if (!file.open(QIODevice::WriteOnly) || file.write(QByteArray::fromHex("524946460000000057415645")) != 12)
            qFatal("Cannot create RIFF extent fixture");
    }
    check(malformed, "Invalid RIFF size");
    {
        QFile file(malformed);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate) ||
            file.write(QByteArray::fromHex("524946460c000000574156454a554e4b64000000")) != 20)
            qFatal("Cannot create RIFF chunk fixture");
    }
    check(malformed, "Chunk extends beyond RIFF bounds");
    render_->setEnabled(true); cancel_->setEnabled(false);
    status_->clear();
}
void StudioPanel::selfTestChannelNames() {
    const auto original = session_;
    const auto history = history_;
    for (const auto count : {1,2,6,8,16,256}) {
        session_ = Session(count); rebuild();
        const auto snapshot = session_.json();
        for (const int channel : {0,count-1}) {
            channel_->setCurrentIndex(channel);
            const auto caption = channelNameText(session_,channel);
            if (channel_->itemText(channel) != caption || name_->text() != caption ||
                !meters_->item(channel,0)->text().endsWith(caption) ||
                !routes_->item(0,0)->text().endsWith(caption))
                qFatal("Generated name display missed a Studio control");
            QMetaObject::invokeMethod(name_,"editingFinished",Qt::DirectConnection);
            if (session_.json() != snapshot) qFatal("Localized no-op name editing rewrote saved setup");
        }
        if (Session::parse(snapshot).json() != snapshot)
            qFatal("Localized generated channel name changed canonical saved state");
    }
    session_ = Session(2); rebuild();channel_->setCurrentIndex(0);
    name_->setText("User %1 / 音声 / Left");
    QMetaObject::invokeMethod(name_,"editingFinished",Qt::DirectConnection);
    if (!session_.defaultNameRole(0).isEmpty() || channel_->itemText(0)!="User %1 / 音声 / Left" ||
        name_->text()!="User %1 / 音声 / Left") qFatal("Custom channel name was translated or altered");
    undo();
    if (session_.defaultNameRole(0)!="left" || name_->text()!=SC_TR("Left"))
        qFatal("Undo failed to restore localized generated name");
    session_ = original; history_ = history; rebuild();
}
void StudioPanel::selfTestFormatting() {
    const auto original = session_;
    const auto history = history_;
    session_ = Session(2);
    session_.engine.channels[0].gainDb = -1.5;
    session_.engine.channels[0].bands = {{1234.5,-2.5,.707}};
    session_.routing[1] = -.125;
    const auto fixture = session_.json();
    rebuild();
    const QLocale locale;
    if (trim_->toolTip() != locale.toString(-1.5,'f',1)+SC_TR(" dB") ||
        filters_->item(0,1)->text() != locale.toString(1234.5,'g',6) ||
        filters_->item(0,2)->text() != locale.toString(-2.5,'g',6) ||
        filters_->item(0,3)->text() != locale.toString(.707,'g',6) ||
        routes_->item(1,1)->text() != locale.toString(-.125,'f',3) ||
        routes_->item(1,0)->data(Qt::UserRole).toInt() != 1)
        qFatal("Studio regional filter/route/trim display failed");
    const std::array<float,2> levels{.5f,0};
    setLiveLevels(levels);
    if (meters_->item(0,1)->text() != locale.toString(20*std::log10(.5f),'f',1)+SC_TR(" dBFS") ||
        meters_->item(1,1)->text() != SC_TR("−∞ dBFS"))
        qFatal("Studio regional live level display failed");
    if (session_.json() != fixture) qFatal("Studio formatting changed persisted audio settings");
    auto *delayLabel = qobject_cast<QLabel *>(qobject_cast<QFormLayout *>(wetDelay_->parentWidget()->layout())->labelForField(wetDelay_));
    auto *reverbLabel = qobject_cast<QLabel *>(qobject_cast<QFormLayout *>(wetReverb_->parentWidget()->layout())->labelForField(wetReverb_));
    if (!delayLabel || !reverbLabel) qFatal("Studio wet mix label fixture missing");
    wetDelay_->setValue(37); wetReverb_->setValue(42);
    if (delayLabel->text() != SC_TR("Delay wet mix · %1%").arg(locale.toString(37)) ||
        reverbLabel->text() != SC_TR("Reverb wet mix · %1%").arg(locale.toString(42)) ||
        session_.engine.delay.mix != .37 || session_.engine.reverb.mix != .42)
        qFatal("Studio regional wet mix label or processing value failed");
    session_ = original; history_ = history; rebuild();
}
void StudioPanel::selfTest() {
    Session lowGain(2);
    lowGain.engine.postGainDb = -24;
    const auto effective = lowGain.effective({}, -60, 0);
    AudioEngine quietEngine(48000, 2);
    if (effective.postGainDb != -84 || !quietEngine.configure(effective))
        qFatal("Studio clipped the shared low post gain");
    std::array<float, 2> quietFrame{.5f, .5f};
    quietEngine.process(quietFrame);
    if (std::abs(quietFrame[0] - .5 * std::pow(10., -84. / 20)) > 1e-9)
        qFatal("Studio low post gain amplitude is incorrect");
    {
        StudioPanel production(false);
        production.session_=Session(2);production.rebuild();
        if(!production.monitoringAvailable() || !production.showMeters_->isEnabled())qFatal("Production stereo monitoring button should be enabled");
        production.showMonitoring();QCoreApplication::processEvents();
        if(!production.monitorDialog_->isVisible() || production.monitorDialog_->windowModality()!=Qt::ApplicationModal || production.stereoBalanceControls_->isHidden())qFatal("Production stereo monitoring lacks its modal balance dialog");
        const std::array<float,2> stereoLevels{.2f,.4f};production.setLiveLevels(stereoLevels);
        if(production.meterView_->levelForChannel(0)!=stereoLevels[0] || production.meterView_->levelForChannel(1)!=stereoLevels[1])qFatal("Production stereo meters did not receive live levels");
        production.stereoBalance_->setValue(35);production.centerBalance_->click();
        if(production.stereoBalance_->value()!=0 || !production.centerBalance_->isEnabled())qFatal("Production stereo balance reset failed");
        production.monitorDialog_->hide();production.count_->setValue(1);
        if(production.monitoringAvailable() || production.showMeters_->isEnabled())qFatal("Production mono monitoring button should be disabled");
        production.showMonitoring();if(production.monitorDialog_->isVisible())qFatal("Production mono dialog unexpectedly opened");
        production.count_->setValue(6);
        if(!production.monitoringAvailable() || !production.showMeters_->isEnabled() || production.session_.offline)qFatal("Live multichannel layout should enable monitoring without staging");
    }
    {
        QSettings settings;
        const auto savedSession = settings.value("studioSession");
        const auto savedPreference = settings.value("studioOfflineEditing");
        Session staged(2); staged.offline = true;
        settings.setValue("studioSession", QJsonDocument(staged.json()).toJson(QJsonDocument::Compact));
        settings.remove("studioOfflineEditing");
        {
            StudioPanel restored(true);
            if (restored.session_.offline) qFatal("Legacy staged session trapped startup offline");
            restored.offline_->setChecked(true);
            restored.undo();
            if(restored.session_.offline || settings.value("studioOfflineEditing").toBool())
                qFatal("Undo left an unintended offline startup preference");
            restored.offline_->setChecked(true);
            StudioPanel intentional(true);
            if (!intentional.session_.offline) qFatal("Intentional offline preference was lost");
            int liveChanges = 0;
            restored.onChanged = [&] { if (!restored.session_.offline) ++liveChanges; };
            restored.apply_->click();
            if (restored.session_.offline || settings.value("studioOfflineEditing").toBool() || liveChanges != 1)
                qFatal("Apply settings did not clear preference and notify backend");
            restored.mute_->setChecked(true);
            if (!restored.session_.engine.channels[0].muted || liveChanges != 2)
                qFatal("Compatible live channel mute was not delivered");
            restored.liveStatus("Rejected fixture", true);
            StudioPanel afterRejection(true);
            if (afterRejection.session_.offline) qFatal("Rejected layout persisted accidental offline preference");
        }
        if (savedSession.isValid()) settings.setValue("studioSession", savedSession); else settings.remove("studioSession");
        if (savedPreference.isValid()) settings.setValue("studioOfflineEditing", savedPreference); else settings.remove("studioOfflineEditing");
    }
    const auto provenanceSession = session_;
    const auto provenanceHistory = history_;
    session_ = Session(2); rebuild();
    if(!testControls_)qFatal("Studio self-test requires explicit test controls");
    showMonitoring();QCoreApplication::processEvents();
    if(monitorDialog_->windowModality()!=Qt::ApplicationModal || !centerBalance_->isEnabled() || stereoBalanceControls_->isHidden())qFatal("Stereo test dialog lacks modality/balance/reset");
    stereoBalance_->setValue(67);centerBalance_->click();
    if(stereoBalance_->value()!=0 || balance_!=0 || !centerBalance_->isEnabled())qFatal("Stereo Center reset failed");
    count_->setValue(6);
    if(session_.engine.channels.size()!=2 || count_->value()!=2)qFatal("Modal monitoring allowed a channel-layout change");
    monitorDialog_->hide();
    const std::array<float,2> displayedLevels{.25f,.5f}, liveLevels{.9f,.8f};
    setLiveLevels(displayedLevels);
    session_.offline=true;updateApplyState();
    setLiveLevels(liveLevels);
    if(meterView_->levelForChannel(0)!=liveLevels[0] || meterSource_->text()!=SC_TR("Meters show live playback"))
        qFatal("Offline editing suppresses actual live meters");
    const std::array<float,1> wrongLayout{.8f};setLiveLevels(wrongLayout);
    if(meterView_->levelForChannel(0)!=0 || meterSource_->text()!=SC_TR("Live layout differs from staged layout"))
        qFatal("Mismatched live layout shown as draft channels");
    session_.offline=false;updateApplyState();setLiveLevels(displayedLevels);
    const auto displayedText=meters_->item(0,1)->text();
    preview_->setChecked(true);
    for(const auto feed:{std::span<const float>(liveLevels),std::span<const float>()}) {
        setLiveLevels(feed);
        if(meterView_->levelForChannel(0)!=displayedLevels[0] ||
           meterView_->levelForChannel(1)!=displayedLevels[1] || meters_->item(0,1)->text()!=displayedText)
            qFatal("Live/stopped playback overwrote silent-preview meters");
    }
    preview_->setChecked(false);
    if(meterView_->levelForChannel(0)!=0)qFatal("Stopping silent preview did not reset graphical meters");
    setLiveLevels(liveLevels);
    if(meterView_->levelForChannel(0)!=liveLevels[0])qFatal("Live meters did not resume after silent preview");
    const auto noEdit = session_.json();
    QMetaObject::invokeMethod(name_, "editingFinished", Qt::DirectConnection);
    if (session_.json() != noEdit || session_.defaultNameRole(0) != "left")
        qFatal("Unchanged name field discarded default name provenance");
    name_->setText("Custom %1 / 音声");
    QMetaObject::invokeMethod(name_, "editingFinished", Qt::DirectConnection);
    if (!session_.defaultNameRole(0).isEmpty() || session_.names[0] != "Custom %1 / 音声")
        qFatal("Custom channel name did not clear generated-name provenance");
    count_->setValue(8);
    if (session_.names[0] != "Custom %1 / 音声" || session_.defaultNameRole(1) != "front-right" ||
        session_.defaultNameRole(7) != "side-right") qFatal("Channel resize lost name provenance");
    undo(); undo();
    if (session_.json() != noEdit) qFatal("Channel name/resize undo lost provenance");
    session_ = provenanceSession; history_ = provenanceHistory; rebuild();
    // The mixer and the selected-channel editor share the same immediate state.
    auto *mixGain=qobject_cast<QDoubleSpinBox *>(mixer_->cellWidget(0,1));
    const auto gainBeforeWheel=mixGain->value();
    QWheelEvent mixerWheel(QPointF(5,5),QPointF(5,5),QPoint(),QPoint(0,-120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
    QCoreApplication::sendEvent(mixGain,&mixerWheel);
    if(mixGain->value()!=gainBeforeWheel)qFatal("Scrolling changed a mixer gain");
    mixGain->setValue(-4.5);
    if(session_.engine.channels[0].gainDb!=-4.5 || trim_->value()!=-9)qFatal("Mixer gain is not immediate/shared");
    qobject_cast<QCheckBox *>(mixer_->cellWidget(0,2))->setChecked(true);
    if(!session_.engine.channels[0].muted || !mute_->isChecked())qFatal("Mixer mute is not immediate/shared");
    undo();undo();
    preGain_->setValue(300);if(session_.engine.preGainDb!=3)qFatal("Input gain not applied immediately");undo();
    if(session_.engine.preGainDb!=0)qFatal("Input gain undo failed");
    if(pages_->count()!=4 || pages_->tabText(0)!=SC_TR("Effects"))qFatal("Studio pages not organized");
    if(!apply_->isHidden()||apply_->isEnabled())qFatal("Live editing exposed Apply settings");
    if(live_->minimumHeight()<40||offline_->minimumHeight()<40||apply_->minimumHeight()<40||live_->minimumWidth()<96||offline_->minimumWidth()<96||apply_->minimumWidth()<140)qFatal("Studio mode buttons are too small");
    const auto initial=session_.json();offline_->setChecked(true);count_->setValue(256);if(count_->value()!=256||channel_->count()!=256||!session_.offline||apply_->isHidden()||!apply_->isEnabled()||mixer_->rowCount()!=256)qFatal("Studio 256-channel UI failed");
    liveStatus(SC_TR("Offline editing. Current playback keeps its last live Studio setup."));
    if(!status_->text().isEmpty()||modeHint_->text().isEmpty())qFatal("Offline editing guidance was rendered twice");
    apply_->click();if(session_.offline||apply_->isEnabled()||!apply_->isHidden()||!live_->isChecked()||offline_->isChecked())qFatal("Apply settings did not switch staged settings to live state");
    channel_->setCurrentIndex(255);trim_->setValue(-12);if(session_.engine.channels[255].gainDb!=-6)qFatal("Channel 256 trim failed");
    undo();if(session_.engine.channels[255].gainDb!=0)qFatal("Studio undo failed");
    count_->setValue(1);showMonitoring();QCoreApplication::processEvents();
    if(session_.defaultNameRole(0)!="mono" || !monitorDialog_->isVisible() || !roomBalanceBox_->isHidden())qFatal("Mono monitoring/balance state incorrect");
    const auto shots=qEnvironmentVariable("SOUNDCURRENT_UI_SCREENSHOT_DIR");
    if(!shots.isEmpty()) {
        monitorDialog_->grab().save(shots+"/monitor-mono.png");
        monitorDialog_->hide();count_->setValue(6);showMonitoring();QCoreApplication::processEvents();
        if(!session_.hasRoomLayout() || roomBalanceBox_->isHidden())qFatal("Choosing surround did not expose room balance");
        monitorDialog_->grab().save(shots+"/monitor-surround.png");
        roomBalanceBox_->grab().save(shots+"/balance-surround.png");
        monitorDialog_->hide();count_->setValue(256);showMonitoring();QCoreApplication::processEvents();monitorDialog_->grab().save(shots+"/monitor-256.png");
    }
    monitorDialog_->hide();
    count_->setValue(2);effectPreset(6);if(!session_.engine.delay.enabled||!session_.engine.reverb.enabled)qFatal("Studio effect preset failed");
    const auto beforeEnhancement=session_.engine.enhancements;
    auto *bass=enhancements_->findChildren<QSlider *>().at(4);bass->setValue(50);
    if(session_.engine.enhancements.values[BassBoost]!=.5)qFatal("Studio enhancement control failed");
    undo();if(!(session_.engine.enhancements==beforeEnhancement))qFatal("Studio enhancement undo failed");
    auto *threshold=enhancements_->findChildren<QDoubleSpinBox *>().at(5);threshold->setValue(-30);
    if(session_.engine.enhancements.values[Threshold]!=-30)qFatal("Advanced enhancement control failed");
    const auto roundtrip=Session::parse(session_.json());if(roundtrip.json()!=session_.json())qFatal("Studio profile round trip failed");
    auto legacy=session_.json();legacy.remove("enhancements");
    if(Session::parse(legacy).engine.enhancements.active())qFatal("Legacy setup enhancements not dry");
    auto invalid=session_.json();invalid["enhancements"]=QJsonArray{1};bool badEffects=false;
    try{Session::parse(invalid);}catch(const std::exception &){badEffects=true;}if(!badEffects)qFatal("Malformed effects accepted");
    auto broken=session_.json();broken["postGain"]=100;bool rejected=false;
    try{Session::parse(broken);}catch(const std::exception &){rejected=true;}if(!rejected)qFatal("Invalid Studio setup gain accepted");
    setLocked(true);const auto locked=session_.json();change([](Session &s){s.engine.delay.milliseconds=100;});if(session_.json()!=locked)qFatal("Studio lock failed");setLocked(false);
    session_=Session(256);session_.routing[255*256]=.5;session_.routing[255*256+255]=0;rebuild();tail_->setValue(0);
    QTemporaryDir files;const auto input=files.filePath("input.wav"),output=files.filePath("output.wav");
    {WaveWriter writer(path(input),{48000,2,3,8});std::vector<float> audio(16);for(int f=0;f<8;++f){audio[2*f]=.2f;audio[2*f+1]=.3f;}writer.write(audio);writer.finish();}
    renderFiles(input,output);const auto rendered=renderJob_.get();if(!QFile::exists(output))qFatal("UI renderer did not publish output: %s",qPrintable(rendered));
    {WaveReader reader(path(output));std::vector<float> audio(256*8);reader.read(audio);
        if(reader.format().channels!=256||std::abs(audio[255]-.1f)>1e-5||std::abs(audio[1]-.3f)>1e-5)qFatal("UI renderer lost channel routing");}
    renderFiles(input,output);const auto duplicate=renderJob_.get();if(!duplicate.contains("already exists"))qFatal("UI renderer replaced existing output");
    render_->setEnabled(true);cancel_->setEnabled(false);tail_->setValue(3);status_->clear();
    session_=Session::parse(initial);history_.clear();rebuild();
}
}
