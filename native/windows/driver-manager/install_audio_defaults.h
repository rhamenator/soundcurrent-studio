// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <devicetopology.h>
#include <functiondiscoverykeys_devpkey.h>
#include <wrl/client.h>
#include <array>
#include <string>
#include <stdexcept>
#include "../../../src/windows_policy_config.h"
#include "../../../src/windows_endpoint_identity.h"

// Installation must not select the internal microphone feed/capture as a user's
// default device. Snapshot metadata only: never open a microphone audio stream.
class InstallAudioDefaults {
 using MicrosoftCom=Microsoft::WRL::ComPtr<IMMDeviceEnumerator>;
 MicrosoftCom devices;
 std::array<std::array<std::wstring,3>,2> before;
 bool apartment=false;
 static void check(HRESULT hr){if(FAILED(hr))throw std::runtime_error("Cannot preserve Windows audio defaults during driver setup");}
 std::wstring current(EDataFlow flow,ERole role){
  Microsoft::WRL::ComPtr<IMMDevice> device;
  const auto hr=devices->GetDefaultAudioEndpoint(flow,role,&device);
  if(hr==E_NOTFOUND)return {};
  check(hr);LPWSTR id=nullptr;check(device->GetId(&id));std::wstring result=id;CoTaskMemFree(id);return result;
 }
 bool native(IMMDevice* device,bool capture){
  Microsoft::WRL::ComPtr<IPropertyStore> store;
  if(FAILED(device->OpenPropertyStore(STGM_READ,&store)))return false;
  PROPVARIANT value;PropVariantInit(&value);
  const auto hr=store->GetValue(PKEY_DeviceInterface_FriendlyName,&value);
  const std::wstring name=SUCCEEDED(hr)&&value.vt==VT_LPWSTR?value.pwszVal:L"";
  PropVariantClear(&value);
  if(soundcurrent::managedEndpointKind(name)==soundcurrent::ManagedEndpointKind::Other)return false;
  Microsoft::WRL::ComPtr<IDeviceTopology> topology;Microsoft::WRL::ComPtr<IConnector> connector;
  LPWSTR id=nullptr;
  if(FAILED(device->Activate(__uuidof(IDeviceTopology),CLSCTX_ALL,nullptr,reinterpret_cast<void**>(topology.GetAddressOf())))||
     FAILED(topology->GetConnector(0,&connector))||FAILED(connector->GetDeviceIdConnectedTo(&id)))return false;
  const auto kind=soundcurrent::managedTopologyKind(name,id,capture);CoTaskMemFree(id);
  return kind!=soundcurrent::ManagedEndpointKind::Other;
 }
 void restoreOnce(){
  for(int f=0;f<2;++f)for(int r=0;r<3;++r){
   const auto& original=before[f][r];if(original.empty())continue;
   auto selected=current(static_cast<EDataFlow>(f),static_cast<ERole>(r));
   if(selected.empty()||selected==original)continue;
   Microsoft::WRL::ComPtr<IMMDevice> now,target;
   check(devices->GetDevice(selected.c_str(),&now));
   // Respect a user/another application's new physical/default choice.
   if(!native(now.Get(),f==eCapture))continue;
   check(devices->GetDevice(original.c_str(),&target));DWORD state=0;check(target->GetState(&state));
   if(!(state&DEVICE_STATE_ACTIVE))continue; // unplugged originals cannot be restored
   constexpr GUID clsid{0x870af99c,0x171d,0x4f9e,{0xaf,0x0d,0xe6,0x3d,0xf4,0x0c,0x2b,0xc9}};
   constexpr GUID iid{0xf8679f50,0x850a,0x41cf,{0x9c,0x72,0x43,0x0f,0x29,0x02,0x90,0xc8}};
   Microsoft::WRL::ComPtr<soundcurrent::PolicyConfig> policy;
   check(CoCreateInstance(clsid,nullptr,CLSCTX_ALL,iid,reinterpret_cast<void**>(policy.GetAddressOf())));
   check(policy->SetDefaultEndpoint(original.c_str(),static_cast<ERole>(r)));
   if(current(static_cast<EDataFlow>(f),static_cast<ERole>(r))!=original)
    throw std::runtime_error("Windows audio default restoration did not take effect");
  }
 }
public:
 InstallAudioDefaults(){
  const auto hr=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
  check(hr);apartment=true;
  try{
   check(CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,IID_PPV_ARGS(&devices)));
   for(int f=0;f<2;++f)for(int r=0;r<3;++r)before[f][r]=current(static_cast<EDataFlow>(f),static_cast<ERole>(r));
  }catch(...){devices.Reset();CoUninitialize();apartment=false;throw;}
 }
 ~InstallAudioDefaults(){devices.Reset();if(apartment)CoUninitialize();}
 void restore(){
  // Endpoint Builder publishes endpoints asynchronously after PnP binding. A
  // bounded settling interval catches delayed automatic default selection.
  for(int i=0;i<20;++i){restoreOnce();Sleep(250);}
  restoreOnce();
 }
};
