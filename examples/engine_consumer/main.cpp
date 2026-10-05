// SPDX-License-Identifier: GPL-3.0-only
#include <engine.h>
#include <array>
#include <cstdio>

int main() {
    // A DAW owns its devices/clock. Give its bus buffers to this same library.
    soundcurrent::studio::AudioEngine bus(48000, 6);
    soundcurrent::studio::EngineSettings settings;
    settings.channels.resize(6);
    settings.channels[2].gainDb = -6.020599913;
    if (!bus.configure(settings)) return 1;
    std::array<float, 6> buffer{.1f,.2f,.3f,.4f,.5f,.6f};
    if (!bus.process(buffer).validBuffer || buffer[2] < .1499f || buffer[2] > .1501f || buffer[5] != .6f)
        return 2;
    std::puts("Imported SoundCurrent::Engine: independent six-channel bus processed without Qt or an audio device");
    return 0;
}
