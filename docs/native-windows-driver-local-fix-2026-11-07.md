# Local Windows driver fix and retest — November 7, 2026

The format-negotiation failure is fixed on the destination Windows PC by source
revision `69f116b661777cf887615fc05449fefbcd452997`. The shared data range now
advertises `KSDATARANGE_ATTRIBUTES` and includes `PinDataRangeAttributeList`,
matching the processing modes/default formats already supplied by the driver.
This affects render, microphone feed and capture; it adds no hardware dependency.
The original failure report remains historical evidence.

## Build and installation

- Installed Microsoft's global WDK and VS driver-kit component successfully,
  without forcing a restart. Used pinned WDK/SDK NuGet 10.0.28000.2526,
  VS 2026 MSBuild, Release x64, single build worker and BelowNormal priority.
- INF verification, Universal API validation, Inf2Cat and local SYS/CAT signing
  and catalog membership checks passed. Memory Integrity was left enabled.
- Updated the existing `ROOT\MEDIA\0002` / `Root\SOUNDCURRENTVAD` device rather
  than creating another root device. Published INF is now `oem50.inf`.
  PnP status is OK, service SoundCurrentVAD is running, and the loaded image's
  hash matches the signed candidate SYS.
- Captured and restored all playback/capture default roles. Playback remains
  Realtek. No physical output volume was changed by these probes.

## Runtime results

The explicit-endpoint probe in `tools/windows_native_format_probe.cpp` passes
on BOTH render endpoints and the capture endpoint. GetMixFormat now returns
48 kHz stereo, 32-bit extensible engine format; the engine also accepts the
declared extensible PCM 48 kHz stereo 16-bit format. Endpoint-volume activation,
shared initialization, buffer acquisition (render), Start and Stop all return
S_OK. Five additional opens per endpoint produced **15 runs, zero failures**.
The original package failed GetMixFormat/volume/initialization with 0x88890008.

The standalone `tests/windows_mic_transport.cpp` test also passed buffering,
overflow, silence, restart and 200,000-frame concurrent producer/consumer checks.
This is a unit test, not proof that WASAPI microphone feed reaches capture.

These tests establish the format fix and safe silent stream start/stop. They do
not establish signal-bearing render loopback, native microphone feed-to-capture,
effects, system-volume behavior, crash recovery, long-duration stability or
audible end-to-end Studio operation. Keep this PR draft pending those tests and
independent review. Endpoint GUIDs changed on update; discover current endpoints
instead of reusing historical IDs. The initial attempts with obsolete IDs
returned device-not-found, before rerunning against current endpoints.

## Preserved artifacts and rollback

Original ZIP SHA-256 remains
`07c7569c50e7f6587418a8d308297a4d1f59e17dd586d81f22ca73a86a1cdb63`.
Neither the original ZIP nor its extracted driver files were overwritten.

The new local folder is
`C:\Users\wyurd\Downloads\SoundCurrent-native-driver-test-69f116b`.
It contains separate unsigned/signed drivers, exact source archive, manifests,
checksums, validator/build/install/probe logs, and `rollback-native-test.ps1`.
That elevated rollback script validates the exact device/hardware ID/OEM INF
before removing only this candidate device/package. Its syntax was checked;
rollback was not executed because the candidate tests pass. Certificate trust
removal is optional and restricted to the journaled local test certificate.
Test mode/Secure Boot restoration remains a separate explicit step. Never share
the nonexportable private key or treat this local signature as production trust.

Raw selected evidence is in
`tests/results/native-driver/2026-11-07-local-fix-69f116b/`.
The native device remains enabled for further direct tests; Realtek remains the
normal playback path. The old failing package is retained as historical input.
