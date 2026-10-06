# Windows preview verification

## SoundCurrent Studio 0.8.0

The private Studio desktop and native WASAPI adapter were checked on 2026-10-05
in a signed-in, full independent Windows 11 26H2 clone. Its standard signed
VB-CABLE input and virtual High Definition Audio output provide stereo audio;
no original Copperfin VM was changed.

The installed application passed shortcut creation, shared Qt controls,
window fit above the taskbar, background close/reopen, single-instance activation
and graceful Quit. The
Studio controls also passed 256-channel editing, undo/lock, setup validation,
and an actual 256-channel WAVE render including a route into channel 256.
A visible desktop screenshot confirmed the Studio tab is usable in the VM.

Native live measurements of the Studio processing route gave +6.00 dB for a
+6 dB post-gain change and −12.00 dB for a −12 dB filter on one channel, while
the other channel retained its level. Independent mute, delay/reverb output,
bypass and rejection of a live layout change with playback preserved passed.
The inherited route tests also verified balance, stop/restart, restoration of
all three default playback roles and a 1.022-second stereo calibration signal.
The final installed package came from commit `03b1c14`, release workflow
`37271794116`; the independent SDK consumer checks came from workflow
`37271793716`. These results verify the software in a stereo VM, not a physical multichannel
interface, real room response, microphone cable or hotplug operation.

Ubuntu passed all four CTest suites and ASan/UBSan checks. A separate eight-channel
PipeWire null-output test measured −12.00 dB EQ and +6.00 dB post gain, channel
isolation and delay/reverb output without using the workstation's speakers.
Native Windows CI and Ubuntu/Fedora 44/RHEL 10 compatible package builds pass.
The core, silent meter test and offline renderer support 256 logical channels;
live Linux formats are capped at 64 and Windows requires matching endpoint formats.

## Inherited SoundCurrent EQ 0.7.0 baseline

Version 0.7.0 builds the shared Qt interface on Windows. Speaker and
amplifier profile selection, adjustable frequency/Q controls, level meters,
microphone EQ and the calibration preview use the same UI as Linux. Windows
uses its own WASAPI backend and requires a separately installed second cable
for simultaneous microphone processing. A build or UI test does not establish
that room calibration or hotplug works with every physical device.

Version 0.7.0 was checked in an independent Windows 11 26H2 clone with the
signed VB-CABLE driver and a virtual High Definition Audio output. The native
Microsoft compiler build measured +6.05 dB live post gain, -11.95 dB EQ cut,
0.05 dB bypass difference, full-left balance, successful stop/restart, restoration
of all three playback roles, and a 1.007-second stereo calibration signal.
The installed UI check passed for tabs, controls, shortcuts, background
close/reopen, a single running instance and graceful quit. Real output hotplug,
a second microphone cable and room measurements still need physical-hardware
verification.

The bundled-driver installer has also been checked in that clone with the
driver present and absent: default selection, opting out, cancelling UAC,
approving the vendor install, retry shortcuts, and finishing with restart
deferred. No original Copperfin VM was modified for these installer checks.

## Automated checks

Build on Windows with `scripts/build-windows.ps1 -QtPrefix <Qt SDK path>`.
The script tests the shared DSP and the Qt UI, with bounded process waits.
The packaged application includes Qt and the Microsoft app-local runtime.
CTest provides `dsp-response`, `studio-engine`, `studio-wave` and `shared-ui` checks.

The opt-in `build-windows-native/Release/soundcurrent-windows-audio-smoke.exe` integration
check lists endpoints by default. Run it with `--run` only in an isolated,
signed-in Windows test system with VB-CABLE installed and a physical output
available. Quit other copies of SoundCurrent EQ and Studio first. It plays a quiet
1 kHz tone at approximately -34 dBFS into CABLE Input and measures the
physical output through WASAPI loopback. It temporarily changes all three
Windows default playback roles to the cable and verifies their restoration
before testing the bridge. The Studio stage also checks native independent channel filters,
mute, effects, bypass and preservation of playback after a rejected layout. It checks Flat playback, a live +6 dB post-gain
change, a -12 dB EQ cut, bypass, full-left balance, stopping/restarting the
bridge, and the duration of a one-second stereo calibration signal. An
unsigned-in Windows VM returned silence from the cable; signing into its
desktop restored normal audio.

Run `tests/windows_ui_smoke.ps1` from the signed-in desktop of an isolated
Windows test system. Set the Windows default output to the physical speakers
first and quit the app. An optional `-InstallerPath` argument installs the
preview silently as the current user before the check. An optional
`-ResultPath` chooses where to save the result. The test verifies desktop
and Start menu shortcuts, runs the shared Qt control checks with isolated INI
settings, then verifies that the decorated window fits the desktop working area,
closing to the notification area, restoring the
existing instance and gracefully quitting through the app's activation channel.
The shared UI check also verifies that the Equalizer tab opens first with the
bands at the top, that device and sweep controls belong to Settings & calibration,
and that wheel movement scrolls the active page without changing a control.
It backs up the Windows registry settings before the desktop check and
restores them after successful exit. Use only disposable test settings; on failure it leaves
the app running so its state can be inspected.

`tests/windows_installer_smoke.ps1 -InstallerPath <setup.exe>` checks the
interactive installer in a signed-in independent clone. With the default
`-CableState Present`, it verifies detection and skipping of an existing
standard cable. After explicitly removing the driver and rebooting that
clone, use `-CableState Missing` to verify the default offer and opting out.
It checks visible vendor notices and the retry shortcut. By default, it opts
out without installing or removing any driver. On the independent clone,
`-CableState Missing -InstallDriver` exercises the real install step: manually
approve UAC, click Install Driver, and acknowledge the restart notices. Also
check cancelling elevation and restarting after a successful install.

## Check on real Windows hardware

1. Install the SoundCurrent EQ preview. When VB-CABLE is missing, keep the
   driver option checked, approve Windows' administrator prompt, and click
   Install Driver in VB-Audio's setup. Restart Windows when requested. On an
   existing installation, check that setup detects and skips the driver.
2. Start with your physical speakers as Windows' default output. Select your speakers or
   headphones in SoundCurrent EQ and start with Flat, 0 dB post gain, and
   centered balance. The app selects the cable while running. Play ordinary
   music at a comfortable volume.
3. Confirm that Flat sounds normal, that each named preset changes the
   sound, and that dragging post gain changes the level during playback.
   Check both ends of balance and restore it to center.
4. Toggle Equalizer on/off. Close the window, confirm playback continues,
   and reopen it from its icon or desktop shortcut. Lock the controls and
   confirm accidental wheel movement does not change a band.
5. Try a second physical output if available, including connecting and
   disconnecting it. Confirm the app's device list and Automatic behavior.
6. Use Quit. Confirm the prior Windows default output is restored and the
   app process exits. Also change Windows' default yourself before quitting
   and confirm that your subsequent choice is preserved.
7. With a separate cable installed, select a mono/stereo microphone and the
   microphone cable, then enable mic EQ. Check both routes together. Without
   a second cable the app should report that one is required.
8. Put the microphone at the listening position, stop music and use a quiet
   sweep. Reject measurements that are clipped or too close to background
   noise. Review the proposed changes before applying them.

The app installer is currently unsigned. The audio driver is separately
signed by its vendor. The installer and checksum should be obtained from
the project's GitHub release page.

## Unreleased integrated SoundCurrent driver checks (2026-10-06)

Release installer builds now require `-SignedDriverPackage` and
`-SignedDriverManager` in addition to `-QtPrefix`. See `windows-signing.md`.
The privileged manager has a static CRT and System32-only dependency lookup.

`tests/windows_integrated_installer_smoke.ps1 -Run` is an opt-in test for an
independent clone. It expects both explicitly named unsigned prototype fixtures
in the selected `-PrototypeDirectory`. It checks application installation,
in-place updating and user-file preservation, installed UI, unsigned-driver
refusal, unowned-driver uninstall and unchanged physical/third-party audio
identities. These checks passed on `soundcurrent-win11-dev`. They do not prove
successful installation or removal of a signed SoundCurrent kernel driver.

The older `windows_installer_smoke.ps1` covers the previously shipped VB-CABLE
installer. The detailed current evidence and remaining gates are in
`native/windows/virtual-driver/verification-2026-10-06.json` and
`IMPLEMENTATION-STATUS.md`.

### Endpoint volume handoff

On an independent Windows clone, build `soundcurrent-windows-volume-smoke`
and place it beside the route guardian and Qt runtime. Run it with `--run`.
It temporarily changes guest default routes and endpoint levels, verifies
71%/unity/changed-volume-and-mute handoff using the production bridge, then
restores fixture levels. It needs existing virtual capture/render and physical
endpoints, emits no test signal, and stays outside CTest. This is userspace
control-state verification; signed SoundCurrent kernel audio remains a
separate required acceptance check.

### Live Windows output-choice checks

On an independent clone, run `tests/windows_live_ui_routes.ps1 -Run` against
the deployed builds. Add `-TwoPhysicalOutputs` when two physical render
endpoints are present. The extra case pins one output in the app, then changes
all three actual Windows default roles to the other physical output. It requires
processing to show Off and the user's Windows defaults to remain selected.
The runner rejects missing/nonzero native exit codes and requires the explicit
manual-choice completion marker. These are offscreen Qt window tests using
endpoint APIs, not Windows Settings interaction or own-kernel verification.

### Live process-conflict discovery

On an independent clone, build/deploy `soundcurrent-processing-guard-test`
and run `tests/windows_live_process_conflict.ps1 -Run`. Harmless copies of
Windows cmd hold open under FxSound.exe and Peace.exe names; the exact
production Toolhelp discovery must return the matching conflict. An unrelated
process and the state after fixture cleanup must remain allowed. Retained native
process handles enforce exit codes. No third-party equalizer is installed or
executed; this verifies Windows process discovery rather than third-party audio
behavior or the complete live-window conflict shutdown workflow.

### Conflict introduced after desktop startup

`tests/windows_live_window_conflict.ps1 -Run` starts each ordinary desktop
with isolated settings and an offscreen Qt window. The opt-in
`--windows-live-conflict-test` hooks report readiness only after processing
acquires all three output roles and dismiss the warning dialog unattended.
The production startup guard and two-second conflict monitor remain in use.
A harmless FxSound.exe or Peace.exe process starts after readiness. Before
the dialog is dismissed, hooks require the power control to show Off and the
managed route to be released. The coordinator requires the matching diagnostic,
native exit zero, original output-role restoration and guardian exit.
No third-party audio software or unsigned kernel driver is installed.

### Physical volume after direct Windows selection

On an independent clone with the documented USB/onboard fixtures, run
`tests/windows_manual_output_volume.ps1 -Run`. It saves endpoint scalar/mute
states, starts the actual desktop route test, observes owned roles followed by
the direct physical choice, and adjusts that output to 58% unmuted. The chosen
state must survive processor shutdown; the previous output lease must restore
its untouched state. Both same-output and different-output cases are exercised.
Fixture endpoint levels are restored in `finally`; no test signal is generated.

The opt-in volume helper supports `--read-volume ID` and
`--set-volume ID SCALAR MUTE` for this coordinator. The setter intentionally
persists a guest-only fixture change; callers must save and restore its state.
This proves endpoint control-state behavior, not acoustic gain or own-kernel
frequency response.
