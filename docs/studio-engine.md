# SoundCurrent Studio engine 0.1

This preview separates processing from device access. It supports 1–256 logical
channels at 8–384 kHz, with independent state on every channel. The desktop application uses this engine for Studio playback through its native
PipeWire and WASAPI adapters. The shared EQ, channel filters, routing, delay and
reverb feed the same processing path.

## Processing

The engine processes interleaved or planar floating-point buffers in this order:

1. Per-channel EQ: up to 64 peaking, shelf, high-pass or low-pass bands.
2. Optional automatic EQ headroom, calculated independently for each channel.
3. Feedback delay, then algorithmic reverb.
4. Channel trim/mute and overall post gain.
5. Output clipping to −1 through +1, with peak and clipping statistics.

Delay and reverb settings currently apply to all channels; each channel has
separate delay lines and reverb state. There is no implicit crossfeed, upmixing or
speaker-position mapping. Reverb uses damped feedback combs and all-pass
diffusers. Delay supports 1–2000 ms, feedback up to 0.9 and an adjustable wet mix.
Reverb supports a 0.1–10 second decay, damping and an adjustable wet mix.

Channel trim ranges from −60 to +24 dB and post gain from −24 to +24 dB.
Bypass ignores EQ, effects, trim and mute; it still sanitizes invalid samples and
bounds output. Switching bypass clears effect tails. Invalid floating-point
samples become silence and are counted. Peaks are measured before final clipping.

`ChannelRouter` supplies an explicit output-by-input gain matrix. Its default
maps matching channel indexes and leaves extra outputs silent. It does not invent
a surround layout or downmix coefficients. A host supplies those intentionally.

The preview limits channel count and effect preparation to a 128 MiB state
budget, including existing and proposed state. Long delays at high sample rates
and high channel counts can exceed that budget; rejected configurations leave
the current processing state intact. These are resource limits, not a fixed
speaker layout.

## Reuse from another C++ project

After installing the SDK, use:

```cmake
find_package(SoundCurrentEngine 0.1 REQUIRED CONFIG)
target_link_libraries(your-audio-target PRIVATE SoundCurrent::Engine)
```

Point `CMAKE_PREFIX_PATH` at the installation prefix. `SoundCurrent::Wave` also
exports the streaming WAVE reader/writer. The package has no Qt or audio-driver
dependency. Use a compatible compiler/toolchain for the static libraries.
Version 0.1 is a preview API; future minor versions may change the API.

The [consumer example](../examples/engine_consumer) constructs a six-channel bus
and applies independent gain without opening any device:

```bash
cmake -S examples/engine_consumer -B consumer-build \
  -DCMAKE_PREFIX_PATH="$PWD/engine-sdk" -DCMAKE_BUILD_TYPE=Release
cmake --build consumer-build --config Release
```

The library uses zero-based channel indexes. Renderer command-line indexes below
are one-based.

### Audio thread contract

Construct and configure while processing is stopped, or serialize configuration
with processing in the host. Configuration allocates memory and computes EQ
headroom, so it does not belong in an audio callback. `process`, `processPlanar`
and `reset` perform no allocation, locking or I/O.

The audio thread can apply scalar post-gain, channel-gain and mute updates
between blocks using the dedicated setters. These preserve filter/effect tails
and do not allocate. Hosts should deliver control changes to that thread through
their own queue; do not call setters from a concurrent UI thread. Gain changes
are immediate at the next block and are not ramped in this preview.

Planar buffers must have equal lengths and must not overlap. The router permits
same-buffer routing when input/output channel counts match, including channel
swaps. Other overlapping input/output buffers are rejected before mutation.

## Offline renderer

The renderer reads PCM16/24/32 or float32 RIFF/WAVE and writes float32 extensible
WAVE. It streams 1024-frame blocks instead of loading the entire recording.
RF64, compressed WAVE and files exceeding the standard RIFF 4 GiB limit are
unsupported. A known input speaker mask is preserved when routing is unchanged;
explicit routing or a channel-count change clears it rather than assigning
incorrect speaker positions.

Examples, using the Linux build path:

```bash
# Independent EQ and trim on channel 3 of an existing multichannel file.
build-engine/soundcurrent-studio-render --input input.wav --output adjusted.wav \
  --eq 3:1000:-4:1 --gain 3:-2 --post-gain 1

# Explicit six-channel to stereo matrix. Choose weights for the source layout.
# Unlisted input/output connections are silent once any --route is supplied.
build-engine/soundcurrent-studio-render --input six-channel.wav --output stereo.wav \
  --output-channels 2 --route 1:1:-3 --route 2:2:-3 \
  --route 1:3:-6 --route 2:3:-6 --route 1:5:-6 --route 2:6:-6

# For a file whose channel 4 is known to be LFE, add a 120 Hz low-pass.
build-engine/soundcurrent-studio-render --input surround.wav --output lfe-filtered.wav \
  --lowpass 4:120:0.707
```

Use `--help` for the full option list. Delay/reverb are enabled by their
`--delay-mix`/`--reverb-mix` options; `--tail` appends silence so effects can decay.
The renderer reports frames, channels, peak, clipped samples and invalid samples.

Output is written to a temporary directory next to the destination and published
only after success, without replacing an existing file. The destination
filesystem must support hard links, such as ext4 or NTFS. The Windows CI SDK
includes the renderer's Microsoft runtime DLLs alongside the executable; no
virtual audio driver is needed for offline rendering.

## Verification and remaining device work

Synthetic tests cover channel isolation from mono through 256 channels,
independent EQ/headroom, delay timing, reverb decay, effect-state preservation,
block-size equivalence, planar/interleaved equivalence, routing, clipping and
allocation-free processing. File tests exercise sample formats, channel masks,
malformed input, bounded parsing and protection of existing output files. An
independent project builds against the installed CMake package in Linux and
Windows CI. Local AddressSanitizer and UndefinedBehaviorSanitizer checks also
cover the engine and renderer.

These checks do not require expensive multichannel hardware. Native platform adapters and Studio controls are implemented. Actual speaker
mapping, driver-specific formats, latency and device changes still need physical
hardware verification before a production surround release. Linux live streams
are limited to PipeWire’s 64-channel format; offline processing supports 256.

GPL-3.0-only and the inherited copyright/third-party notices remain in force.
