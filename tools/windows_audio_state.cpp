// SPDX-License-Identifier: GPL-3.0-only
// Opt-in test journal. Snapshot/enumeration never opens capture streams.
#include "windows_audio.h"
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <wrl/client.h>
#include <set>
#include <cmath>
#include <iostream>
using Microsoft::WRL::ComPtr;
namespace {
void check(HRESULT hr) { if (FAILED(hr)) throw std::runtime_error("Audio state API failed"); }
ComPtr<IAudioEndpointVolume> volume(const QString &id) {
    ComPtr<IMMDeviceEnumerator> devices; ComPtr<IMMDevice> device;
    check(CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,IID_PPV_ARGS(&devices)));
    check(devices->GetDevice(id.toStdWString().c_str(),&device));
    ComPtr<IAudioEndpointVolume> result;
    check(device->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,nullptr,
                          reinterpret_cast<void **>(result.GetAddressOf())));
    return result;
}
QJsonObject snapshot() {
    QJsonArray roles, levels;
    for (bool capture : {false,true}) {
        for (int role=0;role<3;++role) {
            try { roles.append(QJsonObject{{"capture",capture},{"role",role},
                {"id",QString::fromStdWString(soundcurrent::windowsDefaultEndpointId(capture,role))}}); }
            catch (...) {} // No default is a legitimate initial state.
        }
        for (const auto &endpoint : soundcurrent::windowsAudioEndpoints(capture)) {
            const auto id=QString::fromStdWString(endpoint.id);auto control=volume(id);
            float scalar=0;BOOL muted=FALSE;check(control->GetMasterVolumeLevelScalar(&scalar));check(control->GetMute(&muted));
            levels.append(QJsonObject{{"id",id},{"capture",capture},{"level",double(scalar)},
                {"muted",bool(muted)},{"nativeRole",endpoint.soundCurrent?"playback":endpoint.soundCurrentMicFeed?"feed":endpoint.soundCurrentMicCapture?"capture":""}});
        }
    }
    return {{"schema",1},{"roles",roles},{"levels",levels}};
}
QJsonObject read(const QString &path) {
    QFile file(path);if(!file.open(QIODevice::ReadOnly)||file.size()>1024*1024)throw std::runtime_error("Cannot read bounded audio journal");
    QJsonParseError error;const auto data=QJsonDocument::fromJson(file.readAll(),&error).object();
    if(error.error!=QJsonParseError::NoError||data.value("schema").toInt()!=1||!data.value("roles").isArray()||!data.value("levels").isArray())throw std::runtime_error("Invalid audio journal");
    return data;
}
void restore(const QJsonObject &saved) {
    // Restore only recorded roles. A test environment with partially absent
    // defaults is rejected rather than inventing a microphone fallback.
    for (bool capture : {false,true}) {
        std::array<std::wstring,3> original;unsigned count=0;
        for(const auto &entry:saved.value("roles").toArray()) {
            const auto row=entry.toObject();if(row.value("capture").toBool()!=capture)continue;
            const int role=row.value("role").toInt(-1);if(role<0||role>2||!original[role].empty())throw std::runtime_error("Invalid/duplicate saved audio role");
            original[role]=row.value("id").toString().toStdWString();if(original[role].empty())throw std::runtime_error("Empty audio role");++count;
        }
        if(!count)continue;
        if(count!=3)throw std::runtime_error("Partially absent original roles need manual recovery");
        std::set<std::wstring> current;
        for(int role=0;role<3;++role)current.insert(soundcurrent::windowsDefaultEndpointId(capture,role));
        for(const auto &owned:current)if(!soundcurrent::windowsRestoreOwnedRoute(capture,owned,original[0],original))throw std::runtime_error("Audio role restoration failed");
    }
    for(const auto &entry:saved.value("levels").toArray()) {
        const auto row=entry.toObject();const double level=row.value("level").toDouble(-1);
        if(!std::isfinite(level)||level<0||level>1||!row.value("muted").isBool())throw std::runtime_error("Invalid saved endpoint level");
        auto control=volume(row.value("id").toString());
        check(control->SetMasterVolumeLevelScalar(float(level),nullptr));check(control->SetMute(row.value("muted").toBool(),nullptr));
    }
}
bool matches(const QJsonObject &saved) {
    const auto now=snapshot();
    if(now.value("roles")!=saved.value("roles"))return false;
    for(const auto &entry:saved.value("levels").toArray()) {
        const auto before=entry.toObject();bool found=false;
        for(const auto &candidate:now.value("levels").toArray()) {
            const auto after=candidate.toObject();if(after.value("id")!=before.value("id"))continue;
            found=true;
            if(after.value("muted")!=before.value("muted")||std::abs(after.value("level").toDouble()-before.value("level").toDouble())>0.0001)return false;
        }
        if(!found)return false;
    }
    return true;
}
}
int main(int argc,char **argv) {
    QCoreApplication app(argc,argv);const auto args=app.arguments();
    if(args.size()!=3||(args[1]!="--snapshot"&&args[1]!="--restore"&&args[1]!="--compare"))return 2;
    const HRESULT initialized=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    try {
        check(initialized);
        if(args[1]=="--snapshot") {
            QSaveFile file(args[2]);const auto bytes=QJsonDocument(snapshot()).toJson();
            if(!file.open(QIODevice::WriteOnly)||file.write(bytes)!=bytes.size()||!file.commit())throw std::runtime_error("Cannot save audio journal");
        }else {
            const auto saved=read(args[2]);if(args[1]=="--restore")restore(saved);
            if(!matches(saved))throw std::runtime_error("Audio state differs from journal");
        }
        std::cout<<"PASS: "<<args[1].toStdString()<<"\n";
        CoUninitialize();return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';if(SUCCEEDED(initialized))CoUninitialize();return 1;}
}
