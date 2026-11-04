# Metering, dynamics, volume and room balance

## Hardware verification limitation

The current development workstation has stereo output only. No physical
multichannel playback has been verified on it. Generated signals, silent UI
fixtures and offline WAV round trips exercise 1, 2, 6, 8, 16, 32, 64, 128 and
256 channels. These checks establish software channel separation, routing,
serialization and rendering behavior, not correct physical speaker assignment,
interface-driver operation, synchronization, latency or listening quality for a
real surround/theater system. Those require a suitable multichannel interface
and speakers before claiming hardware qualification. This limitation applies to
Linux and Windows; a Windows VM is not a substitute for multichannel hardware.

## Reading the meters

The persistent overall readout is an estimated sample peak, not a certified
true-peak or loudness meter. Its fixed-font text is right aligned. The number
updates at most five times per second, collecting the largest incoming peak in
each 200 ms window, with 150 ms upward and 600 ms downward smoothing. Short
clipping-risk events remain flagged for one second. Graphical meter delivery
keeps the selected refresh interval; smoothing the number does not slow audio
processing or hide transient warnings from the graphical meter.

The Studio tab has separate labeled meters for stereo, a speaker-position view
for recognized 5.1/7.1 roles, and labeled meter banks (up to 64 channels per bank) for arbitrary channels.
Each channel has a fast level bar and a one-second held peak value. Green,
amber and red indicate increasing sample levels; the color boundaries are
approximately −6 and −1 dBFS. These are signal values, not calibrated acoustic
sound-pressure levels. LFE is shown separately from the room's directional
speakers. Unrecognized/custom role metadata falls back to labeled banks rather
than guessing a speaker location from the number of channels.

The existing detailed channel table remains available below the spatial view.
The silent generated-signal test never plays test tones through the speakers.

## Dynamics and limiting

The former Dynamic Boost parameter retains its stable numeric ID. Its range is
now −100% to +100%, with zero in the middle as the default neutral position.
Negative values apply downward expansion below the configured threshold;
positive values retain the existing compression plus makeup behavior. This
control chooses one behavior at a time. A receiver-style loudness EQ is a
separate process, not dynamics compression.

Limiter enable is independent of this control. Its ceiling and release are in
Advanced enhancement controls. In the Studio engine it is linked across all
channels after channel/output gains and before the final numerical safety
clamp. Limiter bypass does not disable that final hard clamp at full scale.
This limiter has instantaneous peak attenuation and release smoothing; it is
not a lookahead or oversampled true-peak limiter. New dry profiles default to
limiter off. Old 16-value enhancement profiles with positive Dynamic Boost
migrate to limiter on, preserving their previous protection; old dry profiles
stay dry. New state appends the limiter flag as parameter 17.

## Volume

The persistent System volume slider controls the current default OS playback
endpoint, including Studio's virtual sink while it owns the route. Media keys
and this slider therefore adjust the same endpoint volume. This slider does
not apply another DSP gain. Startup reads the current level without setting or
unmuting it. Unavailable devices disable the control; endpoint changes are
re-read. Linux uses asynchronous pactl calls; Windows uses Core Audio endpoint
volume, all outside audio callbacks. The UI refreshes system volume every
200 ms. Existing saved post gain is retained under Settings as processing trim,
with its previous gain range and undo behavior. It remains distinct from the
main listening-volume control and can still cause clipping if raised.

## Room balance and future source panning

Stereo retains a left/right balance slider. Recognized 5.1/7.1 layouts use a
mouse-draggable room balance pad in the Studio tab, with keyboard arrows
(Shift for fine adjustment), Home/double-click and Center reset. Center applies
unity gain. Moving right attenuates left speakers; moving rearward attenuates
front speakers. Axes combine multiplicatively. The control never boosts favored
speakers, never moves audio between channels and never changes LFE gain.
The existing per-channel trims handle individual calibration and arbitrary
layouts. Lock, Studio undo and saved setup state cover room balance.

A sound-source panner is a separate future feature: it would move source
material among outputs, require explicit speaker geometry and a defined pan
law, and optionally accept physical joystick/controller input. The current
room pad is not a source panner and does not implement physical joystick input.

Design references:

- [Cubase Pro 15 MultiPanner panel](https://www.steinberg.help/r/cubase-pro/15.0/en/cubase_nuendo/topics/surround_sound/surround_sound_plug_in_panel_r.html)
- [Cubase Pro 15 joystick panning](https://www.steinberg.help/r/cubase-pro/15.0/en/cubase_nuendo/topics/surround_sound/surround_sound_use_joystick_for_panning_t.html)
- [Audient speaker-position metering](https://docs.audient.com/horizon32/metering/)
- [Microsoft endpoint-volume scalar API](https://learn.microsoft.com/en-us/windows/win32/api/endpointvolume/nf-endpointvolume-iaudioendpointvolume-getmastervolumelevelscalar)
