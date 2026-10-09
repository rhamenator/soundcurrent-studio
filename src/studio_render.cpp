// SPDX-License-Identifier: GPL-3.0-only
#include "engine.h"
#include "cli_localization.h"
#include "wav.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string_view>
#include <sstream>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <system_error>
#endif

using namespace soundcurrent::studio;
namespace {
double number(std::string_view text) {
    double value = 0;
    const auto parsed = std::from_chars(text.data(), text.data()+text.size(), value);
    if (parsed.ec != std::errc() || parsed.ptr != text.data()+text.size() || !std::isfinite(value))
        throw std::runtime_error("Invalid finite numeric argument");
    return value;
}
std::vector<double> fields(std::string_view text, std::size_t count) {
    std::vector<double> result;
    while (true) {
        const auto colon = text.find(':');
        result.push_back(number(text.substr(0, colon)));
        if (colon == std::string_view::npos) break;
        text.remove_prefix(colon+1);
    }
    if (result.size() != count) throw std::runtime_error("Wrong number of colon-separated fields");
    return result;
}
std::size_t channel(double value, std::size_t count) {
    if (value < 1 || value > static_cast<double>(count) || value != std::floor(value))
        throw std::runtime_error("Channel indexes are one-based and must exist");
    return static_cast<std::size_t>(value)-1;
}
class NewOutput {
public:
    explicit NewOutput(std::filesystem::path destination) : final_(std::move(destination)) {
        if (std::filesystem::exists(final_)) throw std::runtime_error("Output already exists; choose a new filename");
        const auto parent = final_.parent_path().empty() ? std::filesystem::path(".") : final_.parent_path();
        std::random_device random;
        for (int attempt = 0; attempt < 32; ++attempt) {
            directory_ = parent / (".soundcurrent-render-" + std::to_string(random()) + "-" + std::to_string(random()));
            if (std::filesystem::create_directory(directory_)) {
                std::error_code error;
                std::filesystem::permissions(directory_, std::filesystem::perms::owner_all,
                                             std::filesystem::perm_options::replace, error);
#ifndef _WIN32
                if (error) { std::filesystem::remove(directory_); throw std::runtime_error("Cannot protect output staging directory"); }
#endif
                path_ = directory_ / "audio.wav";
                return;
            }
        }
        throw std::runtime_error("Cannot create output staging directory");
    }
    ~NewOutput() {
        std::error_code error;
        std::filesystem::remove(path_, error);
        std::filesystem::remove(directory_, error);
    }
    const std::filesystem::path &path() const { return path_; }
    void publish() {
        // Atomic no-replace publication, even if another process created the
        // requested filename during rendering. Both paths share a filesystem.
        std::error_code error;
        std::filesystem::create_hard_link(path_, final_, error);
        if (error) throw std::runtime_error("Cannot publish output: " + error.message() +
                                            "; choose a new name on a filesystem supporting hard links");
    }
private:
    std::filesystem::path final_, directory_, path_;
};
void help() {
    std::cout << soundcurrent::cli::text("SoundCurrent Studio offline renderer (no audio device required)") << '\n'
        << soundcurrent::cli::format("Usage: %1 [options]",
            {"soundcurrent-studio-render --input in.wav --output NEW.wav"}) << '\n'
        <<
        "  --language TAG         " << soundcurrent::cli::text("interface language; unsupported tags use English") << '\n' <<
        "  --output-channels N    " << soundcurrent::cli::text("1-256 output channels (default: input count)") << '\n' <<
        "  --route OUT:IN:DB      " << soundcurrent::cli::text("explicit matrix gain; using any route clears defaults") << '\n' <<
        "  --eq CH:HZ:DB:Q        " << soundcurrent::cli::text("peaking EQ for one output channel; repeat as needed") << '\n' <<
        "  --lowpass CH:HZ:Q     " << soundcurrent::cli::text("optional channel low-pass (e.g. LFE)") << '\n' <<
        "  --highpass CH:HZ:Q    " << soundcurrent::cli::text("optional channel high-pass") << '\n' <<
        "  --gain CH:DB           " << soundcurrent::cli::text("output channel trim, -60 to +24 dB") << '\n' <<
        "  --post-gain DB         " << soundcurrent::cli::text("overall post gain, -84 to +24 dB") << '\n' <<
        "  --delay-ms MS          1-2000 ms (default 250)\n"
        "  --delay-feedback F    0-0.9 (default .35)\n"
        "  --delay-mix F         wet fraction 0-1 (enables delay)\n"
        "  --reverb-decay SEC    .1-10 seconds (default 1.5)\n"
        "  --reverb-damping F    0-.95 (default .4)\n"
        "  --reverb-mix F        wet fraction 0-1 (enables reverb)\n"
        "  --tail SEC            append 0-30 seconds to render effect tails\n"
        "  --no-headroom         disable automatic EQ headroom\n"
        "  --bypass              bypass EQ, effects, gains and mute\n"
        "Input: PCM16/24/32 or float32 RIFF/WAVE. Output: float32 extensible WAVE.\n"
        << soundcurrent::cli::text("Channel indexes start at 1. Existing output files are never overwritten.") << '\n';
}
}

int renderMain(int argc, char **argv) {
    try {
        for (int i = 1; i + 1 < argc; ++i)
            if (std::string_view(argv[i]) == "--language") soundcurrent::cli::selectLanguage(argv[++i]);
        std::filesystem::path input, output;
        EngineSettings settings;
        double tail = 0;
        std::size_t outputChannels = 0;
        std::vector<std::vector<double>> eq, gains, routes, lowpass, highpass;
        for (int i = 1; i < argc; ++i) {
            const std::string_view option = argv[i];
            if (option == "--help") { help(); return 0; }
            if (option == "--no-headroom") { settings.automaticHeadroom = false; continue; }
            if (option == "--bypass") { settings.bypass = true; continue; }
            if (++i == argc) throw std::runtime_error("Missing option value");
            const std::string_view value = argv[i];
            if (option == "--language") continue;
            if (option == "--input") input = std::filesystem::path(std::u8string(value.begin(), value.end()));
            else if (option == "--output") output = std::filesystem::path(std::u8string(value.begin(), value.end()));
            else if (option == "--output-channels") outputChannels = channel(number(value), maxChannels)+1;
            else if (option == "--eq") eq.push_back(fields(value, 4));
            else if (option == "--lowpass") lowpass.push_back(fields(value, 3));
            else if (option == "--highpass") highpass.push_back(fields(value, 3));
            else if (option == "--gain") gains.push_back(fields(value, 2));
            else if (option == "--route") routes.push_back(fields(value, 3));
            else if (option == "--post-gain") settings.postGainDb = number(value);
            else if (option == "--delay-ms") settings.delay.milliseconds = number(value);
            else if (option == "--delay-feedback") settings.delay.feedback = number(value);
            else if (option == "--delay-mix") { settings.delay.mix = number(value); settings.delay.enabled = true; }
            else if (option == "--reverb-decay") settings.reverb.decaySeconds = number(value);
            else if (option == "--reverb-damping") settings.reverb.damping = number(value);
            else if (option == "--reverb-mix") { settings.reverb.mix = number(value); settings.reverb.enabled = true; }
            else if (option == "--tail") tail = number(value);
            else throw std::runtime_error("Unknown option: " + std::string(option));
        }
        if (input.empty() || output.empty()) { help(); return 1; }
        if (tail < 0 || tail > 30) throw std::runtime_error("Tail must be between 0 and 30 seconds");
        WaveReader reader(input);
        const auto source = reader.format();
        if (!outputChannels) outputChannels = source.channels;
        AudioEngine engine(static_cast<int>(source.sampleRate), outputChannels);
        settings.channels.resize(outputChannels);
        for (const auto &band : eq) {
            auto &bands = settings.channels[channel(band[0], outputChannels)].bands;
            if (bands.size() >= soundcurrent::kMaxProcessingBands) throw std::runtime_error("Too many EQ bands for one channel");
            bands.push_back({band[1], band[2], band[3]});
        }
        for (const auto &gain : gains) settings.channels[channel(gain[0], outputChannels)].gainDb = gain[1];
        for (auto type : {soundcurrent::FilterType::LowPass, soundcurrent::FilterType::HighPass}) {
            for (const auto &filter : type == soundcurrent::FilterType::LowPass ? lowpass : highpass) {
                auto &bands = settings.channels[channel(filter[0], outputChannels)].bands;
                if (bands.size() >= soundcurrent::kMaxProcessingBands) throw std::runtime_error("Too many EQ bands for one channel");
                bands.push_back({filter[1], 0, filter[2], type});
            }
        }
        std::string error;
        if (!engine.configure(settings, &error)) throw std::runtime_error(error);
        ChannelRouter router(source.channels, outputChannels);
        if (!routes.empty()) {
            std::vector<double> matrix(source.channels * outputChannels);
            for (const auto &route : routes) {
                if (route[2] < -120 || route[2] > 12) throw std::runtime_error("Route gain must be between -120 and +12 dB");
                matrix[channel(route[0], outputChannels) * source.channels + channel(route[1], source.channels)] =
                    std::pow(10.0, route[2] / 20);
            }
            if (!router.setMatrix(matrix)) throw std::runtime_error("Invalid routing matrix");
        }
        WaveFormat destination = source;
        destination.channels = static_cast<unsigned>(outputChannels);
        if (outputChannels != source.channels || !routes.empty()) destination.channelMask = 0;
        destination.frames += static_cast<std::uint64_t>(std::llround(tail * source.sampleRate));
        NewOutput staging(output);
        WaveWriter writer(staging.path(), destination);
        constexpr std::size_t blockFrames = 1024;
        std::vector<float> raw(blockFrames * source.channels), processed(blockFrames * outputChannels);
        ProcessReport total;
        for (std::uint64_t frame = 0; frame < destination.frames;) {
            const auto frames = static_cast<std::size_t>(std::min<std::uint64_t>(blockFrames, destination.frames-frame));
            auto inputBlock = std::span(raw).first(frames * source.channels);
            std::fill(inputBlock.begin(), inputBlock.end(), 0);
            reader.read(inputBlock);
            total.invalidSamples += std::count_if(inputBlock.begin(), inputBlock.end(),
                                                  [](float value) { return !std::isfinite(value); });
            auto outputBlock = std::span(processed).first(frames * outputChannels);
            if (!router.process(inputBlock, outputBlock)) throw std::runtime_error("Invalid routing buffer");
            const auto report = engine.process(outputBlock);
            if (!report.validBuffer) throw std::runtime_error("Invalid processing buffer");
            total.peakBeforeClip = std::max(total.peakBeforeClip, report.peakBeforeClip);
            total.clippedSamples += report.clippedSamples;
            total.invalidSamples += report.invalidSamples;
            writer.write(outputBlock);
            frame += frames;
        }
        writer.finish();
        staging.publish();
        std::ostringstream peak;
        peak << total.peakBeforeClip;
        std::cout << soundcurrent::cli::format("Rendered %1 -> %2 channels, %3 frames at %4 Hz.",
                      {std::to_string(source.channels), std::to_string(outputChannels),
                       std::to_string(destination.frames), std::to_string(source.sampleRate)}) << '\n'
                  << soundcurrent::cli::format("Peak before clipping: %1; clipped samples: %2; invalid samples: %3",
                      {peak.str(), std::to_string(total.clippedSamples), std::to_string(total.invalidSamples)}) << '\n';
        return 0;
    } catch (const std::exception &error) {
        std::cerr << soundcurrent::cli::renderError(error.what()) << '\n';
        return 1;
    }
}

#ifdef _WIN32
// Windows passes UTF-16 arguments. Convert once to the renderer's UTF-8
// boundary, without an ANSI-code-page round trip or replacement characters.
int wmain(int argc, wchar_t **wideArguments) {
    try {
        std::vector<std::string> arguments;
        arguments.reserve(argc);
        for (int i = 0; i < argc; ++i) {
            const auto length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                wideArguments[i], -1, nullptr, 0, nullptr, nullptr);
            if (!length) throw std::system_error(GetLastError(), std::system_category());
            std::string argument(length, '\0');
            if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wideArguments[i], -1,
                                     argument.data(), length, nullptr, nullptr))
                throw std::system_error(GetLastError(), std::system_category());
            argument.pop_back(); // The sizing call included the terminating NUL.
            arguments.push_back(std::move(argument));
        }
        std::vector<char *> pointers;
        pointers.reserve(argc);
        for (auto &argument : arguments) pointers.push_back(argument.data());
        return renderMain(argc, pointers.data());
    } catch (const std::exception &error) {
        std::cerr << soundcurrent::cli::renderError(error.what()) << '\n';
        return 1;
    }
}
#else
int main(int argc, char **argv) { return renderMain(argc, argv); }
#endif
