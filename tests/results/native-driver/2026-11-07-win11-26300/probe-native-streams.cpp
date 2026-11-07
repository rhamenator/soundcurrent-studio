#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <wrl/client.h>
#include <iostream>
using Microsoft::WRL::ComPtr;
void hr(const char*label,HRESULT h){std::cout<<label<<"=0x"<<std::hex<<(unsigned long)h<<std::dec<<"\n";}
int wmain(){CoInitializeEx(nullptr,COINIT_MULTITHREADED);ComPtr<IMMDeviceEnumerator>e;HRESULT h=CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,IID_PPV_ARGS(e.GetAddressOf()));if(FAILED(h))return 1;
const wchar_t* ids[]={L"{0.0.0.00000000}.{6e82412c-4084-46e9-804f-bdca7ebe7329}",L"{0.0.0.00000000}.{3ff2d4ab-b99f-44f7-89c5-9ebbd7bdd8c8}",L"{0.0.1.00000000}.{17c656d0-1382-4ef7-9bf5-918555b2fb57}"};
for(int i=0;i<3;i++){std::wcout<<L"ENDPOINT "<<ids[i]<<L"\n";ComPtr<IMMDevice>d;h=e->GetDevice(ids[i],d.GetAddressOf());hr("GetDevice",h);if(FAILED(h))continue;ComPtr<IAudioEndpointVolume>v;h=d->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,nullptr,(void**)v.GetAddressOf());hr("EndpointVolume",h);
ComPtr<IAudioClient>c;h=d->Activate(__uuidof(IAudioClient),CLSCTX_ALL,nullptr,(void**)c.GetAddressOf());hr("AudioClient",h);if(FAILED(h))continue;WAVEFORMATEX*mix=nullptr;h=c->GetMixFormat(&mix);hr("GetMixFormat",h);if(SUCCEEDED(h)){std::cout<<"mix tag="<<mix->wFormatTag<<" channels="<<mix->nChannels<<" rate="<<mix->nSamplesPerSec<<" bits="<<mix->wBitsPerSample<<"\n";CoTaskMemFree(mix);}
WAVEFORMATEX f{};f.wFormatTag=WAVE_FORMAT_PCM;f.nChannels=2;f.nSamplesPerSec=48000;f.wBitsPerSample=16;f.nBlockAlign=4;f.nAvgBytesPerSec=192000;WAVEFORMATEX*closest=nullptr;h=c->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,&f,&closest);hr("Shared48kStereo16Support",h);if(closest)CoTaskMemFree(closest);
h=c->Initialize(AUDCLNT_SHAREMODE_SHARED,0,1000000,0,&f,nullptr);hr("InitializeShared",h);if(FAILED(h))continue;UINT32 frames=0;c->GetBufferSize(&frames);std::cout<<"bufferFrames="<<frames<<"\n";if(i<2){ComPtr<IAudioRenderClient>r;h=c->GetService(IID_PPV_ARGS(r.GetAddressOf()));hr("RenderClient",h);if(SUCCEEDED(h)){BYTE*p=nullptr;h=r->GetBuffer(frames,&p);hr("GetSilentBuffer",h);if(SUCCEEDED(h))hr("ReleaseSilentBuffer",r->ReleaseBuffer(frames,AUDCLNT_BUFFERFLAGS_SILENT));}}
h=c->Start();hr("StartSilent",h);if(SUCCEEDED(h)){Sleep(200);hr("Stop",c->Stop());}}
return 0;}
