#include "localized_file_dialog.h"
#include "localization.h"
// SPDX-License-Identifier: GPL-3.0-only
#include "accelerating_spinbox.h"
#include "studio_panel.h"
#include "studio_name_text.h"
#include "audio_error_text.h"
#include "wav.h"
#include <QCheckBox>
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
StudioPanel::StudioPanel(bool persist, QWidget *parent) : QWidget(parent), persist_(persist) {
    if (persist) try {
        const auto data = QSettings().value("studioSession").toByteArray();
        if (!data.isEmpty() && data.size() <= 8 * 1024 * 1024) session_ = Session::parse(QJsonDocument::fromJson(data).object());
    } catch (const std::exception &) {} // Malformed saved state falls back to dry stereo.
    auto *root = new QVBoxLayout(this);
    auto *layoutBox = new QGroupBox(SC_TR("Channels and routing")); auto *layoutForm = new QFormLayout(layoutBox);
    layout_ = new QComboBox; layout_->addItems({SC_TR("Stereo"), SC_TR("Mono"), "5.1", "7.1", SC_TR("16 channels"), SC_TR("Custom")});
    count_ = new soundcurrent::AcceleratingSpinBox; count_->setRange(1, int(maxChannels)); count_->setAccessibleName(SC_TR("Studio channel count"));
    auto *layoutRow = new QHBoxLayout; layoutRow->addWidget(layout_); layoutRow->addWidget(count_);
    layoutForm->addRow(SC_TR("Layout"), layoutRow);
    offline_ = new QCheckBox(SC_TR("Offline editing — keep current playback unchanged")); layoutForm->addRow(offline_);
    auto *ioHint = new QLabel(SC_TR("Live layouts must fit the selected audio device. Offline rendering and silent meter tests support all 256 channels."));
    ioHint->setWordWrap(true); layoutForm->addRow(ioHint);
    root->addWidget(layoutBox);
    editing_ = new QWidget; auto *editRoot = new QVBoxLayout(editing_); editRoot->setContentsMargins(0,0,0,0);
    auto *fx = new QGroupBox(SC_TR("Effects")); auto *fxForm = new QFormLayout(fx);
    preset_ = new QComboBox; preset_->setAccessibleName(SC_TR("Studio effect preset"));
    preset_->addItems({SC_TR("Dry"), SC_TR("Slapback echo"), SC_TR("Rhythmic echo"), SC_TR("Small room"), SC_TR("Warm hall"), SC_TR("Large hall"), SC_TR("Echo and space"), SC_TR("Custom")});
    fxForm->addRow(SC_TR("Effect preset"), preset_);
    bypass_ = new QCheckBox(SC_TR("Bypass Studio processing")); headroom_ = new QCheckBox(SC_TR("Automatic EQ headroom"));
    auto *powerRow = new QHBoxLayout; powerRow->addWidget(bypass_); powerRow->addWidget(headroom_); fxForm->addRow(powerRow);
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
    editRoot->addWidget(enhancements_);
    enhancements_->onEdited=[this]{change([this](Session &s){s.engine.enhancements=enhancements_->settings();});if(!rebuilding_)preset_->setCurrentIndex(7);};
    editRoot->addWidget(fx);
    auto *channelBox = new QGroupBox(SC_TR("Selected channel")); auto *ch = new QFormLayout(channelBox);
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
    auto *filterButtons = new QHBoxLayout; filterButtons->addWidget(add); filterButtons->addWidget(replace); filterButtons->addWidget(remove); ch->addRow(filterButtons);
    routes_ = table({SC_TR("Input channel"), SC_TR("Gain / polarity")}); routes_->setAccessibleName(SC_TR("Routes into selected output channel")); ch->addRow(routes_);
    routeInput_ = new soundcurrent::AcceleratingSpinBox; routeInput_->setRange(1,2); routeGain_ = spin(-4,4,1,.05);
    ch->addRow(SC_TR("Input channel"), routeInput_); ch->addRow(SC_TR("Linear route gain (negative = invert)"), routeGain_);
    auto *route = new QPushButton(SC_TR("Set route")), *unroute = new QPushButton(SC_TR("Remove selected route")), *identity = new QPushButton(SC_TR("Reset all routing"));
    auto *routeButtons = new QHBoxLayout; routeButtons->addWidget(route); routeButtons->addWidget(unroute); routeButtons->addWidget(identity); ch->addRow(routeButtons);
    editRoot->addWidget(channelBox); root->addWidget(editing_);
    auto *tools = new QHBoxLayout; auto *save = new QPushButton(SC_TR("Save Studio setup")), *open = new QPushButton(SC_TR("Open Studio setup"));
    undo_ = new QPushButton(SC_TR("Undo Studio change")); tools->addWidget(save); tools->addWidget(open); tools->addWidget(undo_); root->addLayout(tools);
    preview_ = new QCheckBox(SC_TR("Test channel meters with a silent generated signal")); root->addWidget(preview_);
    meters_ = table({SC_TR("Channel"), SC_TR("Peak")}); meters_->setAccessibleName(SC_TR("Studio channel output levels")); root->addWidget(meters_);
    auto *renderBox = new QGroupBox(SC_TR("Offline WAVE rendering")); auto *rf = new QFormLayout(renderBox);
    tail_ = spin(0,30,3,.5); tail_->setSuffix(" s"); rf->addRow(SC_TR("Effect tail"), tail_);
    auto *renderRow = new QHBoxLayout; render_ = new QPushButton(SC_TR("Render audio file…")); cancel_ = new QPushButton(SC_TR("Cancel render")); cancel_->setEnabled(false);
    renderRow->addWidget(render_); renderRow->addWidget(cancel_); rf->addRow(renderRow);
    progress_ = new QProgressBar; progress_->setRange(0,100); rf->addRow(progress_); root->addWidget(renderBox);
    status_ = new QLabel(SC_TR("Ready. Effects are dry until enabled.")); status_->setWordWrap(true); root->addWidget(status_); root->addStretch();
    connect(count_, &QSpinBox::valueChanged, this, [this](int count) {
        if (rebuilding_) return;
        change([&](Session &s) { Session next(count); next.engine.delay=s.engine.delay; next.engine.reverb=s.engine.reverb; next.engine.enhancements=s.engine.enhancements;
            next.engine.automaticHeadroom=s.engine.automaticHeadroom; next.engine.bypass=s.engine.bypass; next.offline = true;
            for (int c=0;c<std::min(count,int(s.engine.channels.size()));++c) { next.engine.channels[c]=s.engine.channels[c]; next.names[c]=s.names[c]; next.nameProvenance[std::size_t(c)]=s.nameProvenance[std::size_t(c)]; next.solo[c]=s.solo[c]; }
            s=std::move(next); }); rebuild();
    });
    connect(layout_, &QComboBox::activated, this, [this](int i) { constexpr int counts[]{2,1,6,8,16}; if(i<5) count_->setValue(counts[i]); });
    connect(channel_, &QComboBox::currentIndexChanged, this, [this] { if (!rebuilding_) loadChannel(); });
    connect(offline_, &QCheckBox::toggled, this, [this](bool on) { change([&](Session &s){s.offline=on;}); });
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
    connect(preview_,&QCheckBox::toggled,this,[this](bool on){if(on)startPreview();else {previewEngine_.reset();setLiveLevels({});}});
    connect(render_,&QPushButton::clicked,this,[this]{render();});connect(cancel_,&QPushButton::clicked,this,[this]{cancelJob_=true;});
    connect(&timer_,&QTimer::timeout,this,[this]{tick();});timer_.start(20);rebuild();
}
StudioPanel::~StudioPanel() { cancelJob_=true; if(renderJob_.valid())renderJob_.wait(); }
void StudioPanel::setShared(std::span<const EqBand> bands,double gain,int balance) { shared_.assign(bands.begin(),bands.end());sharedGain_=gain;balance_=balance;if(preview_->isChecked())startPreview(); }
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
}
void StudioPanel::setLocked(bool locked) { locked_=locked;editing_->setEnabled(!locked);count_->setEnabled(!locked);layout_->setEnabled(!locked);offline_->setEnabled(!locked);undo_->setEnabled(!locked&&!history_.empty()); }
void StudioPanel::undo() { if(locked_||history_.empty())return;session_=history_.back();history_.pop_back();rebuild();if(persist_)QSettings().setValue("studioSession",QJsonDocument(session_.json()).toJson(QJsonDocument::Compact));if(preview_->isChecked())startPreview();if(onChanged)onChanged(); }
void StudioPanel::rebuild() {
    rebuilding_=true;const int selected=std::clamp(channel_->currentIndex(),0,int(session_.engine.channels.size())-1);
    count_->setValue(int(session_.engine.channels.size()));layout_->setCurrentIndex(count_->value()==2?0:count_->value()==1?1:count_->value()==6?2:count_->value()==8?3:count_->value()==16?4:5);
    offline_->setChecked(session_.offline);channel_->clear();for(int c=0;c<int(session_.engine.channels.size());++c)channel_->addItem(channelNameText(session_,c));channel_->setCurrentIndex(selected);routeInput_->setMaximum(count_->value());
    enhancements_->setSettings(session_.engine.enhancements);
    const auto &d=session_.engine.delay;const auto &r=session_.engine.reverb;
    delay_->setChecked(d.enabled);delayMs_->setValue(d.milliseconds);feedback_->setValue(d.feedback);wetDelay_->setValue(int(std::lround(d.mix*100)));
    reverb_->setChecked(r.enabled);decay_->setValue(r.decaySeconds);damping_->setValue(r.damping);wetReverb_->setValue(int(std::lround(r.mix*100)));
    bypass_->setChecked(session_.engine.bypass);headroom_->setChecked(session_.engine.automaticHeadroom);undo_->setEnabled(!locked_&&!history_.empty());
    preset_->setCurrentIndex(!d.enabled && !r.enabled && !session_.engine.enhancements.active() ? 0 : 7);
    meters_->setRowCount(count_->value());for(int c=0;c<count_->value();++c){cell(meters_,c,0,QString("%1 · %2").arg(QLocale().toString(c+1)).arg(channelNameText(session_,c)));cell(meters_,c,1,SC_TR("−∞ dBFS"));}
    rebuilding_=false;loadChannel();
}
void StudioPanel::loadChannel() {
    rebuilding_=true;const int c=std::max(0,channel_->currentIndex());const auto &s=session_.engine.channels[std::size_t(c)];
    name_->setText(channelNameText(session_,c));trim_->setValue(int(std::lround(s.gainDb*2)));trim_->setToolTip(QLocale().toString(s.gainDb,'f',1)+SC_TR(" dB"));mute_->setChecked(s.muted);solo_->setChecked(session_.solo[std::size_t(c)]);
    filters_->setRowCount(int(s.bands.size()));for(int r=0;r<int(s.bands.size());++r){const auto &b=s.bands[std::size_t(r)];cell(filters_,r,0,filterType_->itemText(int(b.type)));cell(filters_,r,1,QLocale().toString(b.frequency,'g',6));cell(filters_,r,2,QLocale().toString(b.gainDb,'g',6));cell(filters_,r,3,QLocale().toString(b.q,'g',6));}
    routes_->setRowCount(0);for(int in=0;in<count_->value();++in){const auto weight=session_.routing[std::size_t(c)*std::size_t(count_->value())+std::size_t(in)];if(weight==0)continue;
        const int row=routes_->rowCount();routes_->insertRow(row);cell(routes_,row,0,QString("%1 · %2").arg(QLocale().toString(in+1)).arg(channelNameText(session_,in)));routes_->item(row,0)->setData(Qt::UserRole,in);cell(routes_,row,1,QLocale().toString(weight,'f',3));}
    rebuilding_=false;
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
        auto next=Session::parse(QJsonDocument::fromJson(file.readAll()).object());next.offline=true;const auto before=session_;session_=std::move(next);rebuild();commit(before);status_->setText(SC_TR("Studio setup loaded for offline review. Uncheck offline editing to use it live."));
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
    if(preview_->isChecked()&& !levels.empty())return;
    for(int c=0;c<meters_->rowCount();++c){const float value=std::size_t(c)<levels.size()?levels[std::size_t(c)]:0;
        auto *item=meters_->item(c,1);item->setText(value>1e-8?QLocale().toString(20*std::log10(value),'f',1)+SC_TR(" dBFS"):SC_TR("−∞ dBFS"));
        item->setForeground(value>=1?QColor("#ff6868"):value>=.7?QColor("#ffc66d"):QColor("#55d7c3"));}
}
void StudioPanel::liveStatus(const QString &message,bool rejected) {
    status_->setText(message);
    if(rejected) {
        session_.offline=true;offline_->blockSignals(true);offline_->setChecked(true);offline_->blockSignals(false);
        if(persist_)QSettings().setValue("studioSession",QJsonDocument(session_.json()).toJson(QJsonDocument::Compact));
    }
}
void StudioPanel::tick() {
    if(isVisible()&&previewEngine_&&preview_->isChecked()){
        const auto n=session_.engine.channels.size();for(std::size_t f=0;f<960;++f)for(std::size_t c=0;c<n;++c)
            raw_[f*n+c]=float(.05*std::sin(2*std::numbers::pi*(110+10*c)*(previewFrame_+f)/48000.0));
        previewFrame_+=960;previewRouter_->process(raw_,processed_);previewEngine_->process(processed_);
        preview_->blockSignals(true);preview_->setChecked(false);setLiveLevels(previewEngine_->channelPeaks());preview_->setChecked(true);preview_->blockSignals(false);
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
    status_->setText(SC_TR("Ready. Effects are dry until enabled."));
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
    const auto provenanceSession = session_;
    const auto provenanceHistory = history_;
    session_ = Session(2); rebuild();
    const auto noEdit = session_.json();
    QMetaObject::invokeMethod(name_, "editingFinished", Qt::DirectConnection);
    if (session_.json() != noEdit || session_.defaultNameRole(0) != "left")
        qFatal("Unchanged name field discarded default name provenance");
    name_->setText("Custom %1 / 音声");
    QMetaObject::invokeMethod(name_, "editingFinished", Qt::DirectConnection);
    if (!session_.defaultNameRole(0).isEmpty() || session_.names[0] != "Custom %1 / 音声")
        qFatal("Custom channel name did not clear generated-name provenance");
    count_->setValue(8);
    if (session_.names[0] != "Custom %1 / 音声" || session_.defaultNameRole(1) != "right" ||
        session_.defaultNameRole(7) != "side-right") qFatal("Channel resize lost name provenance");
    undo(); undo();
    if (session_.json() != noEdit) qFatal("Channel name/resize undo lost provenance");
    session_ = provenanceSession; history_ = provenanceHistory; rebuild();
    const auto initial=session_.json();count_->setValue(256);if(count_->value()!=256||channel_->count()!=256||!session_.offline)qFatal("Studio 256-channel UI failed");
    channel_->setCurrentIndex(255);trim_->setValue(-12);if(session_.engine.channels[255].gainDb!=-6)qFatal("Channel 256 trim failed");
    undo();if(session_.engine.channels[255].gainDb!=0)qFatal("Studio undo failed");
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
    render_->setEnabled(true);cancel_->setEnabled(false);tail_->setValue(3);status_->setText(SC_TR("Ready. Effects are dry until enabled."));
    session_=Session::parse(initial);history_.clear();rebuild();
}
}
