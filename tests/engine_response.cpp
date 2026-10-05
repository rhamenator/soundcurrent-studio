// SPDX-License-Identifier: GPL-3.0-only
#include "engine.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <new>
#include <numbers>
#include <stdexcept>

// Guard only processing, after allocating and preparing all test buffers.
static std::atomic<bool> trackAllocations{false};
static std::atomic<std::size_t> allocations{0};
void *operator new(std::size_t size) {
    if (trackAllocations) ++allocations;
    if (auto *p = std::malloc(std::max<std::size_t>(1, size))) return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }
void operator delete[](void *p, std::size_t) noexcept { std::free(p); }

using namespace soundcurrent::studio;
namespace {
void check(bool okay, const char *message) { if (!okay) throw std::runtime_error(message); }
EngineSettings settingsFor(std::size_t count) { EngineSettings s; s.channels.resize(count); return s; }
bool near(double a, double b, double tolerance = 1e-6) { return std::abs(a-b) <= tolerance; }
void identityAndBounds() {
    for (const auto channels : {1, 2, 3, 6, 8, 16, 64, 256}) {
        AudioEngine engine(48000, channels);
        check(engine.configure(settingsFor(channels)), "Identity configuration rejected");
        std::vector<float> data(64*channels);
        for (std::size_t i = 0; i < data.size(); ++i) data[i] = float(int(i%53)-26)/100;
        const auto before = data;
        check(engine.process(data).validBuffer && data == before, "Identity changed or mixed channels");
    }
    for (auto count : {std::size_t(0), maxChannels+1}) {
        bool rejected = false;
        try { AudioEngine bad(48000, count); } catch (const std::invalid_argument &) { rejected = true; }
        check(rejected, "Invalid channel count accepted");
    }
    AudioEngine engine(48000, 6);
    std::vector<float> bad(7, .2f);
    const auto before = bad;
    check(!engine.process(bad).validBuffer && bad == before, "Partial frame changed memory");
    auto s = settingsFor(6);
    s.channels[3].gainDb = -6.020599913;
    check(engine.configure(s), "Channel trim rejected");
    std::vector<float> frame(6, .2f);
    engine.process(frame);
    for (int i = 0; i < 6; ++i) check(near(frame[i], i==3 ? .1 : .2), "Trim leaked to another channel");
    s.channels[3].gainDb = std::numeric_limits<double>::quiet_NaN();
    check(!engine.configure(s), "Nonfinite channel gain accepted");
    std::fill(frame.begin(), frame.end(), .2f); engine.process(frame);
    check(near(frame[3], .1), "Rejected configuration changed the active gain");
    AudioEngine huge(384000, 256);
    auto heavy = settingsFor(256); heavy.delay.enabled = true; heavy.delay.milliseconds = 2000;
    check(!huge.configure(heavy), "Unbounded delay allocation accepted");
}
void eqResponse() {
    constexpr int channels = 8, frames = 48000;
    AudioEngine engine(48000, channels);
    auto s = settingsFor(channels);
    s.channels[5].bands.push_back({1000, -12, 1});
    check(engine.configure(s), "Per-channel EQ rejected");
    std::vector<float> audio(channels*frames);
    for (int i = 0; i < frames; ++i) for (int c = 0; c < channels; ++c)
        audio[i*channels+c] = float(.1 * std::sin(2*std::numbers::pi*1000*i/48000));
    const auto report = engine.process(audio);
    check(report.validBuffer && !report.clippedSamples, "Quiet EQ tone clipped");
    double flat = 0, cut = 0;
    for (int i = frames/2; i < frames; ++i) {
        flat += audio[i*channels] * audio[i*channels];
        cut += audio[i*channels+5] * audio[i*channels+5];
        for (int c = 1; c < channels; ++c)
            if (c != 5) check(audio[i*channels+c] == audio[i*channels], "EQ altered another channel");
    }
    check(std::abs(10*std::log10(cut/flat)+12) < .1, "Per-channel 1 kHz cut has wrong response");
    const soundcurrent::EqBand lfe{120, 0, .707, soundcurrent::FilterType::LowPass};
    check(soundcurrent::filterResponseDb(lfe, 48000, 1000) < -36 &&
          soundcurrent::filterResponseDb(lfe, 48000, 40) > -.1, "LFE low-pass has wrong response");
    s.channels[5].bands[0].gainDb = 6;
    check(engine.configure(s) && near(engine.headroomDb()[5], -7, .03) && engine.headroomDb()[0]==0,
          "Headroom was not independent per channel");
    s.channels[5].bands[0].type = static_cast<soundcurrent::FilterType>(999);
    check(!engine.configure(s), "Unknown filter type accepted");
}
void delayAndLiveGain() {
    AudioEngine engine(48000, 6);
    auto s = settingsFor(6);
    s.delay = {true, 10, .5, 1};
    check(engine.configure(s), "Delay rejected");
    std::vector<float> impulse(6*1500);
    impulse[2] = .25f;
    engine.process(impulse);
    check(near(impulse[480*6+2], .25) && near(impulse[960*6+2], .125) &&
          near(impulse[1440*6+2], .0625), "Delay time or feedback is wrong");
    for (int i = 0; i < 1500; ++i) for (int c = 0; c < 6; ++c)
        if (c != 2) check(impulse[i*6+c]==0, "Delay crosstalk");
    engine.reset();
    std::array<float, 6> frame{0,0,.25f,0,0,0};
    engine.process(frame);
    s.postGainDb = 6.020599913;
    check(engine.configure(s), "Live gain rejected");
    std::vector<float> tail(480*6);
    engine.process(tail);
    check(near(tail[479*6+2], .5), "Gain update failed to preserve the existing delay tail");
    engine.reset();
    frame = {0,0,.25f,0,0,0}; engine.process(frame);
    trackAllocations = true;
    const bool updated = engine.setPostGainDb(0) && engine.setChannelGainDb(2, -6.020599913);
    std::fill(tail.begin(), tail.end(), 0); engine.process(tail);
    trackAllocations = false;
    check(updated && near(tail[479*6+2], .125) && allocations==0,
          "Scalar gain update lost the delay tail or allocated");
    s.bypass = true; s.channels[2].muted = true;
    check(engine.configure(s), "Bypass rejected");
    frame = {0,0,.25f,0,0,0}; engine.process(frame);
    check(near(frame[2], .25), "Bypass still applies effects, mute or gain");
    s.bypass = false; s.channels[2].muted = false; check(engine.configure(s), "Enable rejected");
    std::fill(tail.begin(), tail.end(), 0); engine.process(tail);
    check(std::all_of(tail.begin(), tail.end(), [](float f){return f==0;}), "Bypass restored stale tails");
}
void reverbAndBlocks() {
    constexpr std::size_t channels = 8, frames = 48000*3;
    auto s = settingsFor(channels);
    s.reverb = {true, .4, .2, 1};
    s.delay = {true, 17, .3, .25};
    AudioEngine whole(48000, channels), blocks(48000, channels), planar(48000, channels);
    check(whole.configure(s) && blocks.configure(s) && planar.configure(s), "Effects rejected");
    std::vector<float> reference(frames*channels); reference[4] = .1f;
    auto chunked = reference;
    std::vector<std::vector<float>> planeData(channels, std::vector<float>(frames)); planeData[4][0]=.1f;
    std::vector<std::span<float>> planes;
    for (auto &data : planeData) planes.push_back(data);
    trackAllocations = true;
    whole.process(reference);
    for (std::size_t i = 0; i < frames;) {
        const auto count = std::min<std::size_t>((i%137)+1, frames-i);
        blocks.process(std::span(chunked).subspan(i*channels, count*channels));
        i += count;
    }
    const auto planarReport = planar.processPlanar(planes);
    whole.reset();
    trackAllocations = false;
    check(allocations == 0, "Processing or reset allocated memory");
    check(planarReport.validBuffer, "Disjoint planar buffers rejected");
    if (reference != chunked) {
        for (std::size_t i = 0; i < reference.size(); ++i) if (reference[i] != chunked[i]) {
            std::fprintf(stderr, "First block difference: sample %zu full %.9g split %.9g\n", i, reference[i], chunked[i]);
            break;
        }
        check(false, "Output depends on block size");
    }
    double early = 0, late = 0;
    for (std::size_t i = 0; i < frames; ++i) for (std::size_t c = 0; c < channels; ++c) {
        check(reference[i*channels+c] == planeData[c][i], "Planar and interleaved processing differ");
        if (c != 4) check(reference[i*channels+c]==0, "Reverb crosstalk");
        if (i < 24000) early += reference[i*channels+c]*reference[i*channels+c];
        if (i > frames-24000) late += reference[i*channels+c]*reference[i*channels+c];
    }
    check(early > 1e-5 && late < early*1e-6, "Reverb tail is absent or fails to decay");
    planes[1] = planes[0];
    check(!planar.processPlanar(planes).validBuffer, "Aliased planes accepted");
}
void safetyAndRouting() {
    AudioEngine engine(48000, 3);
    std::array<float, 3> frame{2, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()};
    const auto result = engine.process(frame);
    check(result.clippedSamples==1 && result.invalidSamples==2 && result.peakBeforeClip==2 &&
          frame[0]==1 && frame[1]==0 && frame[2]==0, "Nonfinite/clipping handling is wrong");
    check(!engine.setChannelGainDb(3, 0) && !engine.setPostGainDb(25), "Invalid scalar gain accepted");
    check(engine.setChannelMuted(1, true), "Channel mute rejected");
    frame = {.2f,.2f,.2f}; engine.process(frame);
    check(frame[0]==.2f && frame[1]==0 && frame[2]==.2f, "Mute affected another channel");
    ChannelRouter route(6, 2);
    const std::array<double, 12> matrix{1,0,.5,0,.5,0, 0,1,.5,0,0,.5};
    check(route.setMatrix(matrix), "Routing matrix rejected");
    std::array<float, 6> input{.1f,.2f,.3f,.4f,.5f,.6f};
    std::array<float, 2> output{};
    check(route.process(input, output) && near(output[0], .5) && near(output[1], .65), "Explicit downmix is wrong");
    check(!route.process(input, std::span(input).first(2)), "Unsafe channel-count alias accepted");
    auto invalid = matrix; invalid[0] = std::numeric_limits<double>::quiet_NaN();
    check(!route.setMatrix(invalid) && route.process(input, output) && near(output[0], .5),
          "Rejected routing matrix changed active routes");
    ChannelRouter swap(2, 2);
    check(swap.setMatrix(std::array<double,4>{0,1,1,0}) && swap.process(output, output) && near(output[0], .65),
          "In-place channel swap is wrong");
}
}
int main() {
    try {
        identityAndBounds(); eqResponse(); delayAndLiveGain(); reverbAndBlocks(); safetyAndRouting();
        std::puts("PASS: 1-256 channels, independent EQ/gain, delay/reverb tails, routing, planar/block equivalence, bounds and allocation-free processing");
        return 0;
    } catch (const std::exception &error) {
        trackAllocations = false;
        std::fprintf(stderr, "FAIL: %s\n", error.what()); return 1;
    }
}
