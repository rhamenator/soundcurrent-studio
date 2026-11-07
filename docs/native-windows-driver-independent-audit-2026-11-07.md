# Independent Linux retrieval and Windows VM audit — 2026-11-07

PR: https://github.com/rhamenator/soundcurrent-studio/pull/13

This audit found and corrected fresh-worker setup failures in addition to
checking the original Windows host's audio evidence. It is development/test-mode
evidence, not production-signing or Windows 10 certification.

## Inputs and revisions

- Frozen driver/source bundle: `30d064a0c9f7d75297cf8c9819263b8353a1d98f`.
- Draft asset `SoundCurrent-native-driver-retest-30d064a.zip`, SHA-256
  `367e3ddf132c565f8c6054feb1f764884fd3ae1604f6f6214e3cfcc61447fc15`.
- Workflow supplement `0216e38bd3ede28789ebd0c971d4b87b4e3e14e4`, ZIP SHA-256
  `97de410ce15c13254208c6ba80a9e8d4711c0e77b19256396e014ba112d731fd`.
- All six unsigned/signed INF/SYS/CAT hashes matched their manifests.
- Nested source archive vs frozen Git revision: 1033 byte-identical files, 2110
  text files differing only by CRLF, zero other differences/missing files.
  All 19 supplemental workflow files matched their Git revision.
- App/Qt/test tools: successful Windows CI run `42418413881`, commit `43ce6ca`.
  App SHA-256 `66cdefa115c8cc1afd95be8b8671a82b85e0bff9ec6ef509f6f8d089dc43dbf6`.
- Final tested manager source: `fe548ffbba9fe6d08374ea941dd6c5cbbbb79778`.
  Locally cross-compiled static executable using MinGW GCC 13-win32, SHA-256
  `753c589fc4d2c00fb8234873d6a95e306dea7a557beb6fb77bcf75db6ab7d470`.
  This is not the byte-identical MSVC CI manager artifact. Compile command:

  ```sh
  x86_64-w64-mingw32-g++ -std=c++20 -O2 -D_WIN32_WINNT=0x0A00 \
    -DNTDDI_VERSION=0x0A000000 -static native/windows/driver-manager/main.cpp \
    -o soundcurrent-driver-manager.exe -lsetupapi -lnewdev -lshell32 \
    -lole32 -ladvapi32 -lwintrust -lcrypt32 -luuid
  ```

The app's later `windows_io.cpp` change extracts the existing default-route COM
ABI into a shared header; the independent transport run used the CI binary
before that refactor. Subsequent CI must still pass for the final head.

## Independent environment and method

A full independent clone, `soundcurrent-native-audit-20261107`, ran Windows 11 Pro
build 26200. No backing image, source VM, original Copperfin VM, host boot policy
or physical Windows host was modified. Only this disposable clone received the
explicitly authorized Secure Boot/test-signing/public-trust changes.

Initial state: Secure Boot on, test-signing absent, VBS/HVCI not running. HVCI
was not disabled by this test, and this VM is not HVCI-enabled evidence. The
original Windows host's separate HVCI evidence remains separate. Disk protection
status was recorded before boot changes; no recovery keys were read.

Only the uploaded public certificate was imported. No new certificate/private key
was created or transferred. The owner can later follow
[the independent certificate workflow](native-driver-own-test-certificate.md).
Driver setup ran elevated as the signed-in test user. Public inputs entered via
read-only ISO; text logs returned over a temporary VM-bridge-only, source-IP-
restricted receiver. No physical microphone or acoustic sweep was used.

## Findings and corrections

1. Windows CI localization export originally rejected two new mute HRESULT
   actions. Map them to the existing translated output-mute message.
2. The original integration runner depended on an untracked C# helper and
   machine-specific directories. Add tracked `soundcurrent-audio-state`, format
   probe targets and `tests/windows_native_integration.ps1`; include required
   binaries and the static manager in CI artifacts.
3. The manager rejected a valid trusted non-WHQL package: SetupAPI returned
   `ERROR_AUTHENTICODE_TRUSTED_PUBLISHER` (`0xE0000241`) with FALSE. Accept that
   documented trusted outcome, retain catalog trust checks and verify both exact
   INF and SYS membership in that same catalog. Unknown/untrusted outcomes fail.
4. Read-only ISO input attributes propagated into protected staging and broke
   cleanup. Normalize the copied staging files and recover already journaled
   read-only staging members without touching original inputs.
5. First installation let Windows select the microphone feed and native capture
   as all six defaults. Snapshot roles before installation and conditionally
   restore still-active originals when Windows selected a native endpoint. A
   fresh reinstall with the correction returned with all original roles intact
   **before** the external fixture's restoration. Other new non-native choices
   are not overwritten. The five-second bound is not proof against an abrupt
   manager kill or arbitrarily late endpoint publication; see remaining limits.
6. PowerShell 5.1 retained a nested JSON array in the signing input hash reader.
   Assign the parsed array before wrapping/enumerating it. The same fixture issue
   was corrected; this run did not create a new private signing certificate.

Microsoft describes the non-WHQL result in
[Using SetupAPI to Verify Driver Authenticode Signatures](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/using-setupapi-to-verify-driver-authenticode-signatures).

## Observed results

- Valid signed package accepted; independently altered INF, SYS and CAT rejected
  (exit 30), including by the tracked package verification script.
- Device loaded healthy as `ROOT\SOUNDCURRENTVAD\0000`, error code 0.
- Service image in the Driver Store matched signed SYS SHA-256
  `f0736cd124b7603e37304fafa673c61954c56fe3fc641ba26fc67b3b00b80fca`.
  Do not assume a System32\drivers path or another machine's OEM INF/device ID.
- All three endpoints negotiated format and started/stopped silent streams.
- End-to-end native playback → Studio bridge → microphone feed → native capture:
  +6.00 dB live gain, −12.00 dB live EQ, unchanged bypass, balance, channel
  gain/mute/effects, rejected-layout preservation and restart passed.
- Source volume attenuation matched endpoint dB; source mute/unmute passed.
  Thirty-second signal transport and 96 kHz playback timing passed.
- Native microphone feed/capture gain and mute passed using generated digital
  audio. This does not test a physical microphone or microphone acoustics.
- Live UI control automation, manual Windows output selection, owner termination
  and route-guardian recovery passed. These used Qt offscreen; they are not
  screenshots proving native visual layout/accessibility.
- Integration's before/after six roles and recorded master volume/mute matched.
- Same-package updates for Studio and EQ succeeded; removing Studio retained the
  EQ-owned device; removing the last EQ owner removed it. A subsequent fresh
  install exercised new device creation again and preserved original defaults.

Raw selected text evidence, including historical failures, is under
[`tests/results/native-driver/2026-11-07-independent-vm-fe548ff`](../tests/results/native-driver/2026-11-07-independent-vm-fe548ff/).
`SHA256.json` covers those files. Full unrelated Windows setup logs, binaries,
credentials and private keys are excluded.

## Remaining qualification and claims

- Windows 10: **not yet tested/supported by this native package**. Current INF and
  manager explicitly require build 22621+. Official English x64 Windows 10 22H2
  ISO was downloaded and verified against Microsoft's SHA-256
  `a6f470ca6d331eb353b815c043e327a347f594f37ff525f17764738fe812852e`.
  This is media preparation only, not installation/runtime evidence. A native
  compatibility change requires rebuilt/re-signed matching inputs and an actual
  independent Windows 10 run. Do not lower the INF floor and reuse the old CAT.
- A self-signed development certificate/test-mode install does not establish
  normal-mode production installation on either Windows 10 or Windows 11.
- Native WDK rebuild, new-owner certificate creation/signing, NSIS install/
  uninstall end-to-end, final MSVC binary runtime qualification, Secure Boot on
  with production signing, physical microphones/hotplug, suspend/resume, lengthy
  stress and interrupted elevated-manager installation are separate work.
- Installation default restoration is bounded and best effort during failures;
  abrupt process death is not covered. Consider an INF-level policy for the
  internal microphone-feed endpoint in a separately rebuilt/signed candidate.
  Do not prohibit default selection of the user-facing playback/capture routes
  that the apps deliberately use.
- Shared-driver app-side fixes still need equivalent integration/backport and
  tests in SoundCurrent EQ; a shared driver does not by itself update EQ code.

## Cleanup verification

Final-owner removal passed. Only the test-added Root/TrustedPublisher certificate
entries were removed, and test-signing was disabled. With the VM shut off, its
original saved firmware variables were restored and byte-compared before boot.
A subsequent actual normal-mode boot reported Secure Boot on, no test-signing
entry, neither test trust entry, no native device, and exact original audio
roles/master levels/mute. See `reboot-1794091562509370243` evidence. The clone was
then shut down; the bridge receiver and its temporary UFW rule were removed.
Original VMs and their firmware remained untouched.
