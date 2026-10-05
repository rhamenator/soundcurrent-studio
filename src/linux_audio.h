// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "engine.h"
#include <memory>
#include <string>
namespace soundcurrent::studio {
// PipeWire callbacks run on a serialized thread loop. Control changes take
// that loop's lock; configuration never races with processing.
class LinuxBridge {
public:
    LinuxBridge();
    ~LinuxBridge();
    void start(const std::string &target, const std::string &sink, const std::string &output,
               const EngineSettings &, std::span<const double> matrix);
    void update(const EngineSettings &, std::span<const double> matrix);
    void stop();
    bool running() const;
    std::vector<float> levels() const;
    std::string error() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
