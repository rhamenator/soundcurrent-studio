// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <vector>

namespace soundcurrent::studio {
struct WaveFormat {
    unsigned sampleRate = 0, channels = 0;
    std::uint32_t channelMask = 0;
    std::uint64_t frames = 0;
};
// Bounded RIFF/WAVE reader. PCM16/24/32 and float32, including extensible.
// Samples are streamed in caller-sized blocks; metadata never sizes a file buffer.
class WaveReader {
public:
    explicit WaveReader(const std::filesystem::path &);
    const WaveFormat &format() const noexcept { return format_; }
    std::size_t read(std::span<float> interleaved);
private:
    std::ifstream file_;
    WaveFormat format_;
    unsigned bytesPerSample_ = 0;
    bool floating_ = false;
    std::uint64_t remaining_ = 0;
};
// The renderer creates a temporary output and publishes it only after finish.
class WaveWriter {
public:
    WaveWriter(const std::filesystem::path &, WaveFormat);
    void write(std::span<const float> interleaved);
    void finish();
private:
    std::ofstream file_;
    WaveFormat format_;
    std::uint64_t written_ = 0;
};
} // namespace soundcurrent::studio
