# Reproduce the Windows native driver build from a Linux-hosted workflow

## Starting points and scope

Read PR #13, the [verified bundle retrieval instructions](native-windows-driver-linux-retrieval-2026-11-07.md)
and [runtime report](native-windows-driver-integration-result-2026-11-07.md).
The frozen passing driver/app inputs are commit
`30d064a0c9f7d75297cf8c9819263b8353a1d98f`. Retrieve the latest PR branch for
these portable scripts and later reports. Keep the original failing bundle,
passing bundle, and each new build output separate. Do not overwrite another
issue's working checkout; use an isolated Git worktree.

The original bundle's local scripts are historical evidence: they contain this
PC's paths, certificate thumbprint, endpoint GUIDs and root instance. Do not
execute them unchanged on a different machine. The new scripts accept paths
and record the actual source revision. The production/user wrapper deliberately
reuses the existing trust-checking shared-owner manager rather than introducing
a bypass into it.

Linux can orchestrate the work, fetch source and host a Windows VM/worker.
The recorded MSVC/WDK build commands run **inside native Windows x64**, not
directly under Linux or as an unvalidated Wine substitute. Record the worker
OS build and test locally on Windows. Do not change the Linux host's firmware.

## Required Windows build environment

| Requirement | Recorded/required configuration |
| --- | --- |
| Driver source | Full Git checkout, SYSVAD vendor source and MS-PL notices retained |
| OS/architecture | Windows x64 worker; runtime validation was Windows 11 Pro build 26300; INF targets 22621+ x64 |
| Visual Studio | VS 2026 Community 18.11.3 (18.11.12328.437), Desktop development with C++, x64 MSVC 14.51.36231 and Windows Driver Kit individual component |
| Global kit integration | WDK/SDK 28000.2526 installed; `WindowsKernelModeDriver10.0` x64 PlatformToolset must exist in VS MSBuild; NuGet restore alone did not supply it |
| Pinned NuGet inputs | Microsoft.Windows.WDK.x64, Microsoft.Windows.SDK.CPP, Microsoft.Windows.SDK.CPP.x64, all 10.0.28000.2526 |
| Toolchain identities | MSBuild 18.11.1-1.26513.3; actual compiler printed MSVC 19.51.36260; retain file versions/logs if installer component versions differ |
| Restore tooling | Git and .NET SDK with `dotnet nuget verify --all`; recorded .NET SDK 10.0.403 |
| PowerShell | Build script validated with PowerShell 7.6; wrapper parsing/Status/trust rejection also checked under clean Windows PowerShell 5.1. Do not inherit PowerShell 7 module paths when launching 5.1 from another host |
| Network/disk | Official Microsoft/NuGet downloads; roughly 330 MB of NuGet archives plus extracted SDK/WDK, VS and build outputs; provision several GB free |
| Resource limits | BelowNormal process priority, `/m:1`, `CL_MPCount=1`, `UseMultiToolTask=false`, `BuildInParallel=false`; no competing large dev jobs |

Install VS C++/driver components and global WDK through Microsoft's installers
before running the build script. Match [Microsoft's supported WDK/VS matrix](https://learn.microsoft.com/windows-hardware/drivers/other-wdk-downloads).
Our original Microsoft-signed WDK bootstrapper used
`https://go.microsoft.com/fwlink/?LinkId=2371554`; verify its Microsoft signature
and the resulting version rather than assuming a moving download is identical.
The VS component ID used here was `Component.Microsoft.Windows.DriverKit`.
Installers may request elevation/restart; do not silently reboot the worker.

## Portable unsigned build

From a clean worktree on Windows, call the script from the latest PR checkout.
`SourceRoot` may point to a separate clean checkout of the frozen revision;
the script's own revision must also be recorded with the evidence.

```powershell
.\native\windows\virtual-driver\reproduce-build.ps1 `
  -SourceRoot C:\work\studio-frozen `
  -ExpectedRevision 30d064a0c9f7d75297cf8c9819263b8353a1d98f `
  -OutputDirectory C:\artifacts\native-30d064a-run1
```

Use a new output directory outside the checkout. The script verifies all NuGet
archive signatures, refreshes restored inputs, rebuilds common library/driver,
runs InfVerif `/u`, strict Universal API validation and Inf2Cat for 22H2/24H2/
25H2 x64, and records hashes, exact source archive, versions and logs. It never
installs a driver or changes boot/trust settings. ARM64 is not validated here.

"Reproduce" means the recorded source, configuration, dependencies and checks.
Do not promise bit-identical SYS/CAT/ZIP outputs: StampInf stamps the current
date/version, PE build timestamps and test signatures differ. Retain the
original artifact to compare behavior and inputs. Build/validator success does
not establish runtime correctness; the manifest starts `runtimeTested=false`.

## Optional local test signing on an authorized Windows test worker

1. Record Secure Boot, BCD/test-signing, HVCI, BitLocker/recovery readiness,
   default audio roles, volume/mute, app startup entries and installed drivers.
   Have the device/package/trust rollback journal ready. Do not assume this PC's
   decrypted disk or recovery configuration applies elsewhere.
2. Use a **worker-local**, nonexportable code-signing key. Example creation in
   that worker's original user's PowerShell:

   ```powershell
   $cert = New-SelfSignedCertificate -Type CodeSigningCert `
     -Subject 'CN=SoundCurrent Local Driver Test' -CertStoreLocation Cert:\CurrentUser\My `
     -KeyExportPolicy NonExportable -HashAlgorithm SHA256
   Export-Certificate -Cert $cert -FilePath C:\artifacts\local-test-public.cer
   ```

   Journal whether that exact thumbprint was present in LocalMachine Root and
   TrustedPublisher **before** importing its public certificate there with
   elevation. Only explicitly authorized test workers should import this trust.
   Never trust an internal Microsoft build root to silence a tool warning;
   obtain tools through signature-verified official NuGet archives instead.
3. Follow Microsoft's [test-signing procedure](https://learn.microsoft.com/windows-hardware/drivers/install/the-testsigning-boot-configuration-option).
   Secure Boot changes are manual firmware actions. Set test-signing on only
   for the authorized worker, restart, and verify the runtime state. Keep HVCI
   enabled. Do not automate these steps on the user's standard-mode PC.
4. Use a new directory, exact local thumbprint and verified tool paths:

   ```powershell
   .\native\windows\virtual-driver\test-sign-package.ps1 `
     -BuildDirectory C:\artifacts\native-30d064a-run1 `
     -OutputDirectory C:\artifacts\native-30d064a-signed1 `
     -CertificateThumbprint $cert.Thumbprint `
     -SignToolPath 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\signtool.exe' `
     -Inf2CatPath 'C:\work\studio-frozen\native\windows\virtual-driver\vendor\packages\Microsoft.Windows.WDK.x64.10.0.28000.2526\c\bin\10.0.28000.0\x86\Inf2Cat.exe'
   ```

   SignTool 10.0.26100.0 x64 was used here; record any alternative tool version.
   SYS must be signed **before** catalog regeneration; then sign CAT and verify
   both INF and SYS membership. The script copies inputs and exports only the
   public certificate. It does not change trust or test-signing itself.

## Installation, exact identity and rollback requirements for Codex

The supported end-user wrapper is documented in the [user guide](native-windows-driver-user-guide.md).
Build the existing `soundcurrent-driver-manager` target with static runtime,
ship its checksum, and retain its trust/identity checks and shared ownership.
Test a trusted package using that manager; Windows still decides kernel-load
eligibility. Generic WinVerifyTrust success is not proof of production approval.
Do not add a production `--force` or trust-bypass option for development inputs.

The historical DevCon installation here predates manager ownership. For any
separate developer-only DevCon fixture, first enumerate exact hardware IDs
`Root\SOUNDCURRENTVAD` and service `SoundCurrentVAD`. Zero matches permits a
single root creation; one match requires an update; multiple matches must stop
for review. Repeated `devcon install` creates duplicates. Use the official
verified WDK DevCon, and record actual instance ID, actual published OEM INF,
driver hashes, service state and certificate ownership immediately. Do not
assume instance paths begin with the hardware ID. This PC assigned
`ROOT\MEDIA\0002`, but that is an observation, not a portable target.

Before every mutation, preserve all six default roles and affected volume/mute,
stop competing processors, preserve the previous package and prepare targeted
recovery. Restore audio defaults in a finally path, including failure. A
developer removal must validate the recorded live HardwareID/service/OEM INF,
then remove only that instance and journaled package; use no force/broad Driver
Store cleanup. Remove test trust only when the journal says that exact entry
was added by the test. Keep originals, logs and source. Do not transfer keys.

**Codex delivery gate:** package and validate the complete first-install,
existing-device update, shared EQ/Studio ownership and last-owner removal
lifecycle on an independent worker before calling the user scripts release-ready.
The new wrapper does not make the locally test-signed bundle a production
installer. The legacy hard-coded local scripts must not be shipped as generic
end-user setup. Ship a separate, clearly labeled test-only harness if DevCon
tests are still necessary, with dynamic identity/journal/default restoration.

## Reproduce Studio integration and package the handoff

For the tested Studio GUI build use Qt 6.5.3 **MinGW x64** and MinGW 13.1.0,
CMake/Ninja, C++20 and Python 3.12.9 for generated CLI catalogs. A Qt 6.5.3 MSVC
build failed with VS 2026's removed `stdext` iterators; do not mix those headers
or ship MSVC Qt DLLs with the MinGW executable. Driver compilation remains MSVC.

Configure with `CMAKE_PREFIX_PATH` for the MinGW Qt installation,
`CMAKE_CXX_COMPILER` for that MinGW g++, `Python3_EXECUTABLE` for the interpreter,
`CMAKE_BUILD_TYPE=Release`, and `CMAKE_CXX_FLAGS_RELEASE=-O1 -DNDEBUG` as used
here. Build `soundcurrent-studio`, `soundcurrent-route-guardian`,
`soundcurrent-cable-setup-guard`, `soundcurrent-driver-manager`,
`soundcurrent-endpoint-identity-test`, `soundcurrent-windows-audio-smoke` and
`soundcurrent-windows-volume-smoke` with `cmake --build ... --parallel 1`.
Deploy matching QtCore/Gui/Widgets/Network/Svg, MinGW runtime, Windows platform,
image/style/network and Schannel TLS plugins. Validate with developer Qt paths
removed from PATH and inspect loaded module paths. Preserve user's app/presets.

Run the exact-endpoint format probe for all three current endpoints, then:

```text
soundcurrent-endpoint-identity-test
soundcurrent-windows-audio-smoke --run-native
soundcurrent-windows-audio-smoke --run-native-mic
soundcurrent-studio --windows-live-ui-test
soundcurrent-studio --windows-live-manual-route-test
soundcurrent-windows-volume-smoke --run-native
```

These opt-in tests temporarily change defaults/levels; use a worker with saved
settings and verify restoration. Quantitative audio tests isolate the virtual
chain to avoid contamination from other physical playback. No measurement mic
is needed for generated-PCM transport. Distinguish physical input/acoustics,
startup-style launch and actual reboot validation in reports. No driver gain
compensation is needed for the passing flat/0 dB/no-profile route.

Deliver unsigned inputs, complete frozen source/licenses, exact script revision,
tool versions and package hashes, validators, runtime before/after evidence,
public certificate if applicable, manifests, per-file hashes and ZIP sidecar.
Publish a new draft artifact; never replace the original failing/passing ZIPs.
Include readable user setup/rollback instructions and the supported scripts in
the distribution. Update PR #13 and the retrieval guide with precise scope,
test gaps and latest artifact checksum.

## Independent-worker runner (follow-up audit)

The original archived run-integration.ps1/local-integration-runner.ps1 is historical:
it relies on the destination PC's temporary AudioCheck.cs and hard-coded paths.
Use the tracked `tests/windows_native_integration.ps1` instead. Its native
`soundcurrent-audio-state` helper enumerates endpoints without opening a microphone,
journals all available default roles and endpoint master levels/mute, restores them
in a finally path and compares the result. No temporary C# source is required.
Build the `soundcurrent-audio-state` and `soundcurrent-native-format-probe` targets
alongside the listed app/test targets and deploy their matching Qt/runtime files.
The Windows CI artifact includes these helpers and the endpoint identity test.

```powershell
.\tests\windows_native_integration.ps1 -Run `
  -BinaryDirectory C:\work\deployed-test-binaries `
  -ResultDirectory C:\artifacts\native-independent-run1
```

Run only on an independent authorized test worker, with no other SoundCurrent
processor running. It discovers endpoint IDs from the adapter/topology identity,
probes all three formats, runs the six integration cases, preserves the explicit
GUI completion marker, and records exact exit codes. An unavailable endpoint or
failed restoration is a failure, not a passing result. This improves reproducibility;
it does not replace real driver installation, reboot or shared-owner lifecycle tests.
