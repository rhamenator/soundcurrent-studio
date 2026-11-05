# SoundCurrent Studio 1.1.3

- Adds 59 correction profiles shared by EQ and Studio: 42 microphone curves (including lavaliers and USB) and 17 amplifier/load-state curves. Model variants and measurement conditions remain explicit; models without correction curves are excluded.
- Selecting a usable equipment profile applies it immediately. None clears the selected equipment route. Fresh installations start with no equipment correction and microphone EQ off; saved choices are retained on upgrades.
- The equipment manager scrolls within the display and keeps Close accessible. Accidental wheel movements over the closed profile selector do not change audio. Importing, saving, opening or filtering the library does not apply a correction.
- Includes source attribution, measurement uncertainty, source hashes, reproducible correction admission, and an experimental AR-2ax acoustic-power custom import in the source examples. Typical/relative profiles are not individual-unit certification.
- Keeps Quit, system volume, output gain, balance and readable peak metering accessible. Input gain is a slider; compression/expansion share a neutral-centered control and limiting is independent.
- Organizes effects, mixer, channel editing and rendering into separate pages. Compatible layout changes apply live by default; explicit offline editing and failed-route recovery remain available.
- Channel monitoring is application-modal and fits the display. Production mono/stereo uses the persistent controls; `--test-controls` enables stereo/mono dialog testing with a synchronized stereo balance slider and Center reset. Multichannel testing remains limited by the owner's stereo hardware.

Install over the existing application; Quit the active equalizer first. Closing its window may leave processing running. Presets and settings are retained. Only one equalizer should process audio at a time.

Linux installers cover compatible Ubuntu/Debian systems, Fedora 44+, and RHEL 10-compatible systems. Windows retains the existing interim VB-CABLE route, setup/reboot and recovery guidance; the application/installer remains unsigned. Passing automated UI and installer checks does not establish physical microphone accuracy, production acceptance, or arbitrary-channel hardware qualification.
