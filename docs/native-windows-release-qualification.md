# Native Windows release qualification

The native-driver release gate runs only on a full independent Windows clone. It does not run on a developer workstation, either Copperfin original, `copperfin-clean-win11`, or `soundcurrent-win11-dev`. The runner refuses physical hardware and those protected VM names.

`tests/windows_native_release_qualification.ps1` divides the audit into durable phases:

1. `Preflight` records OS, Secure Boot, HVCI, Driver Verifier, SoundCurrent devices, and the original Windows audio state.
2. `Install` validates the signed package through the production manager, registers both Studio and EQ owners, and can enable standard Driver Verifier rules for only `soundcurrentvad.sys`.
3. `Runtime` executes endpoint identity, format, transport, microphone, UI, routing, volume, guardian, and audio-state restoration checks.
4. `Stress` repeats playback and microphone transport for a bounded 1–240 minutes.
5. The host suspends and resumes the clone; `PostResume` reruns the integration suite.
6. `Uninstall` removes both owners and resets Driver Verifier.
7. `Final` requires the driver and Verifier selection to be absent and the original audio state to compare exactly.

The harness deliberately does **not** enable test-signing, import certificates, disable Secure Boot or HVCI, change execution policy, reboot, suspend the host, or handle a private signing key. Those boundaries prevent a test helper from becoming a security-policy bypass. Reboots and VM suspend/resume are controlled externally so a failed guest cannot conceal the transition.

Before any mutating phase, create a short-lived marker inside the disposable clone at `C:\ProgramData\SoundCurrent\disposable-native-qualification.json`:

```json
{
  "purpose": "soundcurrent-native-driver-qualification",
  "computerName": "SOUNDCURRENT-NATIVE-AUDIT",
  "allowDriverChanges": true,
  "expiresUtc": "2026-11-09T00:00:00Z"
}
```

The `computerName` must exactly match `$env:COMPUTERNAME`; use an expiry appropriate to that test session. This marker is authorization metadata, not a credential, and must never be added to the repository.

Run `-Phase Plan` anywhere to inspect the non-mutating plan. For an actual qualification, use an elevated Windows PowerShell in the clone, pass `-Run` to every other phase, retain one result directory across phases, and supply the signed manager/package and current CI binaries explicitly. Use `-RequireHvci` for the HVCI profile. A separate run without HVCI may diagnose compatibility but cannot satisfy the HVCI release gate.

Production acceptance additionally requires a Microsoft-returned production-signed package, the native NSIS app install/update/uninstall lifecycle, Windows 11 and each claimed Windows 10 build, a real microphone/hotplug pass, and review of any Driver Verifier crash dump. A green self-signed or test-mode run is development evidence only.
