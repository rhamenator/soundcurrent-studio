// SPDX-License-Identifier: GPL-3.0-only
// Explicit-endpoint, silent WASAPI regression probe. Never changes defaults.
// Build: cl /EHsc windows_native_format_probe.cpp /link ole32.lib
// Usage: windows_native_format_probe.exe render|capture <MMDevice endpoint ID>
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <wrl/client.h>
#include <iostream>
#include <cstring>

using Microsoft::WRL::ComPtr;

static bool check(const char *label, HRESULT result) {
    std::cout << label << "=0x" << std::hex
              << static_cast<unsigned long>(result) << std::dec << '\n';
    return SUCCEEDED(result);
}

int wmain(int argc, wchar_t **argv) {
    if (argc != 3 || (wcscmp(argv[1], L"render") && wcscmp(argv[1], L"capture"))) {
        std::cerr << "Usage: probe render|capture <explicit endpoint ID>\n";
        return 2;
    }
    const bool capture = !wcscmp(argv[1], L"capture");
    if (!check("CoInitializeEx", CoInitializeEx(nullptr, COINIT_MULTITHREADED))) return 1;
    // Process lifetime bounds COM resources; all streams stop before returning.
    ComPtr<IMMDeviceEnumerator> enumerator;
    if (!check("Enumerator", CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_ALL, IID_PPV_ARGS(enumerator.GetAddressOf())))) return 1;
    ComPtr<IMMDevice> device;
    if (!check("GetDevice", enumerator->GetDevice(argv[2], device.GetAddressOf()))) return 1;
    ComPtr<IMMEndpoint> endpoint;
    EDataFlow flow = eAll;
    if (!check("Endpoint", device.As(&endpoint)) ||
        !check("GetDataFlow", endpoint->GetDataFlow(&flow)) ||
        flow != (capture ? eCapture : eRender)) return 1;

    ComPtr<IAudioEndpointVolume> volume;
    const HRESULT volumeResult = device->Activate(__uuidof(IAudioEndpointVolume),
        CLSCTX_ALL, nullptr, reinterpret_cast<void **>(volume.GetAddressOf()));
    check("EndpointVolume", volumeResult); // Report separately from stream support.
    ComPtr<IAudioClient> client;
    if (!check("AudioClient", device->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
        nullptr, reinterpret_cast<void **>(client.GetAddressOf())))) return 1;
    WAVEFORMATEX *mix = nullptr;
    if (!check("GetMixFormat", client->GetMixFormat(&mix))) return 1;
    std::cout << "mix tag=" << mix->wFormatTag << " channels=" << mix->nChannels
              << " rate=" << mix->nSamplesPerSec << " bits=" << mix->wBitsPerSample << '\n';
    WAVEFORMATEX *closest = nullptr;
    const HRESULT supported = client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, mix, &closest);
    check("MixFormatSupport", supported);
    CoTaskMemFree(closest);
    if (supported != S_OK) { CoTaskMemFree(mix); return 1; }
    const HRESULT initialized = client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0,
        1000000, 0, mix, nullptr);
    CoTaskMemFree(mix);
    if (!check("InitializeShared", initialized)) return 1;
    UINT32 frames = 0;
    if (!check("GetBufferSize", client->GetBufferSize(&frames))) return 1;
    std::cout << "bufferFrames=" << frames << '\n';
    if (!capture) {
        ComPtr<IAudioRenderClient> render;
        if (!check("RenderClient", client->GetService(IID_PPV_ARGS(render.GetAddressOf())))) return 1;
        BYTE *buffer = nullptr;
        if (!check("GetSilentBuffer", render->GetBuffer(frames, &buffer))) return 1;
        if (!check("ReleaseSilentBuffer", render->ReleaseBuffer(frames, AUDCLNT_BUFFERFLAGS_SILENT))) return 1;
    }
    if (!check("StartSilent", client->Start())) return 1;
    Sleep(200);
    if (!check("Stop", client->Stop())) return 1;

    // Also exercise the declared kernel format using extensible PCM. Shared-mode
    // mix-format acceptance above is distinct from this device-format check.
    WAVEFORMATEXTENSIBLE pcm{};
    pcm.Format = {WAVE_FORMAT_EXTENSIBLE, 2, 48000, 192000, 4, 16,
                  sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)};
    pcm.Samples.wValidBitsPerSample = 16;
    pcm.dwChannelMask = 3; // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT
    pcm.SubFormat = {1, 0, 0x10, {0x80, 0, 0, 0xaa, 0, 0x38, 0x9b, 0x71}};
    closest = nullptr;
    const HRESULT pcmResult = client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &pcm.Format, &closest);
    check("Extensible48kStereo16Support", pcmResult);
    CoTaskMemFree(closest);
    if (pcmResult != S_OK) return 1;
    std::cout << "PASS: format negotiation and silent shared stream start/stop\n";
    return 0;
}
