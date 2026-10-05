# Premium feature coverage

The agreed premium scope is arbitrary-channel processing with delay/echo and
reverb, retaining the free EQ's controls. Version 0.8.0 implements that scope
and adds the editing and file workflows needed to use it without surround gear.

| Feature | Implementation | Verification |
| --- | --- | --- |
| 1–256 channel core | Device-independent C++20 engine, interleaved/planar | Synthetic isolation and block equivalence tests |
| Channel EQ | Independent filters plus shared first-page EQ | Frequency-response and channel isolation tests |
| Routing | Explicit matrix, signed gains, no implicit surround synthesis | Router tests and 256-channel UI render |
| Trim, mute, solo | Selected-channel controls and effective processing snapshot | Engine gain/mute tests and UI state checks |
| Delay/echo | Time, feedback, wet/dry, independent channel state | Impulse timing/tails, live virtual-device output |
| Reverb | Decay, damping, wet/dry, independent channel state | Decay/isolation/block tests, live output |
| Effect presets | Seven selectable dry/effect profiles with editable controls | UI checks for preset application |
| Studio setup files | Validated JSON `.scstudio`, bounded size, atomic save | Round trip and invalid parameter rejection |
| Undo and lock | Separate Studio undo history; shared lock and wheel protection | UI tests including channel 256 |
| Channel meters | Colored peak readouts, silent synthetic source | UI controls; live engine peaks |
| Offline rendering | Shared EQ + Studio processing, effect tails, progress/cancel | UI 256-channel render; WAVE format/no-overwrite tests |
| Linux live adapter | Native PipeWire streams and serialized settings changes | Eight-channel virtual output, measured −12 dB EQ/+6 dB gain |
| Windows live adapter | WASAPI endpoint formats, full configured-channel processing | Native build; VM checks recorded in Windows testing notes |
| Reusable library | Exported CMake SDK without Qt/audio-driver dependencies | Separate Linux/Windows consumer build and run |
| Installation | Separate Studio identity, settings and desktop shortcuts | DEB build and Windows installer/UI checks |

## Hardware limits

The core and UI expose 256 logical channels. A selected output must actually
support the requested live channel count. PipeWire formats are capped at 64
channels, and Windows depends on the endpoint/cable mix formats. A stereo output
does not provide 256 physical channels. Large layouts can be edited, metered with
silent signals and rendered to files today.

Physical surround hardware, real device hotplug and representative professional
interfaces remain release verification work. More effects, a graph editor,
plugin hosting, recording/sequencing and a DAW are possible later extensions;
they are not required to use the implemented premium EQ/effects feature set.
