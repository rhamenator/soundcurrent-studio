// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace soundcurrent {
// UI-only ballistics. Accumulate every incoming peak; publish at most 5 Hz.
// No dependence on the configurable graphical meter refresh interval.
class ReadablePeak {
public:
    void reset() noexcept { *this = {}; }
    bool update(double amplitude, std::int64_t nowMs) noexcept {
        const double db = 20 * std::log10(std::max(1e-6, amplitude));
        windowMax_ = std::max(windowMax_, db);
        if(db >= -1) clipUntil_ = nowMs + 1000;
        if(!initialized_) {
            initialized_=true; last_=nowMs; value_=db; windowMax_=-120; return true;
        }
        if(nowMs-last_<200) return false;
        const double elapsed=double(nowMs-last_);
        const double tau=windowMax_>value_?150:600;
        value_ += (windowMax_-value_)*(1-std::exp(-elapsed/tau));
        windowMax_=-120; last_=nowMs; return true;
    }
    double db() const noexcept { return value_; }
    bool clipping(std::int64_t nowMs) const noexcept { return nowMs<clipUntil_; }
private:
    bool initialized_=false;
    std::int64_t last_=0,clipUntil_=0;
    double value_=-120,windowMax_=-120;
};
}
