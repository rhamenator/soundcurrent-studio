# Native Windows transport and Studio integration — November 7, 2026

## Follow-up: installed for normal use

Automatic startup was subsequently enabled at the user's request using the
standard per-user `Run` entry `SoundCurrent`, pointing to the installed Studio
with `--background`. The saved command was read back and exercised after a
normal quit. Studio selected native playback for all three output roles;
endpoint volume, mute and unmute round-trips passed, restoring 0.71131474 and
unmuted. The native PnP device remains healthy, problem code zero, `oem54.inf`;
its normal PnP-managed service start configuration was retained. No boot-policy
change or Windows restart was performed. This validates startup-style launch,
not the actual post-reboot sign-in sequence, which remains untested.

The startup change journal, verification log and `restore-startup.ps1` are in
the installation backup directory below. Studio rollback now also restores the
saved startup configuration. Both rollback scripts were syntax-checked, not
executed against the working configuration. Studio is left running in the
background as it will at sign-in.

At the user's request, the exact tested Studio executable and route guardian
were installed into the existing per-user installation. SHA-256 values match
the previously recorded tested binaries. Matching Qt 6.5.3/MinGW libraries and
plugins were deployed together; loaded-module inspection confirms the running
app loads Qt from its installation directory. The cable setup helper was built
with the matching toolchain. A packaged UI self-test and all six integration
invocations passed again, with GUI tests executing the installed Studio binary.

The normal app was opened with existing presets, EQ/gain and microphone-off
settings preserved. All three playback roles selected native playback
`{0.0.0.00000000}.{bfef26d6-5bd4-46bf-8d2a-de78c4e6ef43}`, with Realtek as the
physical destination. A normal quit restored Realtek defaults; reopening
automatically selected native playback again. The app is left running.
Windows volume is approximately 71.13%, unmuted. Studio's slider changed the
native endpoint level; setting Windows endpoint mute was reflected as “Mute”
in the live UI. Original level and unmuted state were restored. Hardware media
key injection was unavailable in the UI tool; those keys were not simulated.

Original installation and user data/registry settings are backed up under
`C:\Users\wyurd\Downloads\SoundCurrent-Studio-native-install-20261107`.
Its `rollback-studio.ps1` is syntax-checked and ready to run elevated if needed:
it closes Studio, validates/disables only the native `ROOT\MEDIA\0002 / oem54.inf`
device, retains the candidate files and restores the original Studio. Original
settings backups are retained separately. VB-CABLE remains installed for this
fallback; startup preferences and boot/security policies were not changed.
Rollback was not executed against the working installation.

The dated build/test sections below describe the earlier validation stage;
their statement that the installed release had not yet been replaced was true
at that stage and is superseded by this follow-up.

Source revision: `30d064a0c9f7d75297cf8c9819263b8353a1d98f`.
Destination: Windows 11 Pro x64 build 26300, Dell OptiPlex 7040.
The changes remain hardware-independent. Global WDK/SDK 10.0.28000.2526 builds
the Release x64 driver; Qt 6.5.3/MinGW 13.1 builds Studio and integration tools.
Builds used one worker and BelowNormal process priority. Explorer responded
during sampled checks; no automatic reboot or boot/security-policy change occurred.

## Defects found and fixed

1. The earlier data-range attribute fix permits shared streams on all three
   endpoints. Its historical report and original failing bundle are preserved.
2. Studio classified every native endpoint as playback because Core Audio's
   interface friendly name describes the common adapter. Discovery now follows
   `IDeviceTopology` connector zero to the connected filter and matches exact
   driver-owned topology reference strings, with direction checks. Unknown
   native roles remain excluded from physical fallback outputs. The initial
   attempts to set distinct driver interface properties did not fix Core Audio
   classification and were reverted.
3. Native playback endpoint mute did not silence the loopback bridge. The bridge
   now reads source mute, silences processed/queued output while muted and
   restores normal processing on unmute. Volume attenuation is not applied twice.
4. Native capture advertised SYSVAD's simulated hardware mixer nodes: endpoint
   volume changed its reported dB without changing captured PCM. Its topology
   now connects the bridge directly, leaving shared-mode volume/mute to Windows
   software controls. The stock sample's unrelated topology definitions remain.

API context: Microsoft's [Core Audio adapter-name property](https://learn.microsoft.com/en-us/windows/win32/coreaudio/pkey-deviceinterface-friendlyname),
[connected topology identifier](https://learn.microsoft.com/en-us/windows/win32/api/devicetopology/nf-devicetopology-iconnector-getdeviceidconnectedto),
and [software endpoint controls](https://learn.microsoft.com/en-us/windows/win32/api/endpointvolume/nn-endpointvolume-iaudioendpointvolume).

## Final runtime results

All six integration invocations passed on the final candidate:

| Test | Observed result |
| --- | --- |
| Endpoint identity regression | Playback, microphone feed and capture distinguished; negative and wrong-direction cases rejected |
| Complete signal-bearing chain | Native playback → WASAPI loopback → Studio processing → native microphone feed → native capture |
| EQ/Studio processing | +6.00 dB gain, −12.00 dB EQ, 0.00 dB bypass; balance, independent channel EQ/mute, effects, restart and rejected-layout preservation pass |
| Playback volume/mute | Measured −10.51 dB matches endpoint report; mute/unmute pass; muted residual below two PCM16 LSBs |
| Sustained signal | 30-second run: 1,400,160 measured frames after initial settling; stereo RMS about 0.014137; bridge remained running |
| Native microphone controls | Signal RMS 0.014142 per channel; producer silence bounded by PCM16 quantization; feed mute and capture mute produce zero RMS; capture volume measured −31.08 dB matches endpoint report |
| Studio live GUI | Explicit enable/disable/re-enable/Quit completion marker; managed ID matches native playback; all output roles restored |
| External Windows output selection | Studio processing disables and all chosen output roles remain preserved |
| Native route recovery | Volume copying/restoration, forced owner termination, and guardian failure while owner remains alive restore volume/mute and all playback roles |

The 96 kHz calibration PCM playback timing check also passed (1.048 seconds for
one second of input) into the virtual destination; this is not acoustic calibration.
Silence thresholds account for conversion to the driver's PCM16 kernel transport
and are not relaxed signal-level/gain tolerances.

The initial attempt to measure effects using Realtek loopback was contaminated
by other normal playback. It is retained as an invalid measurement, not evidence
of a gain defect. Final quantitative tests use the isolated virtual chain and
do not depend on room acoustics or speaker output.

## Machine state, preserved packages and rollback

Final native instance: `ROOT\MEDIA\0002`, hardware ID `Root\SOUNDCURRENTVAD`,
service `SoundCurrentVAD`, published INF `oem54.inf`. PnP reports OK and the
service is running. Loaded-image hash matches the locally signed SYS. HVCI is
still running (`SecurityServicesRunning` contains 2). Secure Boot remains off
and test-signing remains enabled from the previously authorized setup.

The runner journals all six default roles and affected endpoint levels/mute,
then restores them in a finally block. Final before/after journals match exactly.
Realtek remains playback default at its saved level (approximately 71.13%,
−5.09375 dB), unmuted. The original capture default remains VB-CABLE. Test apps
and guardians exited. Normal installed Studio was not replaced; the tested
Studio build is the PR candidate, using temporary settings for GUI tests.

The final separate local directory is
`C:\Users\wyurd\Downloads\SoundCurrent-native-driver-test-30d064a`.
It holds unsigned/signed drivers, source snapshot, public certificate,
build/validator/signature/runtime logs, checksums and exact-device rollback.
Run its `rollback-native-test.ps1` elevated only when rollback is desired; it
validates the current native hardware ID and `oem54.inf` before removing that
device/package. Optional trust removal is limited to the journaled local test
certificate. Its syntax was checked; destructive rollback was not executed on
the passing candidate. Boot-policy restoration remains a separate action.
Historical candidate folders and logs were retained.

Original failing ZIP SHA-256 is still
`07c7569c50e7f6587418a8d308297a4d1f59e17dd586d81f22ca73a86a1cdb63`.
The original bundle and extracted inputs were not overwritten. No private key
is included in the test package or repository evidence.

## Limits of this run

No physical microphone is connected. These tests inject generated PCM and
measure captured digital PCM; neither a microphone nor a measurement microphone
is needed. A physical microphone's input driver/device selection/conversion path
was not retested. Prior physical-path tests may increase confidence but do not
constitute a new result from this run. Speaker/amplifier/room calibration,
physical hotplug, reboot/resume and long-duration stress were not tested.
Keep PR #13 draft for independent review and remaining release validation.

Raw selected evidence is under
`tests/results/native-driver/2026-11-07-integration-30d064a/`.
