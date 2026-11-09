// SPDX-License-Identifier: GPL-3.0-only
#include "application_mixer.h"
#include "localization_text.h"
#include <QCheckBox>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QShowEvent>
#include <QSignalBlocker>
#include <QSlider>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <vector>
#ifdef Q_OS_WIN
#include <windows.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#endif
namespace soundcurrent::studio {
namespace {
struct Stream { QString id,name,detail;int volume=100;bool muted=false; };
#ifndef Q_OS_WIN
std::vector<Stream> pulseStreams(const QByteArray &bytes,bool *valid=nullptr){
    if(valid)*valid=false;
    if(bytes.size()>4*1024*1024)return {};
    QJsonParseError error;
    const auto document=QJsonDocument::fromJson(bytes,&error);
    if(error.error!=QJsonParseError::NoError||!document.isArray())return {};
    std::vector<Stream> result;
    const auto array=document.array();
    result.reserve(std::min<qsizetype>(array.size(),512));
    for(const auto &value:array){
        if(!value.isObject()||result.size()>=512)continue;
        const auto object=value.toObject();
        const auto properties=object.value("properties").toObject();
        const auto channels=object.value("volume").toObject();
        double total=0.;int channelCount=0;
        for(auto channel=channels.begin();channel!=channels.end();++channel){
            QString percent=channel.value().toObject().value("value_percent").toString();
            percent.remove('%');bool okay=false;const double parsed=percent.trimmed().toDouble(&okay);
            if(okay&&std::isfinite(parsed)){total+=parsed;++channelCount;}
        }
        QString name=properties.value("application.name").toString();
        if(name.isEmpty())name=properties.value("application.process.binary").toString();
        if(name.isEmpty())name=SC_TR("Unknown application");
        result.push_back({QString::number(object.value("index").toInt()),name.left(200),properties.value("media.name").toString().left(500),
                          std::clamp(int(std::lround(channelCount?total/channelCount:100.)),0,100),object.value("mute").toBool()});
    }
    if(valid)*valid=true;
    return result;
}
#endif
#ifdef Q_OS_WIN
template<class T> void release(T *value){if(value)value->Release();}
QString wide(LPCWSTR value){return value?QString::fromWCharArray(value):QString();}
QString processName(DWORD pid){
    HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);if(!process)return {};
    wchar_t path[32768];DWORD size=32768;const bool okay=QueryFullProcessImageNameW(process,0,path,&size);CloseHandle(process);
    return okay?QFileInfo(QString::fromWCharArray(path,int(size))).completeBaseName():QString();
}
std::vector<Stream> windowsStreams(){
    std::vector<Stream> result;IMMDeviceEnumerator *devices=nullptr;IMMDevice *device=nullptr;IAudioSessionManager2 *manager=nullptr;IAudioSessionEnumerator *sessions=nullptr;
    if(FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,IID_PPV_ARGS(&devices))))return result;
    if(SUCCEEDED(devices->GetDefaultAudioEndpoint(eRender,eMultimedia,&device)) && SUCCEEDED(device->Activate(__uuidof(IAudioSessionManager2),CLSCTX_ALL,nullptr,reinterpret_cast<void **>(&manager))) && SUCCEEDED(manager->GetSessionEnumerator(&sessions))){
        int count=0;sessions->GetCount(&count);for(int i=0;i<count;++i){
            IAudioSessionControl *control=nullptr;IAudioSessionControl2 *control2=nullptr;ISimpleAudioVolume *volume=nullptr;
            if(FAILED(sessions->GetSession(i,&control)) || FAILED(control->QueryInterface(IID_PPV_ARGS(&control2))) || FAILED(control->QueryInterface(IID_PPV_ARGS(&volume)))){release(volume);release(control2);release(control);continue;}
            AudioSessionState state=AudioSessionStateInactive;control->GetState(&state);DWORD pid=0;control2->GetProcessId(&pid);LPWSTR instance=nullptr,display=nullptr;control2->GetSessionInstanceIdentifier(&instance);control->GetDisplayName(&display);
            float level=1;BOOL muted=FALSE;volume->GetMasterVolume(&level);volume->GetMute(&muted);
            QString name=wide(display);if(name.trimmed().isEmpty())name=processName(pid);if(name.isEmpty())name=SC_TR("Application mixer");
            if(state!=AudioSessionStateExpired)result.push_back({wide(instance),name,QString::number(pid),int(std::lround(level*100)),bool(muted)});
            CoTaskMemFree(instance);CoTaskMemFree(display);release(volume);release(control2);release(control);
        }
    }
    release(sessions);release(manager);release(device);release(devices);return result;
}
void updateWindows(const QString &wanted,int level,std::optional<bool> muted){
    IMMDeviceEnumerator *devices=nullptr;IMMDevice *device=nullptr;IAudioSessionManager2 *manager=nullptr;IAudioSessionEnumerator *sessions=nullptr;
    if(FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,IID_PPV_ARGS(&devices))))return;
    if(SUCCEEDED(devices->GetDefaultAudioEndpoint(eRender,eMultimedia,&device)) && SUCCEEDED(device->Activate(__uuidof(IAudioSessionManager2),CLSCTX_ALL,nullptr,reinterpret_cast<void **>(&manager))) && SUCCEEDED(manager->GetSessionEnumerator(&sessions))){
        int count=0;sessions->GetCount(&count);for(int i=0;i<count;++i){IAudioSessionControl *control=nullptr;IAudioSessionControl2 *control2=nullptr;ISimpleAudioVolume *volume=nullptr;
            if(SUCCEEDED(sessions->GetSession(i,&control))&&SUCCEEDED(control->QueryInterface(IID_PPV_ARGS(&control2)))){LPWSTR id=nullptr;control2->GetSessionInstanceIdentifier(&id);if(wide(id)==wanted&&SUCCEEDED(control->QueryInterface(IID_PPV_ARGS(&volume)))){if(muted)volume->SetMute(*muted,nullptr);else volume->SetMasterVolume(std::clamp(level/100.f,0.f,1.f),nullptr);}CoTaskMemFree(id);}release(volume);release(control2);release(control);}
    }release(sessions);release(manager);release(device);release(devices);
}
#endif
}
struct ApplicationMixer::Private {
    ApplicationMixer *owner;QVBoxLayout *rows;QLabel *status;QTimer timer;QProcess query;bool test=false,comInitialized=false;QString pactl=QStandardPaths::findExecutable("pactl");std::vector<Stream> streams;
    explicit Private(ApplicationMixer *q,bool testing):owner(q),test(testing),timer(q),query(q){}
    void clear(){while(auto *item=rows->takeAt(0)){if(item->widget())item->widget()->deleteLater();delete item;}}
    void render(){
        clear();status->setVisible(streams.empty());status->setText(streams.empty()?SC_TR("No active application audio streams."):QString());
        for(const auto &stream:streams){
            auto *box=new QWidget;auto *layout=new QVBoxLayout(box);layout->setContentsMargins(0,5,0,5);
            auto *title=new QLabel(stream.name);title->setToolTip(stream.detail);layout->addWidget(title);
            auto *line=new QHBoxLayout;auto *slider=new QSlider(Qt::Horizontal);slider->setObjectName("applicationVolumeSlider");slider->setRange(0,100);slider->setValue(stream.volume);slider->setAccessibleName(stream.name+QStringLiteral(" · ")+SC_TR("System volume"));
            auto *value=new QLabel(QString::number(stream.volume)+"%");value->setMinimumWidth(48);value->setAlignment(Qt::AlignRight|Qt::AlignVCenter);auto *mute=new QCheckBox(SC_TR("Mute"));mute->setChecked(stream.muted);
            line->addWidget(slider,1);line->addWidget(value);line->addWidget(mute);layout->addLayout(line);rows->addWidget(box);
            const auto id=stream.id;QObject::connect(slider,&QSlider::valueChanged,box,[this,id,value](int v){value->setText(QString::number(v)+"%");if(test)return;
#ifdef Q_OS_WIN
                updateWindows(id,v,{});
#else
                if(!pactl.isEmpty())QProcess::startDetached(pactl,{"set-sink-input-volume",id,QString::number(v)+"%"});
#endif
            });
            QObject::connect(mute,&QCheckBox::toggled,box,[this,id,slider](bool on){if(test)return;
#ifdef Q_OS_WIN
                updateWindows(id,slider->value(),on);
#else
                if(!pactl.isEmpty())QProcess::startDetached(pactl,{"set-sink-input-mute",id,on?"1":"0"});
#endif
            });
            QObject::connect(slider,&QSlider::sliderPressed,&timer,&QTimer::stop);
            QObject::connect(slider,&QSlider::sliderReleased,box,[this]{if(!test)timer.start();});
        }rows->addStretch();
    }
};
ApplicationMixer::ApplicationMixer(bool testMode,QWidget *parent):QWidget(parent),d(std::make_unique<Private>(this,testMode)){
#ifdef Q_OS_WIN
    const auto com=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);d->comInitialized=SUCCEEDED(com);
#endif
    auto *root=new QVBoxLayout(this);auto *help=new QLabel(SC_TR("Application streams on the current output device. These controls change the operating system's per-application volumes."));help->setWordWrap(true);root->addWidget(help);
    auto *refreshButton=new QPushButton(SC_TR("Refresh applications"));refreshButton->setObjectName("refreshApplicationMixer");root->addWidget(refreshButton);
    d->status=new QLabel;d->status->setWordWrap(true);root->addWidget(d->status);auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);auto *body=new QWidget;d->rows=new QVBoxLayout(body);scroll->setWidget(body);root->addWidget(scroll,1);
    connect(refreshButton,&QPushButton::clicked,this,[this]{refresh();});connect(&d->timer,&QTimer::timeout,this,[this]{refresh();});d->timer.setInterval(2000);
#ifndef Q_OS_WIN
    connect(&d->query,&QProcess::finished,this,[this](int code,QProcess::ExitStatus status){const auto unavailable=[this]{d->streams.clear();d->status->setText(SC_TR("Application streams are unavailable. PipeWire's PulseAudio compatibility service and pactl are required."));d->status->show();};if(code||status!=QProcess::NormalExit){unavailable();return;}bool valid=false;d->streams=pulseStreams(d->query.readAllStandardOutput(),&valid);if(!valid){unavailable();return;}d->render();});
#endif
    if(testMode){
#ifndef Q_OS_WIN
        bool valid=false;const auto parsed=pulseStreams(R"([{"index":7,"mute":false,"volume":{"front-left":{"value_percent":"64%"},"front-right":{"value_percent":"66%"}},"properties":{"application.name":"Web browser","media.name":"Test stream"}}])",&valid);
        if(!valid||parsed.size()!=1||parsed.front().volume!=65)qFatal("Application mixer could not parse pactl channel volumes");
#endif
        d->streams={{"test-browser",QStringLiteral("Web browser"),QStringLiteral("Test stream"),64,false},{"test-player",QStringLiteral("Media player"),QStringLiteral("Test stream"),82,true}};d->render();
    }
}
ApplicationMixer::~ApplicationMixer(){
#ifdef Q_OS_WIN
    if(d->comInitialized)CoUninitialize();
#endif
}
void ApplicationMixer::refresh(){if(d->test)return;
#ifdef Q_OS_WIN
    d->streams=windowsStreams();d->render();
#else
    if(d->query.state()!=QProcess::NotRunning||d->pactl.isEmpty()){if(d->pactl.isEmpty()){d->status->setText(SC_TR("Application streams are unavailable. PipeWire's PulseAudio compatibility service and pactl are required."));d->status->show();}return;}d->query.start(d->pactl,{"--format=json","list","sink-inputs"});
#endif
}
int ApplicationMixer::sliderCount()const{return findChildren<QSlider *>("applicationVolumeSlider").size();}
void ApplicationMixer::showEvent(QShowEvent *event){QWidget::showEvent(event);if(!d->test){refresh();d->timer.start();}}
void ApplicationMixer::hideEvent(QHideEvent *event){QWidget::hideEvent(event);d->timer.stop();}
}
