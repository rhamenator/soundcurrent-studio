// SPDX-License-Identifier: GPL-3.0-only
// Opt-in Windows integration test: --run plays a quiet tone through VB-CABLE.
#define NOMINMAX
#include "windows_audio.h"
#include <windows.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <ksmedia.h>
#include <wrl/client.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <memory>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>

using Microsoft::WRL::ComPtr;
namespace {
void check(HRESULT result, const char *action) {
    if (FAILED(result)) {
        char message[160];
        std::snprintf(message, sizeof(message), "%s: 0x%08lX", action,
                      static_cast<unsigned long>(result));
        throw std::runtime_error(message);
    }
}
// Isolated-VM diagnostic only. A muted source otherwise makes software
// loopback correctly report silence; restore the source after the test.
class TestSourceVolume {
    ComPtr<IAudioEndpointVolume> volume_;
    float level_ = 1, testReferenceDb_ = 0; BOOL mute_ = FALSE;
public:
    explicit TestSourceVolume(const std::wstring &id) {
        ComPtr<IMMDeviceEnumerator> devices; ComPtr<IMMDevice> device;
        check(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
            IID_PPV_ARGS(devices.GetAddressOf())), "Volume enumerator");
        check(devices->GetDevice(id.c_str(), device.GetAddressOf()), "Volume source");
        check(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
            reinterpret_cast<void**>(volume_.GetAddressOf())), "Source volume");
        check(volume_->GetMasterVolumeLevelScalar(&level_), "Read source volume");
        check(volume_->GetMute(&mute_), "Read source mute");
        std::printf("Test source original volume %.3f, mute %d\n", level_, int(mute_));
        check(volume_->SetMasterVolumeLevelScalar(1, nullptr), "Set test source volume");
        check(volume_->GetMasterVolumeLevel(&testReferenceDb_), "Read test source dB");
        const auto unmute = volume_->SetMute(FALSE, nullptr);
        if (FAILED(unmute)) {
            volume_->SetMasterVolumeLevelScalar(level_, nullptr);
            check(unmute, "Unmute test source");
        }
    }
    void set(float level, BOOL mute) {
        check(volume_->SetMasterVolumeLevelScalar(level, nullptr), "Set test source volume");
        check(volume_->SetMute(mute, nullptr), "Set test source mute");
    }
    double relativeDecibels() const {
        float current = 0;
        check(volume_->GetMasterVolumeLevel(&current), "Read current source dB");
        return current - testReferenceDb_;
    }
    ~TestSourceVolume() { if(volume_) { volume_->SetMasterVolumeLevelScalar(level_, nullptr); volume_->SetMute(mute_, nullptr); } }
    float set(float level, bool mute) {
        check(volume_->SetMasterVolumeLevelScalar(level, nullptr), "Set diagnostic volume");
        check(volume_->SetMute(mute ? TRUE : FALSE, nullptr), "Set diagnostic mute");
        float db = 0; check(volume_->GetMasterVolumeLevel(&db), "Read diagnostic dB");
        return db;
    }
};
struct Measurement { double left = 0, right = 0; std::size_t frames = 0; };
class Probe {
public:
    Probe(const std::wstring &renderId, const std::wstring &loopbackId, bool microphone = false) {
        ComPtr<IMMDeviceEnumerator> devices;
        check(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                               IID_PPV_ARGS(devices.GetAddressOf())), "Enumerator");
        ComPtr<IMMDevice> renderDevice, loopbackDevice;
        check(devices->GetDevice(renderId.c_str(), renderDevice.GetAddressOf()), "Tone device");
        check(devices->GetDevice(loopbackId.c_str(), loopbackDevice.GetAddressOf()), "Loopback device");
        check(renderDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
              reinterpret_cast<void **>(render_.GetAddressOf())), "Tone client");
        check(loopbackDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
              reinterpret_cast<void **>(capture_.GetAddressOf())), "Loopback client");
        WAVEFORMATEX *raw = nullptr;
        check(render_->GetMixFormat(&raw), "Tone mix format");
        renderFormat_.reset(raw);
        check(capture_->GetMixFormat(&raw), "Loopback mix format");
        captureFormat_.reset(raw);
        for (const auto *f : {renderFormat_.get(), captureFormat_.get()}) {
            const bool floating = f->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
                (f->wFormatTag == WAVE_FORMAT_EXTENSIBLE && f->cbSize >= 22 &&
                 reinterpret_cast<const WAVEFORMATEXTENSIBLE *>(f)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
            if (!floating || f->wBitsPerSample != 32 || f->nChannels < 2)
                throw std::runtime_error("This diagnostic requires stereo or multichannel float mix formats");
        }
        std::printf("Tone mix: %lu Hz, %u channels; loopback: %lu Hz, %u channels\n",
                    renderFormat_->nSamplesPerSec, renderFormat_->nChannels,
                    captureFormat_->nSamplesPerSec, captureFormat_->nChannels);
        check(render_->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 1000000, 0,
                                   renderFormat_.get(), nullptr), "Tone initialize");
        check(capture_->Initialize(AUDCLNT_SHAREMODE_SHARED, microphone ? 0 : AUDCLNT_STREAMFLAGS_LOOPBACK,
                                    1000000, 0, captureFormat_.get(), nullptr), "Loopback initialize");
        check(render_->GetBufferSize(&capacity_), "Tone buffer size");
        check(render_->GetService(IID_PPV_ARGS(writer_.GetAddressOf())), "Tone writer");
        check(capture_->GetService(IID_PPV_ARGS(reader_.GetAddressOf())), "Loopback reader");
        check(capture_->Start(), "Start loopback");
        check(render_->Start(), "Start tone");
    }
    ~Probe() { if (render_) render_->Stop(); if (capture_) capture_->Stop(); }
    Measurement measure(double frequency, unsigned durationMs = 1600, double amplitude = .02) {
        Measurement result;
        const auto begin = GetTickCount64();
        while (GetTickCount64() - begin < durationMs) {
            UINT32 padding = 0;
            check(render_->GetCurrentPadding(&padding), "Tone padding");
            const UINT32 available = capacity_ - padding;
            if (available) {
                BYTE *raw = nullptr;
                check(writer_->GetBuffer(available, &raw), "Tone buffer");
                auto *samples = reinterpret_cast<float *>(raw);
                std::fill_n(samples, available * renderFormat_->nChannels, 0.0f);
                for (UINT32 frame = 0; frame < available; ++frame) {
                    const auto value = static_cast<float>(amplitude * std::sin(phase_));
                    phase_ = std::fmod(phase_ + 2 * std::numbers::pi * frequency /
                                       renderFormat_->nSamplesPerSec, 2 * std::numbers::pi);
                    samples[frame * renderFormat_->nChannels] = value;
                    samples[frame * renderFormat_->nChannels + 1] = value;
                }
                check(writer_->ReleaseBuffer(available, 0), "Release tone");
            }
            UINT32 frames = 0;
            check(reader_->GetNextPacketSize(&frames), "Loopback packet");
            while (frames) {
                BYTE *raw = nullptr;
                DWORD flags = 0;
                check(reader_->GetBuffer(&raw, &frames, &flags, nullptr, nullptr), "Loopback read");
                if (GetTickCount64() - begin >= 800) {
                    if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
                        const auto *samples = reinterpret_cast<const float *>(raw);
                        for (UINT32 frame = 0; frame < frames; ++frame) {
                            const float left = samples[frame * captureFormat_->nChannels];
                            const float right = samples[frame * captureFormat_->nChannels + 1];
                            result.left += left * left;
                            result.right += right * right;
                        }
                    }
                    result.frames += frames;
                }
                check(reader_->ReleaseBuffer(frames), "Release loopback");
                check(reader_->GetNextPacketSize(&frames), "Next loopback packet");
            }
            Sleep(3);
        }
        if (!result.frames) throw std::runtime_error("No physical-output loopback frames");
        result.left = std::sqrt(result.left / result.frames);
        result.right = std::sqrt(result.right / result.frames);
        return result;
    }
private:
    ComPtr<IAudioClient> render_, capture_;
    ComPtr<IAudioRenderClient> writer_;
    ComPtr<IAudioCaptureClient> reader_;
    std::unique_ptr<WAVEFORMATEX, decltype(&CoTaskMemFree)> renderFormat_{nullptr, CoTaskMemFree},
                                                         captureFormat_{nullptr, CoTaskMemFree};
    UINT32 capacity_ = 0;
    double phase_ = 0;
};
double decibels(double value, double reference) { return 20 * std::log10(value / reference); }
}

int main(int argc, char **argv) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    try {
        check(CoInitializeEx(nullptr, COINIT_MULTITHREADED), "COM");
        const bool native = argc == 2 && std::string(argv[1]) == "--run-native";
        const bool nativeMic = argc == 2 && std::string(argv[1]) == "--run-native-mic";
        std::wstring cableInput, cableOutput, speakers, nativePlayback, nativeFeed, nativeCapture;
        for (bool capture : {false, true}) {
            for (const auto &device : soundcurrent::windowsAudioEndpoints(capture)) {
                // Keep narrow console output independent of the Windows console code page.
                const std::string name(device.name.begin(), device.name.end());
                std::printf("%s: %s\n", capture ? "Capture" : "Render", name.c_str());
                std::wcout << L"  id=" << device.id << L" playback=" << device.soundCurrent
                    << L" feed=" << device.soundCurrentMicFeed << L" microphone=" << device.soundCurrentMicCapture << L'\n';
                if (!capture && device.soundCurrent) nativePlayback = device.id;
                if (!capture && device.soundCurrentMicFeed) nativeFeed = device.id;
                if (capture && device.soundCurrentMicCapture) nativeCapture = device.id;
                if (!capture && device.name.find(L"CABLE Input") != std::wstring::npos) cableInput = device.id;
                else if (capture && device.name.find(L"CABLE Output") != std::wstring::npos) cableOutput = device.id;
                else if (!capture && !device.virtualCable && device.id == soundcurrent::windowsDefaultOutputId()) speakers = device.id;
            }
        }
        const bool physicalLoopback = argc == 2 && std::string(argv[1]) == "--run-loopback-physical";
        const bool loopback = native || physicalLoopback || (argc == 2 && std::string(argv[1]) == "--run-loopback");
        if (argc != 2 || (std::string(argv[1]) != "--run" && !loopback && !nativeMic)) {
            std::puts("Use --run or --run-loopback in an isolated Windows test system to play a -34 dBFS test tone.");
            return 0;
        }
        if (nativeMic) {
            if (nativeFeed.empty() || nativeCapture.empty()) throw std::runtime_error("Native microphone endpoint identity missing");
            TestSourceVolume sourceVolume(nativeFeed);
            Probe probe(nativeFeed, nativeCapture, true);
            const auto measured = probe.measure(997, 4000);
            std::printf("Native microphone transport frames=%zu RMS L=%.6f R=%.6f\n", measured.frames, measured.left, measured.right);
            if (measured.left < .005 || measured.right < .005 || measured.left > .03 || measured.right > .03)
                throw std::runtime_error("Native microphone transport missing or incorrect signal level");
            const auto silence = probe.measure(997, 1600, 0);
            std::printf("Native microphone silence RMS L=%.8f R=%.8f\n", silence.left, silence.right);
            if (silence.left > 2.0/32768 || silence.right > 2.0/32768)
                throw std::runtime_error("Native microphone retained signal after producer silence");
            sourceVolume.set(1, true);
            const auto feedMuted = probe.measure(997);
            std::printf("Native feed muted RMS L=%.8f R=%.8f\n", feedMuted.left, feedMuted.right);
            if (feedMuted.left > 2.0/32768 || feedMuted.right > 2.0/32768)
                throw std::runtime_error("Native microphone feed mute did not silence capture");
            sourceVolume.set(1, false);
            TestSourceVolume captureVolume(nativeCapture);
            const auto captureFullDb = captureVolume.set(1, false);
            const auto captureFull = probe.measure(997);
            const auto captureHalfDb = captureVolume.set(.5f, false);
            const auto captureHalf = probe.measure(997);
            const auto captureChange = decibels(captureHalf.left, captureFull.left);
            std::printf("Native capture volume measured %.2f dB, endpoint reported %.2f dB\n", captureChange, captureHalfDb-captureFullDb);
            if (std::abs(captureChange-(captureHalfDb-captureFullDb)) > .7)
                throw std::runtime_error("Native microphone capture volume did not affect transport");
            captureVolume.set(.5f, true);
            const auto captureMuted = probe.measure(997);
            std::printf("Native capture muted RMS L=%.8f R=%.8f\n", captureMuted.left, captureMuted.right);
            if (captureMuted.left > 2.0/32768 || captureMuted.right > 2.0/32768)
                throw std::runtime_error("Native microphone capture mute did not silence transport");
            std::puts("PASS: native microphone feed reaches capture with bounded stereo signal");
            return 0;
        }
        if (native) {
            cableInput = nativePlayback; cableOutput = nativeCapture; speakers = nativeFeed;
            if (nativeFeed.empty() || nativeCapture.empty()) throw std::runtime_error("Native microphone endpoint identity missing");
            std::puts("Native isolated chain: playback -> Studio loopback bridge -> mic feed -> mic capture");
        }
        if (cableInput.empty() || cableOutput.empty() || speakers.empty())
            throw std::runtime_error("VB-CABLE and a physical stereo output are required");
        if (physicalLoopback) std::swap(cableInput, speakers);
        if (!physicalLoopback && !native) {
        std::array<std::wstring, 3> originalDefaults;
        for (int i = 0; i < 3; ++i) originalDefaults[i] = soundcurrent::windowsDefaultEndpointId(false, i);
        {
            soundcurrent::WindowsRouteLease route(false, cableInput, speakers, true);
            for (int i = 0; i < 3; ++i)
                if (soundcurrent::windowsDefaultEndpointId(false, i) != cableInput)
                    throw std::runtime_error("Automatic default routing failed");
        }
        for (int i = 0; i < 3; ++i)
            if (soundcurrent::windowsDefaultEndpointId(false, i) !=
                (originalDefaults[i] == cableInput ? speakers : originalDefaults[i]))
                throw std::runtime_error("Default route was not restored");
        }
        TestSourceVolume sourceVolume(cableInput);
        soundcurrent::WindowsBridge bridge;
        bridge.setStatusCallback([](const std::wstring &message) {
            std::printf("Bridge: %s\n", std::string(message.begin(), message.end()).c_str());
        });
        bridge.setProfile({}, 0, 0, true);
        if (!bridge.start(loopback ? cableInput : cableOutput, speakers, false, loopback)) throw std::runtime_error(bridge.error());
        Probe probe(cableInput, native ? nativeCapture : speakers, native);
        const auto flat = probe.measure(1000);
        std::printf("Captured meter samples: %zu\n", bridge.takeMeterPcm().size());
        std::printf("Flat frames: %zu, bridge peak: %.6f, RMS: %.6f / %.6f\n", flat.frames, bridge.peak(), flat.left, flat.right);
        if (!bridge.running() || flat.left < 0.0001 || flat.right < 0.0001)
            throw std::runtime_error("Flat route is silent");
        std::printf("Flat RMS: L %.6f, R %.6f\n", flat.left, flat.right);
        sourceVolume.set(0.5f, FALSE);
        const double expectedHalfVolumeDb = sourceVolume.relativeDecibels();
        const auto halfVolume = probe.measure(1000);
        const double halfVolumeDb = decibels(halfVolume.left, flat.left);
        std::printf("VB-CABLE 50%% endpoint volume: %.2f dB (endpoint reports %.2f)\n",
                    halfVolumeDb, expectedHalfVolumeDb);
        if (std::abs(halfVolumeDb - expectedHalfVolumeDb) > 1.0)
            throw std::runtime_error("VB-CABLE Windows Volume Control did not attenuate the signal");
        sourceVolume.set(1.0f, TRUE);
        const auto mutedVolume = probe.measure(1000);
        std::printf("VB-CABLE muted RMS: L %.6f, R %.6f\n", mutedVolume.left, mutedVolume.right);
        if (mutedVolume.left > flat.left * 0.01 || mutedVolume.right > flat.right * 0.01)
            throw std::runtime_error("VB-CABLE Windows mute did not silence the signal");
        sourceVolume.set(1.0f, FALSE);
        bridge.setProfile({}, 6, 0, true);
        const auto gain = probe.measure(1000);
        const double gainDb = decibels(gain.left, flat.left);
        std::printf("Live post gain: %.2f dB (expected +6)\n", gainDb);
        if (std::abs(gainDb - 6) > 0.5) throw std::runtime_error("Live gain change failed");
        const std::array bands{soundcurrent::EqBand{1000, -12, 1}};
        bridge.setProfile(bands, 0, 0, true);
        const auto cut = probe.measure(1000);
        const double cutDb = decibels(cut.left, flat.left);
        std::printf("Live 1 kHz EQ cut: %.2f dB (expected -12)\n", cutDb);
        if (std::abs(cutDb + 12) > 0.5) throw std::runtime_error("Live EQ change failed");
        bridge.setProfile(bands, 6, 75, false);
        const auto bypass = probe.measure(1000);
        const double bypassDb = decibels(bypass.left, flat.left);
        std::printf("Bypass: %.2f dB relative to Flat (expected 0)\n", bypassDb);
        if (std::abs(bypassDb) > 0.5) throw std::runtime_error("Bypass failed");
        bridge.setProfile({}, 0, -100, true);
        const auto balance = probe.measure(1000);
        // Native transport is PCM16: Windows float/PCM conversion can dither
        // a silent channel. Bound it by two 16-bit LSBs, rather than a float
        // threshold below one quantization step.
        const double silentLimit = native ? 2.0 / 32768 : flat.right * .001;
        std::printf("Full-left balance RMS: L %.6f, R %.6f\n", balance.left, balance.right);
        if (std::abs(decibels(balance.left, flat.left)) > 0.5 || balance.right > silentLimit)
            throw std::runtime_error("Live balance change failed");
        bridge.stop();
        bridge.setProfile({}, 0, 0, true);
        if (!bridge.start(loopback ? cableInput : cableOutput, speakers, false, loopback)) throw std::runtime_error("Bridge restart failed");
        const auto restarted = probe.measure(1000);
        if (!bridge.running() || std::abs(decibels(restarted.left, flat.left)) > 0.5)
            throw std::runtime_error("Restarted audio is silent or changed level");
        bridge.stop();
        constexpr int calibrationRate = 96000;
        soundcurrent::studio::EngineSettings studio;
        studio.channels.resize(2);
        const std::array<double, 4> identity{1,0,0,1};
        if (!bridge.setStudio(studio, identity) || !bridge.start(loopback ? cableInput : cableOutput, speakers, false, loopback))
            throw std::runtime_error("Studio bridge did not start: " + bridge.error());
        const auto studioFlat = probe.measure(1000);
        if (std::abs(decibels(studioFlat.left, flat.left)) > .5)
            throw std::runtime_error("Studio dry route changed level");
        studio.postGainDb = 6;
        if (!bridge.setStudio(studio, identity)) throw std::runtime_error("Studio live gain rejected");
        const auto studioGain = probe.measure(1000);
        if (std::abs(decibels(studioGain.left, studioFlat.left)-6) > .5)
            throw std::runtime_error("Studio live post gain failed");
        studio.postGainDb = 0; studio.channels[0].bands.push_back({1000,-12,1});
        bridge.setStudio(studio, identity); const auto studioCut = probe.measure(1000);
        if (std::abs(decibels(studioCut.left,studioFlat.left)+12) > .5 ||
            std::abs(decibels(studioCut.right,studioFlat.right)) > .5)
            throw std::runtime_error("Studio independent channel EQ failed");
        studio.channels[0].bands.clear(); studio.channels[1].muted = true;
        studio.delay={true,50,.3,.25}; studio.reverb={true,.5,.4,.15};
        bridge.setStudio(studio,identity); const auto effects = probe.measure(1000);
        if (effects.left < .0001 || effects.right > (native ? silentLimit : studioFlat.right * .001))
            throw std::runtime_error("Studio effects or mute failed");
        studio.bypass=true;bridge.setStudio(studio,identity);const auto studioBypass=probe.measure(1000);
        if(std::abs(decibels(studioBypass.left,studioFlat.left))>.5 ||
           std::abs(decibels(studioBypass.right,studioFlat.right))>.5)
            throw std::runtime_error("Studio bypass failed");
        auto incompatible=studio;incompatible.channels.resize(8);
        if(bridge.setStudio(incompatible,std::vector<double>(64)))throw std::runtime_error("Live incompatible layout accepted");
        if(!bridge.running())throw std::runtime_error("Rejected layout stopped playback");
        std::printf("Studio live: gain %.2f dB, channel cut %.2f dB; independent mute, effects, bypass and rejected-layout preservation passed.\n",
            decibels(studioGain.left,studioFlat.left),decibels(studioCut.left,studioFlat.left));
        bridge.stop();
        if (native) {
            bridge.setProfile({}, 0, 0, true);
            if (!bridge.start(cableInput, speakers, false, true)) throw std::runtime_error("Volume bridge restart failed");
            const auto fullDb = sourceVolume.set(1, false);
            const auto full = probe.measure(1000);
            const auto halfDb = sourceVolume.set(.5f, false);
            const auto half = probe.measure(1000);
            const auto change = decibels(half.left, full.left);
            std::printf("Native source volume measured %.2f dB, endpoint reported %.2f dB\n", change, halfDb-fullDb);
            if (std::abs(change-(halfDb-fullDb)) > .7) throw std::runtime_error("Native source volume did not affect transport");
            sourceVolume.set(.5f, true);
            const auto muted = probe.measure(1000);
            std::printf("Native source muted RMS L=%.8f R=%.8f\n", muted.left, muted.right);
            if (muted.left > silentLimit || muted.right > silentLimit) throw std::runtime_error("Native source mute did not silence transport");
            sourceVolume.set(1, false);
            const auto unmuted = probe.measure(1000);
            if (std::abs(decibels(unmuted.left, full.left)) > .5) throw std::runtime_error("Native unmute did not restore transport");
            const auto sustained = probe.measure(1000, 30000);
            std::printf("Native 30-second transport frames=%zu RMS L=%.6f R=%.6f\n", sustained.frames, sustained.left, sustained.right);
            if (sustained.frames < 48000*28 || sustained.frames > 48000*31 ||
                std::abs(decibels(sustained.left, full.left)) > .5 ||
                std::abs(decibels(sustained.right, full.right)) > .5 || !bridge.running())
                throw std::runtime_error("Native sustained transport failed");
            bridge.stop();
            std::puts("PASS: native source volume, mute and unmute affect the complete Studio transport");
        }
        std::vector<std::int16_t> stereo(calibrationRate * 2);
        for (int i = 0; i < calibrationRate; ++i) {
            const auto value = std::int16_t(std::lround(100 * std::sin(2 * std::numbers::pi * 1000 * i / calibrationRate)));
            stereo[i * 2] = value; stereo[i * 2 + 1] = value;
        }
        const auto started = std::chrono::steady_clock::now();
        soundcurrent::windowsPlayPcm(speakers, stereo, calibrationRate, 2);
        const auto seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
        std::printf("Stereo calibration duration: %.3f seconds (expected 1)\n", seconds);
        if (seconds < 0.85 || seconds > 1.5)
            throw std::runtime_error("Stereo calibration frame timing failed");
        std::puts("PASS: live route, post gain, EQ, bypass, balance, and restart");
        return 0;
    } catch (const std::exception &error) {
        std::fprintf(stderr, "FAIL: %s\n", error.what());
        return 1;
    }
}
