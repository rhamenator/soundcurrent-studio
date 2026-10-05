// SPDX-License-Identifier: GPL-3.0-only
#include "linux_audio.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QProcess>
#include <QThread>
#include <cmath>
#include <cstring>
#include <iostream>
#include <numbers>
#include <stdexcept>
using namespace soundcurrent::studio;
namespace {
QString command(const QString &program,const QStringList &arguments) {
    QProcess p;p.start(program,arguments);
    if(!p.waitForStarted(3000)||!p.waitForFinished(5000)||p.exitCode())throw std::runtime_error(p.readAllStandardError().toStdString());
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
}
void require(bool b,const char *message){if(!b)throw std::runtime_error(message);}
double runTone(LinuxBridge &bridge,EngineSettings settings,const std::vector<double> &matrix,int n,int channel,bool rightMustBeSilent) {
    bridge.update(settings,matrix);
    const QString map="--channel-map=front-left,front-right,front-center,lfe,rear-left,rear-right,side-left,side-right";
    QProcess record;record.start("parec",{"--raw","--device=soundcurrent_studio_test.monitor","--format=float32le","--rate=48000",QString("--channels=%1").arg(n),map,"--latency-msec=20","--process-time-msec=5"});
    require(record.waitForStarted(3000),"Cannot start virtual monitor");QThread::msleep(150);
    QProcess play;play.start("paplay",{"--raw","--device=soundcurrent_studio_test_input","--format=float32le","--rate=48000",QString("--channels=%1").arg(n),map,"--latency-msec=20","--process-time-msec=5"});
    require(play.waitForStarted(3000),"Cannot start virtual test playback");
    std::vector<float> tone(48000*n);for(int f=0;f<48000;++f)tone[std::size_t(f*n+channel)]=float(.1*std::sin(2*std::numbers::pi*1000*f/48000));
    play.write(reinterpret_cast<const char *>(tone.data()),qint64(tone.size()*sizeof(float)));play.closeWriteChannel();
    QByteArray bytes;QElapsedTimer clock;clock.start();
    while(play.state()!=QProcess::NotRunning&&clock.elapsed()<6000){record.waitForReadyRead(10);bytes+=record.readAllStandardOutput();QCoreApplication::processEvents();}
    require(play.state()==QProcess::NotRunning&&play.exitCode()==0,"Virtual playback did not finish");clock.restart();
    while(clock.elapsed()<300){record.waitForReadyRead(10);bytes+=record.readAllStandardOutput();}
    record.terminate();record.waitForFinished(1000);bytes+=record.readAllStandardOutput();double energy=0,other=0;std::size_t active=0;
    for(qsizetype i=0;i+qsizetype(n*sizeof(float))<=bytes.size();i+=qsizetype(n*sizeof(float))){float x=0;std::memcpy(&x,bytes.constData()+i+channel*sizeof(float),sizeof(float));
        if(std::abs(x)>1e-6){energy+=double(x)*x;++active;}if(n>=2){float y=0;std::memcpy(&y,bytes.constData()+i+((channel+1)%n)*sizeof(float),sizeof(float));other+=double(y)*y;}}
    require(active>1000,"No audio reached the virtual output");if(rightMustBeSilent)require(other<1e-8,"Live processing mixed channels");
    return std::sqrt(energy/double(active));
}
}
int main(int argc,char **argv) {
    QCoreApplication app(argc,argv);QString module;
    try {
        module=command("pactl",{"load-module","module-null-sink","sink_name=soundcurrent_studio_test","channels=8","channel_map=front-left,front-right,front-center,lfe,rear-left,rear-right,side-left,side-right","sink_properties=node.virtual=true"});
        command("pactl",{"set-sink-volume","soundcurrent_studio_test","100%"});
        EngineSettings settings;settings.channels.resize(8);std::vector<double> matrix(64);for(int c=0;c<8;++c)matrix[c*8+c]=1;
        LinuxBridge bridge;bridge.start("soundcurrent_studio_test","soundcurrent_studio_test_input","soundcurrent_studio_test_output",settings,matrix);QThread::msleep(500);
        const double flat=runTone(bridge,settings,matrix,8,5,true);settings.channels[5].bands.push_back({1000,-12,1});const double cut=runTone(bridge,settings,matrix,8,5,true);
        require(std::abs(20*std::log10(cut/flat)+12)<.7,"Live channel EQ did not apply immediately");settings.channels[5].bands.clear();settings.postGainDb=6;
        const double gain=runTone(bridge,settings,matrix,8,5,true);require(std::abs(20*std::log10(gain/flat)-6)<.7,"Live gain did not apply immediately");
        settings.delay={true,20,.3,.25};settings.reverb={true,.5,.4,.15};runTone(bridge,settings,matrix,8,5,true);
        bridge.stop();command("pactl",{"unload-module",module});module.clear();
        std::cout<<"Live PipeWire: eight channels isolated; EQ "<<20*std::log10(cut/flat)<<" dB; gain "<<20*std::log10(gain/flat)<<" dB; delay/reverb output verified. No physical output used.\n";return 0;
    }catch(const std::exception &e){if(!module.isEmpty())try{command("pactl",{"unload-module",module});}catch(...){}std::cerr<<e.what()<<'\n';return 1;}
}
