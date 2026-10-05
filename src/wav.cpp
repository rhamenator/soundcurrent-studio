// SPDX-License-Identifier: GPL-3.0-only
#include "wav.h"
#include "engine.h"
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace soundcurrent::studio {
namespace {
void require(bool okay, const char *message) { if (!okay) throw std::runtime_error(message); }
std::uint32_t little(const unsigned char *data, unsigned bytes) {
    std::uint32_t value = 0;
    for (unsigned i = 0; i < bytes; ++i) value |= std::uint32_t(data[i]) << (8 * i);
    return value;
}
void get(std::istream &file, void *data, std::size_t bytes) {
    require(bool(file.read(static_cast<char *>(data), static_cast<std::streamsize>(bytes))), "Truncated WAVE file");
}
void put(std::ostream &file, std::uint32_t value, unsigned bytes) {
    for (unsigned i = 0; i < bytes; ++i) file.put(static_cast<char>(value >> (8 * i)));
}
constexpr std::array<unsigned char, 12> guidTail{0,0,0x10,0,0x80,0,0,0xaa,0,0x38,0x9b,0x71};
}
WaveReader::WaveReader(const std::filesystem::path &path) : file_(path, std::ios::binary) {
    require(file_.is_open(), "Cannot open input WAVE file");
    file_.seekg(0, std::ios::end);
    const auto end = file_.tellg();
    require(end >= 12, "Input is too short for RIFF/WAVE");
    const auto fileSize = static_cast<std::uint64_t>(end);
    file_.seekg(0);
    std::array<unsigned char, 40> bytes{};
    get(file_, bytes.data(), 12);
    require(std::memcmp(bytes.data(), "RIFF", 4) == 0 && std::memcmp(bytes.data()+8, "WAVE", 4) == 0,
            "Only little-endian RIFF/WAVE is supported");
    const std::uint64_t riffEnd = std::uint64_t(little(bytes.data()+4, 4)) + 8;
    require(riffEnd >= 12 && riffEnd <= fileSize, "Invalid RIFF size");
    bool foundFormat = false, foundData = false;
    std::uint64_t dataOffset = 0;
    std::uint32_t dataSize = 0;
    unsigned bits = 0, alignment = 0, formatTag = 0;
    unsigned chunks = 0;
    for (std::uint64_t offset = 12; offset < riffEnd;) {
        require(++chunks <= 4096, "Excessive number of RIFF chunks");
        require(riffEnd-offset >= 8, "Truncated chunk header");
        file_.seekg(static_cast<std::streamoff>(offset));
        get(file_, bytes.data(), 8);
        const auto size = little(bytes.data()+4, 4);
        const auto start = offset + 8;
        require(size <= riffEnd-start, "Chunk extends beyond RIFF bounds");
        if (std::memcmp(bytes.data(), "fmt ", 4) == 0) {
            require(!foundFormat && size >= 16 && size <= 4096, "Missing, duplicate or oversized WAVE format");
            get(file_, bytes.data(), std::min<std::uint32_t>(size, 40));
            formatTag = little(bytes.data(), 2);
            format_.channels = little(bytes.data()+2, 2);
            format_.sampleRate = little(bytes.data()+4, 4);
            alignment = little(bytes.data()+12, 2);
            bits = little(bytes.data()+14, 2);
            if (formatTag == 0xfffe) {
                require(size >= 40 && little(bytes.data()+16, 2) >= 22 &&
                        std::uint32_t(little(bytes.data()+16, 2)) + 18 <= size,
                        "Truncated extensible WAVE format");
                const auto validBits = little(bytes.data()+18, 2);
                require(validBits > 0 && validBits <= bits, "Invalid valid-bit count");
                format_.channelMask = little(bytes.data()+20, 4);
                require(!format_.channelMask || std::popcount(format_.channelMask) == int(format_.channels),
                        "Speaker mask does not match channel count");
                require(std::memcmp(bytes.data()+28, guidTail.data(), guidTail.size()) == 0,
                        "Unsupported extensible WAVE subtype");
                formatTag = little(bytes.data()+24, 4);
                if (formatTag == 3) require(validBits == 32, "Invalid float WAVE format");
            }
            require(format_.channels > 0 && format_.channels <= maxChannels &&
                    format_.sampleRate >= 8000 && format_.sampleRate <= 384000,
                    "Unsupported WAVE rate or channel count");
            require((formatTag == 1 && (bits == 16 || bits == 24 || bits == 32)) ||
                    (formatTag == 3 && bits == 32), "Only PCM16/24/32 or float32 WAVE is supported");
            bytesPerSample_ = bits / 8;
            require(alignment == format_.channels * bytesPerSample_ &&
                    little(bytes.data()+8, 4) == format_.sampleRate * alignment,
                    "Invalid WAVE frame alignment or byte rate");
            floating_ = formatTag == 3;
            foundFormat = true;
        } else if (std::memcmp(bytes.data(), "data", 4) == 0) {
            require(!foundData, "Multiple WAVE data chunks are unsupported");
            dataOffset = start; dataSize = size; foundData = true;
        }
        offset = start + size + (size & 1);
        require(offset <= riffEnd, "Missing RIFF padding byte");
    }
    require(foundFormat && foundData && dataSize % alignment == 0, "Missing or incomplete WAVE audio");
    format_.frames = dataSize / alignment;
    remaining_ = format_.frames;
    file_.seekg(static_cast<std::streamoff>(dataOffset));
    require(bool(file_), "Cannot seek to WAVE audio");
}
std::size_t WaveReader::read(std::span<float> destination) {
    require(destination.size() % format_.channels == 0, "Invalid WAVE read buffer");
    const auto frames = static_cast<std::size_t>(std::min<std::uint64_t>(remaining_, destination.size()/format_.channels));
    std::array<unsigned char, 4> bytes{};
    for (std::size_t i = 0; i < frames * format_.channels; ++i) {
        get(file_, bytes.data(), bytesPerSample_);
        const auto raw = little(bytes.data(), bytesPerSample_);
        if (floating_) destination[i] = std::bit_cast<float>(raw);
        else {
            const auto sign = std::uint64_t(1) << (bytesPerSample_ * 8 - 1);
            const auto signedValue = raw & sign ? std::int64_t(raw) - std::int64_t(sign * 2) : std::int64_t(raw);
            destination[i] = static_cast<float>(double(signedValue) / double(sign));
        }
    }
    remaining_ -= frames;
    return frames;
}

WaveWriter::WaveWriter(const std::filesystem::path &path, WaveFormat format) : format_(format) {
    require(format.channels > 0 && format.channels <= maxChannels && format.sampleRate >= 8000 &&
            format.sampleRate <= 384000, "Invalid output WAVE format");
    require(!format.channelMask || std::popcount(format.channelMask) == int(format.channels), "Invalid output speaker mask");
    require(format.frames <= (std::numeric_limits<std::uint32_t>::max()-72ULL)/(format.channels*4ULL),
            "Output exceeds the RIFF/WAVE 4 GiB limit");
    const auto dataSize = static_cast<std::uint32_t>(format.frames * format.channels * 4);
    file_.open(path, std::ios::binary | std::ios::trunc);
    require(file_.is_open(), "Cannot create output WAVE file");
    file_.write("RIFF", 4); put(file_, 72 + dataSize, 4); file_.write("WAVEfmt ", 8); put(file_, 40, 4);
    put(file_, 0xfffe, 2); put(file_, format.channels, 2); put(file_, format.sampleRate, 4);
    put(file_, format.sampleRate * format.channels * 4, 4); put(file_, format.channels * 4, 2);
    put(file_, 32, 2); put(file_, 22, 2); put(file_, 32, 2); put(file_, format.channelMask, 4);
    put(file_, 3, 4);
    file_.write(reinterpret_cast<const char *>(guidTail.data()), guidTail.size());
    file_.write("fact", 4); put(file_, 4, 4); put(file_, static_cast<std::uint32_t>(format.frames), 4);
    file_.write("data", 4); put(file_, dataSize, 4);
    require(bool(file_), "Could not write WAVE header");
}
void WaveWriter::write(std::span<const float> source) {
    require(source.size() % format_.channels == 0 && source.size()/format_.channels <= format_.frames-written_,
            "WAVE output exceeds its declared length");
    for (auto sample : source) put(file_, std::bit_cast<std::uint32_t>(sample), 4);
    written_ += source.size()/format_.channels;
    require(bool(file_), "Could not write WAVE audio");
}
void WaveWriter::finish() {
    require(written_ == format_.frames, "Incomplete WAVE output");
    file_.flush(); require(bool(file_), "Could not flush WAVE output");
    file_.close(); require(!file_.fail(), "Could not close WAVE output");
}
} // namespace soundcurrent::studio
