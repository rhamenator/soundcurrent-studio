// SPDX-License-Identifier: GPL-3.0-only
#include "system_volume_control.h"
#include <QApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QSaveFile>
#include <QElapsedTimer>
#include <QThread>
#include <QJsonDocument>
#include <iostream>
#include <stdexcept>
void check(bool ok,const char *why){if(!ok)throw std::runtime_error(why);}
int main(int argc,char **argv){QApplication app(argc,argv);try{
    QTemporaryDir temp;check(temp.isValid(),"no fixture directory");
    const auto statePath=temp.filePath("state.json");
    auto read=[&]{QFile f(statePath);check(f.open(QIODevice::ReadOnly),"read state");return QJsonDocument::fromJson(f.readAll()).object();};
    auto save=[&](QJsonObject s){QSaveFile f(statePath);check(f.open(QIODevice::WriteOnly),"write state");f.write(QJsonDocument(s).toJson());check(f.commit(),"save state");};
    save({{"endpoint","stereo"},{"percent",71},{"mute",true},{"writes",0},{"fail",false}});
    QFile script(temp.filePath("pactl"));check(script.open(QIODevice::WriteOnly),"write pactl fixture");script.write(R"PY(#!/usr/bin/python3
import json,os,sys
p=os.environ['SC_VOLUME_FIXTURE']
s=json.load(open(p));args=sys.argv[1:]
if s.get('fail'):sys.exit(1)
if args==['get-default-sink']:print(s['endpoint'])
elif args==['--format=json','list','sinks']:
 print(json.dumps([{'name':s['endpoint'],'mute':s['mute'],'volume':{'front-left':{'value':round(s['percent']*65536/100)},'front-right':{'value':round(s['percent']*65536/100)}}}]))
elif args[:2]==['set-sink-volume','@DEFAULT_SINK@']:
 s['percent']=int(args[2].removesuffix('%'));s['writes']+=1
 with open(p+'.tmp','w') as f:json.dump(s,f)
 os.replace(p+'.tmp',p)
else:sys.exit(2)
)PY");script.close();check(script.setPermissions(QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner),"fixture permissions");
    qputenv("SC_VOLUME_FIXTURE",statePath.toUtf8());qputenv("PATH",temp.path().toUtf8()+":"+qgetenv("PATH"));
    auto wait=[&](auto condition){QElapsedTimer t;t.start();while(t.elapsed()<4000){app.processEvents();if(condition())return;QThread::msleep(5);}throw std::runtime_error("volume synchronization timeout");};
    soundcurrent::SystemVolumeControl control(true);
    wait([&]{return control.slider()->isEnabled()&&control.slider()->value()==71;});
    check(read()["writes"].toInt()==0 && read()["mute"].toBool(),"startup wrote/unmuted volume");
    auto state=read();state["percent"]=37;save(state);
    wait([&]{return control.slider()->value()==37;}); // Media key/system UI -> app.
    check(read()["writes"].toInt()==0,"readback caused a feedback write");
    control.slider()->setValue(42);
    wait([&]{return read()["percent"].toInt()==42;}); // App -> system.
    check(read()["mute"].toBool(),"slider silently unmuted system");
    state=read();state["endpoint"]="new-route";state["percent"]=26;save(state);
    wait([&]{return control.slider()->value()==26;});
    state=read();state["fail"]=true;save(state);
    wait([&]{return !control.slider()->isEnabled();});
    state=read();state["fail"]=false;state["percent"]=55;save(state);
    wait([&]{return control.slider()->isEnabled()&&control.slider()->value()==55;});
    std::cout<<"PASS: isolated system/media-key readback, app write, no startup gain/mute changes, no feedback, default-route change and disconnect/recovery\n";
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
