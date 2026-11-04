// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "localization.h"
#ifdef _WIN32
#include "windows_audio.h"
#endif
#include <QWidget>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSignalBlocker>
#include <QLocale>
#include <cmath>
#include <algorithm>
namespace soundcurrent {
// System endpoint volume, never an additional DSP multiplier. All OS calls and
// child processes belong to the GUI thread, outside the realtime audio path.
class SystemVolumeControl : public QWidget {
public:
    explicit SystemVolumeControl(bool live, QWidget *parent=nullptr):QWidget(parent),live_(live) {
        auto *row=new QHBoxLayout(this);row->setContentsMargins(0,0,0,0);
        row->addWidget(new QLabel(SC_TR("System volume")));
        slider_=new QSlider(Qt::Horizontal);slider_->setObjectName("systemOutputVolume");
        slider_->setAccessibleName(SC_TR("System volume"));slider_->setLayoutDirection(Qt::LeftToRight);
        slider_->setRange(0,100);slider_->setEnabled(false);row->addWidget(slider_,1);
        value_=new QLabel;value_->setMinimumWidth(62);value_->setAlignment(Qt::AlignRight|Qt::AlignVCenter);row->addWidget(value_);
        setToolTip(SC_TR("System volume"));
        connect(slider_,&QSlider::valueChanged,this,[this](int v){
            caption(v,false);
            if(!live_)return;
            pending_=v;write();
        });
#ifndef _WIN32
        for(auto *process:{&query_,&writer_,&default_}) {
            auto *deadline=new QTimer(this);deadline->setSingleShot(true);deadline->setInterval(1500);
            connect(process,&QProcess::started,deadline,qOverload<>(&QTimer::start));
            connect(process,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),deadline,&QTimer::stop);
            connect(deadline,&QTimer::timeout,this,[this,process]{pending_=-1;process->kill();unavailable();});
        }
        connect(&query_,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[this](int code,QProcess::ExitStatus status){
            const auto bytes=query_.readAllStandardOutput();query_.readAllStandardError();
            if(code || status!=QProcess::NormalExit){unavailable();return;}
            const auto list=QJsonDocument::fromJson(bytes).array();
            // A new default endpoint is queried by name on each request. Never
            // write a volume to an endpoint inherited from the previous route.
            for(const auto &entry:list){const auto sink=entry.toObject();
                if(sink.value("name").toString()!=queryEndpoint_)continue;
                double max=0;const auto volumes=sink.value("volume").toObject();
                for(const auto &v:volumes)max=std::max(max,v.toObject().value("value").toDouble(-1));
                if(max<0 || volumes.isEmpty()){unavailable();return;}
                if(pending_<0 && writer_.state()==QProcess::NotRunning)
                    applyRead(int(std::lround(max*100/65536)),sink.value("mute").toBool());
                return;
            }
            unavailable();
        });
        connect(&query_,&QProcess::errorOccurred,this,[this](QProcess::ProcessError){unavailable();});
        connect(&writer_,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[this](int code,QProcess::ExitStatus status){
            writer_.readAllStandardOutput();writer_.readAllStandardError();
            if(code || status!=QProcess::NormalExit){pending_=-1;unavailable();return;}
            if(pending_>=0)write();else refresh();
        });
        connect(&writer_,&QProcess::errorOccurred,this,[this](QProcess::ProcessError){pending_=-1;unavailable();});
        connect(&default_,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[this](int code,QProcess::ExitStatus status){
            queryEndpoint_=QString::fromUtf8(default_.readAllStandardOutput()).trimmed();default_.readAllStandardError();
            if(code || status!=QProcess::NormalExit || queryEndpoint_.isEmpty()){unavailable();return;}
            query_.start("pactl",{"--format=json","list","sinks"});
        });
        connect(&default_,&QProcess::errorOccurred,this,[this](QProcess::ProcessError){unavailable();});
#endif
        timer_.setInterval(200);connect(&timer_,&QTimer::timeout,this,[this]{refresh();});
        if(live_){timer_.start();QTimer::singleShot(0,this,[this]{refresh();});}
    }
    ~SystemVolumeControl() override {
#ifndef _WIN32
        for(auto *p:{&query_,&writer_,&default_})p->disconnect(this);
        for(auto *p:{&query_,&writer_,&default_})if(p->state()!=QProcess::NotRunning){p->kill();p->waitForFinished(250);}
#endif
    }
    QSlider *slider() const {return slider_;}
    // Public to permit isolated UI synchronization tests without touching audio.
    void applyRead(int percent,bool muted) {
        QSignalBlocker block(slider_);
        slider_->setRange(0,std::max(100,percent));slider_->setValue(percent);
        slider_->setEnabled(true);slider_->setToolTip(SC_TR("System volume"));caption(percent,muted);
    }
private:
    void caption(int v,bool muted){value_->setText(QLocale().toString(v)+QLocale().percent()+(muted?QStringLiteral(" · ")+SC_TR("Mute"):QString()));}
    void unavailable(){slider_->setEnabled(false);value_->clear();slider_->setToolTip(SC_TR("Selected audio device is unavailable"));}
    void refresh() {
        if(!live_ || pending_>=0 || slider_->isSliderDown())return;
#ifdef _WIN32
        try{const auto state=windowsSystemOutputVolume();applyRead(int(std::lround(state.level*100)),state.muted);}catch(const std::exception &){unavailable();}
#else
        if(default_.state()!=QProcess::NotRunning || query_.state()!=QProcess::NotRunning || writer_.state()!=QProcess::NotRunning)return;
        default_.start("pactl",{"get-default-sink"});
#endif
    }
    void write() {
#ifdef _WIN32
        const int v=pending_;pending_=-1;
        try{windowsSetSystemOutputVolume(float(v)/100);}catch(const std::exception &){unavailable();}
#else
        if(writer_.state()!=QProcess::NotRunning)return;
        const int v=pending_;pending_=-1;
        // Resolve the current default at execution time, including when Studio
        // switches from physical output to its virtual processing sink.
        writer_.start("pactl",{"set-sink-volume","@DEFAULT_SINK@",QString::number(v)+"%"});
#endif
    }
    bool live_;
    int pending_=-1;
    QSlider *slider_;
    QLabel *value_;
    QTimer timer_;
#ifndef _WIN32
    QProcess query_,writer_,default_;
    QString queryEndpoint_;
#endif
};
}
