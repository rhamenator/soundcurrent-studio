# Using Studio's controls

## Always-visible controls

Quit app is beside the top-level tabs. It exits the application and unloads its
audio processing. Closing the main window can leave processing in the background.

The bottom control strip combines the former Playback box with the listening
controls. It remains visible while any page scrolls:

- Equalizer on/off enables or disables the audio route.
- System volume controls the OS default playback endpoint. Media volume keys
  and this slider adjust the same level.
- Output gain is the former Post gain: DSP gain after EQ and effects, before
  the optional limiter. Its range is −60 to +12 dB. It does not replace the OS
  volume percentage or the amplifier's physical knob.
- Balance is shown for stereo only. Mono has no balance control.
- Overall output is an estimated peak meter with a readable numerical display.
- Channel meters opens the larger channel-monitoring and room-balance dialog.

Meter refresh and peak-marker preferences are in Settings & calibration.

## Studio channels & effects

The first controls select a channel layout. The pages below separate the jobs:

### Effects

Input gain changes the signal **before** EQ, dynamics, delay and reverb. Its
range is −60 to +24 dB. Increasing it drives the compressor harder; Output gain
changes the level after those effects. Both default to 0 dB for a fresh setup.

Expansion / compression is the first sound-enhancement control. Center (0%) is
neutral, left applies downward expansion, and right applies compression plus
the configured makeup gain. The independent Limiter switch and its advanced
ceiling/release settings remain separate. This control is part of PR #7; it is
not present in the earlier installed 1.1.2 release.

Bass, Clarity, Ambience and stereo widening are additional listening effects.
Delay/echo and reverb have separate enable switches and wet-mix controls. Dry
means no enabled effect contribution; selecting an effect preset can replace
the existing effect settings.

### Mixer

Each logical output channel has gain, mute and solo controls. These edit the
same state as Selected channel, including undo and saved setups. They are a
channel mixer inside Studio, not a mixer for the separate apps running in the
OS. The OS still handles per-application stream volumes.

This mixer and Input gain use the shared C++ engine on Linux and Windows.
They do not depend on a signed custom driver. A future driver/backend can feed
the same engine and UI; these controls need not be rewritten. OS per-app mixer
integration would be a separate backend feature.

### Selected channel

This is the advanced channel editor: name, trim/mute/solo, channel EQ filters
and routing. Editing a filter's values prepares that edit; Add filter or Update
selected commits it. Set route commits the routing values. Those buttons make
these operations explicit.

### Offline WAVE rendering

Choose an audio file and explicitly start Render audio file. Rendering uses the
current shared EQ/gain and Studio configuration without requiring a matching
physical audio device. Existing output files are not overwritten. The rendered
file is separate from the current live playback route.

## Which changes are live?

With playback enabled and Offline editing unchecked, effects, input gain,
mixer controls and committed filter/routing changes apply immediately. The
status line near the top reports whether the settings were applied or whether
playback needs to be enabled.

Changing channel count/layout or opening a saved setup selects Offline editing.
That stages the configuration while existing playback keeps its last live
setup. Use Apply settings when ready. A compatible channel-layout change may
restart the route; if the device cannot support it, the app reports the problem
and keeps the configuration staged. Supported-capacity checks happen before
stopping the existing route; failed restarts attempt to restore the prior setup.
The route replacement and restoration logic is also tested with fake backends;
these tests do not replace physical device testing. Apply settings is hidden when settings are already live, and disabled while editing is locked.
Checking Offline editing also lets you intentionally prepare and render a
configuration without changing the current playback.

## Channel monitoring

Channel meters opens a modeless, scrollable dialog. Stereo consistently labels
generated channels Left and Right, including old generated Front left/right
names; custom names remain unchanged. Stereo shows L/R bars;
recognized 5.1/7.1 roles use a speaker-position display and room-balance pad.
Mono shows its single meter with no balance pad. Arbitrary layouts use labeled
banks of up to 64 meters, through 256 channels, and per-channel mixer trims.
Meters show current live playback even while settings are staged, provided the
live and editing channel counts match. A different live/staged channel count is
reported explicitly, without assigning live peaks to unrelated draft channels.
The dialog labels live playback, disabled processing, and silent-test readings.
These readings do not apply staged effects: use Apply settings to hear those.
The room pad changes balance, not source panning; LFE is unaffected. Lock also
protects the room pad. The silent generated-signal test never plays test tones.

**Hardware limitation:** the current development machine has stereo output
only. Synthetic/offline/UI checks through 256 channels do not qualify physical
surround drivers, speaker assignment, synchronization, latency or listening
quality. Real multichannel hardware checks remain pending. See
[metering and balance](metering-dynamics-and-balance.md).

## Start with the OS

Settings & calibration → Startup → Start when I sign in is off on a fresh
installation. It only enables startup when the user chooses it. Existing
explicit choices survive updates. This is a per-user desktop sign-in setting;
background mode uses the tray where available. Only one SoundCurrent equalizer
is registered to start at sign-in, to avoid competing processing routes.
