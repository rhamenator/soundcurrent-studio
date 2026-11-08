# Windows issue 22: local diagnosis and verification

Tested on the affected Windows PC, with signed VB-CABLE 3.3.1.7 and Realtek analog output. Secure Boot remained enabled; the native test driver was not enabled or changed. No microphone was required. These are digital measurements at the physical render endpoint, not acoustic amplifier/speaker measurements.

## Diagnosis and repairs

- The saved `studioSession` contained `offline:true` with a valid stereo layout. Startup restored transient staging as permanent editing intent. Startup now uses a separate `studioOfflineEditing` preference, defaulting legacy sessions to live. Explicit mode-button choices and deliberate setup-file review preserve offline intent; automatic rejection does not. Apply clears intentional offline mode and delivers the staged session.
- VB-CABLE had the saved REG_DWORD `VBAudioCableWDM_VolCtl=1`, but the installed guard's `--check-volume` returned 12. The existing explicit elevated configure helper repaired the live setting; subsequent checks returned 0. Endpoint-only readiness previously missed this mismatch. `--check-ready` now requires both endpoints and live/saved volume readiness.
- Realtek Audio enhancements was visibly set to Device Default Effects in Windows Settings. Initial physical-loopback gain measured +6.69 dB for +6 dB, and independent channel mute failed. Switching the visible enhancement setting to Off restored linear gain and channel isolation, matching the user's preference. An initial diagnostic property-store write did not change that Windows UI setting; the extra property was cleared back to its original empty state. No hardware-specific processing compensation was added.
- Replaced the small offline checkbox with a prominent checkable mode button above channel routing: teal for live, amber for offline, with text status and adjacent Apply while staged. Existing translated strings and accessible offline checked state are retained.

## Passing checks

- Direct DSP and 1–256-channel engine test executables passed.
- Fixed-purpose cable API fixture passed saved/live mismatch, inaccessible control, reboot marker, cancellation, idempotence and persistence/apply cases.
- Shared Qt UI self-test passed, including new legacy-staging migration, intentional preference restoration, Apply callback, live mute delivery, and rejection/restart coverage.
- Real VB-CABLE -> Studio -> Realtek loopback: +6.00 dB output gain, -12.00 dB EQ, -6.00 dB input gain, -9.00 dB independent trim; mute, effects, bypass, balance, stream restart and rejected-layout preservation passed.
- Windows default-volume API measured -10.10 dB at 50% (endpoint reported -10.51 dB). Real media-key injection muted both channels to digital zero; unmute and volume-down passed while Studio processed audio. All three playback roles targeted the cable.
- Actual Studio controls: staged gain preserved playback; Apply measured -5.98874 dB; channel mute RMS 0 / 0.00175966; trim -8.98767 dB; solo RMS 0.00176379 / 0; effect preset changed level -2.80074 dB. Enable/disable/re-enable/Quit restored all roles. The old test's Quit text lookup was repaired to use its stable object name.
- Installed patched Studio was opened on the real user settings: it recovered to live, showed the teal button, switched amber/offline, and Apply returned it to live. The saved preference and serialized session are now both false for offline.
- Audio-state journal comparison passed after each completed test, restoring endpoint levels/mutes and roles. Both installed readiness helpers returned 0.

## Build and limits

Based on Studio main 9ba557062b6f06f8c6d726c08a235addaf7d4f9f. Local MSVC 19.51 / VS2026 Release x64 build, one worker at BelowNormal priority. Installed Qt 6.12.0 runtime retained. Local SDK is Qt 6.5.3 msvc2019_64; a build-directory-only forced header replaces its removed MSVC stdext iterator wrappers with raw iterators for QT_CORE_LIB targets. Normal compiler exception/RTTI flags and /Zc:__cplusplus were retained. Python 3.12.10 embeddable (PSF-signed) supplied the required catalog generator. Production CI should use its current Qt SDK; no SDK headers or source toolchain defaults were modified.

Reproduce using the normal Windows CMake build, then shared-ui, dsp-response, studio-engine and windows-cable-volume-configuration tests. With other playback paused and both apps quit, explicitly opt into `soundcurrent-windows-audio-smoke --run` and `soundcurrent-studio --windows-live-ui-test` (QT_FORCE_STDERR_LOGGING=1). Tests emit quiet tones, temporarily acquire output routing and restore it. They need active signed-in physical/cable endpoints. Keep output enhancements off for quantitative DSP/channel-isolation measurements.

Installed local patched executable SHA256: 678C6F70E5F8E16CB089E3D5A2C5B91BF5A3DDF2FDC5FED7143FD859FF2A65F1. Original published installation, installer, settings and rollback files were preserved locally. This is a local patched 1.3.1 build, not a new published release. Windows boot persistence and Linux runtime execution have not been retested in this session; application/session restart logic and Windows stream restarts were tested. Do not claim a Windows reboot or Linux runtime pass from these results.
