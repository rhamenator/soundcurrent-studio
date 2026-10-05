// SPDX-License-Identifier: GPL-3.0-only
#include "engine.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace soundcurrent::studio {
namespace {
bool range(double value, double low, double high) {
    return std::isfinite(value) && value >= low && value <= high;
}
bool overlap(const float *a, std::size_t an, const float *b, std::size_t bn) {
    const auto aa = reinterpret_cast<std::uintptr_t>(a), bb = reinterpret_cast<std::uintptr_t>(b);
    if (aa <= bb) return (bb - aa) / sizeof(float) < an;
    return (aa - bb) / sizeof(float) < bn;
}
struct Biquad {
    FilterCoefficients c{1, 0, 0, 0, 0};
    double z1 = 0, z2 = 0;
    double process(double x) noexcept {
        const auto y = c.b0 * x + z1;
        z1 = c.b1 * x - c.a1 * y + z2;
        z2 = c.b2 * x - c.a2 * y;
        if (std::abs(z1) < 1e-30) z1 = 0;
        if (std::abs(z2) < 1e-30) z2 = 0;
        return y;
    }
    void reset() noexcept { z1 = z2 = 0; }
};
struct Ring {
    std::vector<float> samples;
    std::size_t position = 0;
    double lowpass = 0;
    void prepare(std::size_t size) {
        if (samples.size() != size) { samples.assign(size, 0); position = 0; lowpass = 0; }
    }
    void reset() noexcept {
        std::fill(samples.begin(), samples.end(), 0); position = 0; lowpass = 0;
    }
    double read() const noexcept { return samples[position]; }
    void write(double value) noexcept {
        samples[position] = static_cast<float>(std::clamp(value, -16.0, 16.0));
        if (++position == samples.size()) position = 0;
    }
};
constexpr std::array<double, 4> combMs{29.7, 37.1, 41.1, 43.7};
constexpr std::array<double, 2> diffuserMs{5.0, 1.7};
std::size_t framesFor(double ms, int rate) {
    return std::max<std::size_t>(1, static_cast<std::size_t>(std::llround(ms * rate / 1000)));
}
double headroom(std::span<const Biquad> filters, int rate) {
    double peak = 0;
    for (int i = 0; i <= 512; ++i) {
        const auto frequency = std::min(20 * std::pow(1000.0, i / 512.0), rate * .45);
        const auto z = std::polar(1.0, -2 * std::numbers::pi * frequency / rate);
        double db = 0;
        for (const auto &f : filters) {
            const auto &c = f.c;
            db += 20 * std::log10(std::max(1e-15, std::abs(
                (c.b0 + c.b1*z + c.b2*z*z) / (1.0 + c.a1*z + c.a2*z*z))));
        }
        peak = std::max(peak, db);
    }
    return peak > .01 ? -(peak + 1) : 0;
}
}

struct AudioEngine::Impl {
    struct Channel {
        std::vector<Biquad> eq;
        Ring delay;
        std::array<Ring, 4> comb;
        std::array<Ring, 2> diffuser;
        std::array<double, 4> feedback{};
        double preamp = 1, trim = 1, outputGain = 1;
        bool muted = false;
        void reset() noexcept {
            for (auto &filter : eq) filter.reset();
            delay.reset();
            for (auto &ring : comb) ring.reset();
            for (auto &ring : diffuser) ring.reset();
        }
    };
    int rate;
    std::vector<Channel> state;
    std::vector<double> headrooms;
    std::vector<float> peaks;
    DelaySettings delay;
    ReverbSettings reverb;
    bool bypass = false;
    double postGain = 1;

    float sample(float input, std::size_t index, ProcessReport &report) noexcept {
        double value = input;
        if (!std::isfinite(value)) { value = 0; ++report.invalidSamples; }
        auto &channel = state[index];
        if (!bypass) {
            for (auto &filter : channel.eq) value = filter.process(value);
            value *= channel.preamp;
            if (!std::isfinite(value)) { channel.reset(); value = 0; ++report.invalidSamples; }
            if (delay.enabled && delay.mix > 0) {
                const auto delayed = channel.delay.read();
                channel.delay.write(value + delayed * delay.feedback);
                value = value * (1 - delay.mix) + delayed * delay.mix;
            }
            if (reverb.enabled && reverb.mix > 0) {
                double wet = 0;
                for (std::size_t i = 0; i < channel.comb.size(); ++i) {
                    auto &ring = channel.comb[i];
                    const auto delayed = ring.read();
                    ring.lowpass = reverb.damping * ring.lowpass + (1 - reverb.damping) * delayed;
                    ring.write(value * .25 + ring.lowpass * channel.feedback[i]);
                    wet += delayed;
                }
                for (auto &ring : channel.diffuser) {
                    const auto delayed = ring.read();
                    const auto output = delayed - .5 * wet;
                    ring.write(wet + .5 * output);
                    wet = output;
                }
                value = value * (1 - reverb.mix) + wet * reverb.mix;
            }
            value = channel.muted ? 0 : value * channel.outputGain;
        }
        if (!std::isfinite(value)) { channel.reset(); value = 0; ++report.invalidSamples; }
        report.peakBeforeClip = std::max(report.peakBeforeClip, std::abs(value));
        peaks[index] = std::max(peaks[index], static_cast<float>(std::min(
            std::abs(value), static_cast<double>(std::numeric_limits<float>::max()))));
        if (std::abs(value) > 1) ++report.clippedSamples;
        return static_cast<float>(std::clamp(value, -1.0, 1.0));
    }
};

AudioEngine::AudioEngine(int sampleRate, std::size_t channels) {
    if (sampleRate < 8000 || sampleRate > 384000 || !channels || channels > maxChannels)
        throw std::invalid_argument("Unsupported sample rate or channel count");
    impl_ = std::make_unique<Impl>();
    impl_->rate = sampleRate;
    impl_->state.resize(channels);
    impl_->headrooms.resize(channels);
    impl_->peaks.resize(channels);
}
AudioEngine::~AudioEngine() = default;
AudioEngine::AudioEngine(AudioEngine &&) noexcept = default;
AudioEngine &AudioEngine::operator=(AudioEngine &&) noexcept = default;
std::size_t AudioEngine::channels() const noexcept { return impl_->state.size(); }
int AudioEngine::sampleRate() const noexcept { return impl_->rate; }
std::span<const double> AudioEngine::headroomDb() const noexcept { return impl_->headrooms; }
std::span<const float> AudioEngine::channelPeaks() const noexcept { return impl_->peaks; }

bool AudioEngine::configure(const EngineSettings &settings, std::string *error) {
    const auto reject = [&](const char *message) { if (error) *error = message; return false; };
    if (settings.channels.size() != channels()) return reject("Channel configuration count does not match engine");
    if (!range(settings.postGainDb, -24, 24)) return reject("Post gain must be finite and within -24 to +24 dB");
    const auto &delay = settings.delay;
    const auto &reverb = settings.reverb;
    if (!range(delay.milliseconds, 1, 2000) || !range(delay.feedback, 0, .9) || !range(delay.mix, 0, 1))
        return reject("Delay settings are outside the supported range");
    if (!range(reverb.decaySeconds, .1, 10) || !range(reverb.damping, 0, .95) || !range(reverb.mix, 0, 1))
        return reject("Reverb settings are outside the supported range");
    for (const auto &channel : settings.channels) {
        if (!range(channel.gainDb, -60, 24) || channel.bands.size() > kMaxProcessingBands)
            return reject("Invalid channel gain or too many EQ bands");
        for (const auto &band : channel.bands) {
            if (!range(band.frequency, 20, 20000) || band.frequency >= sampleRate() * .45 ||
                !range(band.gainDb, -24, 24) || !range(band.q, .1, 20) ||
                (band.type != FilterType::Peaking && band.type != FilterType::LowShelf &&
                 band.type != FilterType::HighShelf && band.type != FilterType::HighPass &&
                 band.type != FilterType::LowPass))
                return reject("Invalid EQ band");
        }
    }
    std::size_t preparationBytes = channels() * (2 * sizeof(Impl::Channel) +
                                                  2 * sizeof(double) + sizeof(float));
    for (std::size_t i = 0; i < channels(); ++i) {
        const auto &channel = impl_->state[i];
        preparationBytes += (channel.eq.capacity() + settings.channels[i].bands.size()) * sizeof(Biquad);
        const auto account = [&](const Ring &ring, std::size_t length) {
            preparationBytes += ring.samples.capacity() * sizeof(float);
            if (ring.samples.size() != length) preparationBytes += length * sizeof(float);
        };
        account(channel.delay, delay.enabled && delay.mix > 0 ? framesFor(delay.milliseconds, sampleRate()) : 0);
        for (std::size_t n = 0; n < combMs.size(); ++n)
            account(channel.comb[n], reverb.enabled && reverb.mix > 0
                ? framesFor(combMs[n] + (i % 7) * .7, sampleRate()) : 0);
        for (std::size_t n = 0; n < diffuserMs.size(); ++n)
            account(channel.diffuser[n], reverb.enabled && reverb.mix > 0 ? framesFor(diffuserMs[n], sampleRate()) : 0);
    }
    if (preparationBytes > maxStateBytes)
        return reject("Effects exceed the preview's 128 MiB state budget");
    try {
        std::vector<Impl::Channel> next(channels());
        auto headrooms = impl_->headrooms;
        for (std::size_t i = 0; i < channels(); ++i) {
            auto &channel = next[i];
            const auto &profile = settings.channels[i];
            channel.eq.resize(profile.bands.size());
            for (std::size_t band = 0; band < std::min(channel.eq.size(), impl_->state[i].eq.size()); ++band)
                channel.eq[band] = impl_->state[i].eq[band];
            for (std::size_t band = 0; band < profile.bands.size(); ++band)
                channel.eq[band].c = filterCoefficients(profile.bands[band], sampleRate());
            headrooms[i] = settings.automaticHeadroom ? headroom(channel.eq, sampleRate()) : 0;
            channel.preamp = std::pow(10.0, headrooms[i] / 20);
            channel.trim = std::pow(10.0, profile.gainDb / 20);
            channel.outputGain = channel.trim * std::pow(10.0, settings.postGainDb / 20);
            channel.muted = profile.muted;
            const auto delayLength = delay.enabled && delay.mix > 0 ? framesFor(delay.milliseconds, sampleRate()) : 0;
            if (delayLength != impl_->state[i].delay.samples.size()) channel.delay.prepare(delayLength);
            for (std::size_t n = 0; n < combMs.size(); ++n) {
                const auto length = framesFor(combMs[n] + (i % 7) * .7, sampleRate());
                const auto combLength = reverb.enabled && reverb.mix > 0 ? length : 0;
                if (combLength != impl_->state[i].comb[n].samples.size()) channel.comb[n].prepare(combLength);
                channel.feedback[n] = std::pow(10.0, -3.0 * static_cast<double>(length) / (sampleRate() * reverb.decaySeconds));
            }
            for (std::size_t n = 0; n < diffuserMs.size(); ++n) {
                const auto length = reverb.enabled && reverb.mix > 0 ? framesFor(diffuserMs[n], sampleRate()) : 0;
                if (length != impl_->state[i].diffuser[n].samples.size()) channel.diffuser[n].prepare(length);
            }
        }
        // Commit only after all validation and allocation succeeded. Transfer
        // unchanged delay lines rather than copying potentially large tails.
        for (std::size_t i = 0; i < channels(); ++i) {
            auto &channel = next[i];
            auto &old = impl_->state[i];
            const auto preserve = [](Ring &proposed, Ring &previous, std::size_t length) {
                if (previous.samples.size() == length) std::swap(proposed, previous);
            };
            preserve(channel.delay, old.delay,
                     delay.enabled && delay.mix > 0 ? framesFor(delay.milliseconds, sampleRate()) : 0);
            for (std::size_t n = 0; n < combMs.size(); ++n)
                preserve(channel.comb[n], old.comb[n], reverb.enabled && reverb.mix > 0
                    ? framesFor(combMs[n] + (i % 7) * .7, sampleRate()) : 0);
            for (std::size_t n = 0; n < diffuserMs.size(); ++n)
                preserve(channel.diffuser[n], old.diffuser[n], reverb.enabled && reverb.mix > 0
                    ? framesFor(diffuserMs[n], sampleRate()) : 0);
            if (impl_->bypass != settings.bypass) channel.reset();
        }
        impl_->state.swap(next);
        impl_->headrooms.swap(headrooms);
        impl_->delay = delay;
        impl_->reverb = reverb;
        impl_->bypass = settings.bypass;
        impl_->postGain = std::pow(10.0, settings.postGainDb / 20);
        if (error) error->clear();
        return true;
    } catch (const std::bad_alloc &) { return reject("Could not allocate effect state"); }
}

bool AudioEngine::setPostGainDb(double db) noexcept {
    if (!range(db, -24, 24)) return false;
    impl_->postGain = std::pow(10.0, db / 20);
    for (auto &channel : impl_->state) channel.outputGain = channel.trim * impl_->postGain;
    return true;
}
bool AudioEngine::setChannelGainDb(std::size_t channel, double db) noexcept {
    if (channel >= channels() || !range(db, -60, 24)) return false;
    auto &state = impl_->state[channel];
    state.trim = std::pow(10.0, db / 20);
    state.outputGain = state.trim * impl_->postGain;
    return true;
}
bool AudioEngine::setChannelMuted(std::size_t channel, bool muted) noexcept {
    if (channel >= channels()) return false;
    impl_->state[channel].muted = muted;
    return true;
}

ProcessReport AudioEngine::process(std::span<float> data) noexcept {
    ProcessReport report;
    if (data.size() % channels()) { report.validBuffer = false; return report; }
    std::fill(impl_->peaks.begin(), impl_->peaks.end(), 0);
    for (std::size_t i = 0; i < data.size(); ++i) data[i] = impl_->sample(data[i], i % channels(), report);
    return report;
}
ProcessReport AudioEngine::processPlanar(std::span<const std::span<float>> planes) noexcept {
    ProcessReport report;
    if (planes.size() != channels()) { report.validBuffer = false; return report; }
    for (std::size_t c = 0; c < channels(); ++c) {
        if (planes[c].size() != planes[0].size()) { report.validBuffer = false; return report; }
        for (std::size_t p = 0; p < c; ++p)
            if (overlap(planes[c].data(), planes[c].size(), planes[p].data(), planes[p].size())) {
                report.validBuffer = false; return report;
            }
    }
    std::fill(impl_->peaks.begin(), impl_->peaks.end(), 0);
    for (std::size_t c = 0; c < channels(); ++c)
        for (auto &sample : planes[c]) sample = impl_->sample(sample, c, report);
    return report;
}
void AudioEngine::reset() noexcept {
    for (auto &channel : impl_->state) channel.reset();
    std::fill(impl_->peaks.begin(), impl_->peaks.end(), 0);
}

ChannelRouter::ChannelRouter(std::size_t inputs, std::size_t outputs) : inputs_(inputs), outputs_(outputs) {
    if (!inputs || !outputs || inputs > maxChannels || outputs > maxChannels)
        throw std::invalid_argument("Unsupported routing channel count");
    weights_.resize(inputs * outputs);
    for (std::size_t c = 0; c < std::min(inputs, outputs); ++c) weights_[c * inputs + c] = 1;
}
bool ChannelRouter::setMatrix(std::span<const double> weights) {
    if (weights.size() != weights_.size() ||
        !std::all_of(weights.begin(), weights.end(), [](double w) { return range(w, -4, 4); })) return false;
    std::copy(weights.begin(), weights.end(), weights_.begin());
    return true;
}
bool ChannelRouter::process(std::span<const float> input, std::span<float> output) const noexcept {
    if (input.size() % inputs_ || output.size() % outputs_ || input.size() / inputs_ != output.size() / outputs_)
        return false;
    if (overlap(input.data(), input.size(), output.data(), output.size()) &&
        (input.data() != output.data() || inputs_ != outputs_)) return false;
    std::array<double, maxChannels> frame{};
    for (std::size_t f = 0; f < input.size() / inputs_; ++f) {
        for (std::size_t c = 0; c < inputs_; ++c)
            frame[c] = std::isfinite(input[f * inputs_ + c]) ? input[f * inputs_ + c] : 0;
        for (std::size_t c = 0; c < outputs_; ++c) {
            double value = 0;
            for (std::size_t in = 0; in < inputs_; ++in) value += frame[in] * weights_[c * inputs_ + in];
            output[f * outputs_ + c] = static_cast<float>(std::clamp(value,
                -static_cast<double>(std::numeric_limits<float>::max()),
                 static_cast<double>(std::numeric_limits<float>::max())));
        }
    }
    return true;
}
} // namespace soundcurrent::studio
