// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "dsp.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace soundcurrent::studio {

// Resource limits for this preview, rather than a fixed speaker layout.
inline constexpr std::size_t maxChannels = 256;
inline constexpr std::size_t maxStateBytes = 128 * 1024 * 1024;

struct DelaySettings {
    bool enabled = false;
    double milliseconds = 250;
    double feedback = 0.35;
    double mix = 0.2;
};
struct ReverbSettings {
    bool enabled = false;
    double decaySeconds = 1.5;
    double damping = 0.4;
    double mix = 0.15;
};
struct ChannelSettings {
    std::vector<EqBand> bands;
    double gainDb = 0;
    bool muted = false;
};
struct EngineSettings {
    std::vector<ChannelSettings> channels;
    DelaySettings delay;
    ReverbSettings reverb;
    double postGainDb = 0;
    bool automaticHeadroom = true;
    bool bypass = false;
};
struct ProcessReport {
    bool validBuffer = true;
    double peakBeforeClip = 0;
    std::uint64_t clippedSamples = 0;
    std::uint64_t invalidSamples = 0;
};

// No Qt, device ownership or OS routing. Construct/configure on a control
// thread, with processing stopped. process/reset do not allocate, lock or do I/O.
// Never call configure and process concurrently on the same instance.
class AudioEngine {
public:
    AudioEngine(int sampleRate, std::size_t channels);
    ~AudioEngine();
    AudioEngine(AudioEngine &&) noexcept;
    AudioEngine &operator=(AudioEngine &&) noexcept;
    AudioEngine(const AudioEngine &) = delete;
    AudioEngine &operator=(const AudioEngine &) = delete;
    // Invalid configurations leave the current settings and all state intact.
    bool configure(const EngineSettings &, std::string *error = nullptr);
    // Scalar updates may be applied by the audio thread between blocks.
    // They preserve filter/effect tails and perform no allocation or locking.
    bool setPostGainDb(double) noexcept;
    bool setChannelGainDb(std::size_t channel, double) noexcept;
    bool setChannelMuted(std::size_t channel, bool) noexcept;
    ProcessReport process(std::span<float> interleaved) noexcept;
    // Each plane must contain the same number of frames and be disjoint.
    ProcessReport processPlanar(std::span<const std::span<float>> planes) noexcept;
    void reset() noexcept;
    std::size_t channels() const noexcept;
    int sampleRate() const noexcept;
    std::span<const double> headroomDb() const noexcept;
    std::span<const float> channelPeaks() const noexcept;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// Explicit row-major matrix: output channel first, then input channel.
// Default maps matching indexes; surplus output channels are silent.
class ChannelRouter {
public:
    ChannelRouter(std::size_t inputs, std::size_t outputs);
    bool setMatrix(std::span<const double> weights);
    bool process(std::span<const float> input, std::span<float> output) const noexcept;
    std::size_t inputs() const noexcept { return inputs_; }
    std::size_t outputs() const noexcept { return outputs_; }
private:
    struct Connection { std::size_t input; double gain; };
    std::size_t inputs_, outputs_;
    std::vector<double> weights_;
    std::vector<Connection> connections_;
    std::array<std::size_t, maxChannels + 1> offsets_{};
};
} // namespace soundcurrent::studio
