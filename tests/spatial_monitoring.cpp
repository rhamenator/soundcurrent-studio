// SPDX-License-Identifier: GPL-3.0-only
#include "channel_meter_view.h"
#include "wav.h"
#include <QApplication>
#include <QTemporaryDir>
#include <QJsonArray>
#include <QKeyEvent>
#include <iostream>
#include <stdexcept>
using namespace soundcurrent::studio;
void check(bool ok,const char *why){if(!ok)throw std::runtime_error(why);}
int main(int argc,char **argv){QApplication app(argc,argv);try{
    QTemporaryDir temp;check(temp.isValid(),"temporary files unavailable");
    for(int n:{1,2,6,8,16,32,64,128,256}) {
        Session session(n);session.engine.automaticHeadroom=false;
        ChannelMeterView meters;meters.resize(800,350);meters.setSession(session);
        check(meters.channelCount()==n,"meter count lost");
        check(meters.spatial()==(n==6 || n==8),"unrecognized layout assigned room positions");
        for(int bank=0;bank<(n+63)/64;++bank) {
            meters.setBank(bank);
            for(int c=bank*64;c<std::min(n,(bank+1)*64);++c)check(!meters.channelRect(c).isEmpty(),"bank omitted channel");
            for(int a=0;a<n;++a)for(int b=a+1;b<n;++b)
                check(!meters.channelRect(a).intersects(meters.channelRect(b)),"channel meter cells overlap");
        }
        meters.setBank(0);
        // Unique input/channel impulse for a reversible all-channel permutation.
        std::vector<float> input(std::size_t(n)*n),output(input.size()),readback(input.size());
        std::vector<double> matrix(std::size_t(n)*n);
        for(int c=0;c<n;++c){input[std::size_t(c)*n+c]=.1f+c*.001f;matrix[std::size_t(c)*n+(n-1-c)]=1;}
        ChannelRouter router(n,n);check(router.setMatrix(matrix),"route rejected");check(router.process(input,output),"routing failed");
        AudioEngine engine(48000,n);check(engine.configure(session.effective()),"engine rejected layout");
        check(engine.process(output).validBuffer,"process failed");meters.setLevels(engine.channelPeaks());
        for(int f=0;f<n;++f)for(int c=0;c<n;++c)
            check(output[std::size_t(f)*n+c]==(f==n-1-c?.1f+f*.001f:0),"channel routed incorrectly");
        const auto filename=temp.filePath(QString("%1.wav").arg(n));
        {WaveWriter writer(filename.toStdString(),{48000,unsigned(n),0,std::uint64_t(n)});writer.write(output);writer.finish();}
        WaveReader reader(filename.toStdString());check(reader.format().channels==unsigned(n),"WAV channel count lost");
        check(reader.read(readback)==std::size_t(n),"WAV frame count lost");check(readback==output,"WAV changed channel signals");
        meters.show();app.processEvents();
        if(const auto path=qEnvironmentVariable("SOUNDCURRENT_SPATIAL_QA_DIR");!path.isEmpty())meters.grab().save(path+QString("/meters-%1.png").arg(n));
    }
    Session room(8);room.engine.preGainDb=-3;room.roomBalanceX=.5;room.roomBalanceY=.25;
    const auto settings=room.effective();
    check(std::abs(settings.channels[0].gainDb-20*std::log10(.5*.75))<1e-9,"front-left balance incorrect");
    check(settings.channels[3].gainDb==0 && !settings.channels[3].muted,"balance touched LFE");
    check(settings.channels[5].gainDb==0,"balance boosted/attenuated favored rear-right");
    check(std::abs(settings.channels[6].gainDb-20*std::log10(.5))<1e-9,"side channel balance incorrect");
    check(Session::parse(room.json()).json()==room.json(),"room balance not restored");
    auto oldInput=room.json();oldInput.remove("preGain");check(Session::parse(oldInput).engine.preGainDb==0,"legacy input gain not neutral");
    auto badInput=room.json();badInput["preGain"]=25;bool badGain=false;try{Session::parse(badInput);}catch(const std::exception &){badGain=true;}check(badGain,"invalid saved input gain accepted");
    auto malformed=room.json();malformed["roomBalance"]=QJsonObject{{"x",2},{"y",0}};
    bool rejected=false;try{Session::parse(malformed);}catch(const std::exception &){rejected=true;}check(rejected,"invalid room balance accepted");
    room.setCustomName(0,"Unknown output");check(!room.hasRoomLayout(),"custom layout incorrectly assigned speaker positions");
    ChannelMeterView custom;custom.setSession(room);check(!custom.spatial(),"unknown map presented as room");
    RoomBalancePad pad;pad.show();int edits=0;pad.onEdited=[&](double x,double y){++edits;check(x>=-1&&x<=1&&y>=-1&&y<=1,"pad out of bounds");};
    QKeyEvent right(QEvent::KeyPress,Qt::Key_Right,Qt::NoModifier);QApplication::sendEvent(&pad,&right);check(pad.position().x()>0 && edits==1,"keyboard balance failed");
    pad.setEnabled(false);QApplication::sendEvent(&pad,&right);check(edits==1,"locked pad edited");
    pad.setEnabled(true);QKeyEvent home(QEvent::KeyPress,Qt::Key_Home,Qt::NoModifier);QApplication::sendEvent(&pad,&home);check(pad.position()==QPointF(),"pad center reset failed");
    // Legacy positive boost retains its old limiter; negative values are new-format only.
    Session legacy;auto old=legacy.json();auto effects=old.value("enhancements").toArray();effects.removeLast();effects[soundcurrent::DynamicBoost]=.5;old["enhancements"]=effects;
    check(Session::parse(old).engine.enhancements.values[soundcurrent::Limiter]==1,"legacy limiter lost");
    effects[soundcurrent::DynamicBoost]=0;old["enhancements"]=effects;check(!Session::parse(old).engine.enhancements.active(),"legacy dry profile changed");
    std::cout<<"PASS: silent 1–256 channel identity/permutation/WAV, meter placement, spatial balance/LFE, keyboard/lock, state/legacy migration\n";
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
