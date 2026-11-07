# Build handoff: native Windows audio driver for local testing

The final tested bundle and every source fix are available through GitHub.
See [Linux retrieval and checksum instructions](native-windows-driver-linux-retrieval-2026-11-07.md).
A fresh GitHub download was verified against the frozen local bundle, including
unsigned/signed driver inputs and the complete source snapshot.

## Latest: full digital transport and Studio integration pass locally

Source `30d064a0c9f7d75297cf8c9819263b8353a1d98f` was rebuilt, locally signed
and installed as `oem54.inf` on the existing test device. Signal-bearing native
transport, Studio processing, endpoint volume/mute, GUI routing and guardian
recovery tests now pass. Additional fixes address endpoint role discovery,
loopback mute and simulated microphone hardware controls. Read the
[integration report](native-windows-driver-integration-result-2026-11-07.md)
and its raw evidence before continuing. No physical microphone was connected;
physical capture and acoustic calibration were not retested. The earlier
sections are historical. Preserve all original inputs and keep this PR draft.

## Latest result: local format fix passes (November 7, 2026)

The destination PC now has a global WDK installation and has built and loaded
source `69f116b661777cf887615fc05449fefbcd452997`. All three native endpoints
pass format negotiation and silent stream start/stop, including 15 repeated
opens. See [the local fix report](native-windows-driver-local-fix-2026-11-07.md)
for exact changes, evidence, limitations, preserved artifacts and rollback.
The next sections describe the historical failure and original work request;
do not overwrite the passing candidate or recreate the existing root device.
Remaining work includes signal-bearing loopback/microphone transport and
Studio integration testing, plus independent review. Keep this PR draft.

## Current task: fix first native runtime failure (November 7, 2026)

The first build below was test-signed and installed on the destination PC.
Kernel load/enumeration passed with HVCI running, but **all three endpoints fail
WASAPI format negotiation and shared-stream initialization with
`AUDCLNT_E_UNSUPPORTED_FORMAT` (`0x88890008`)**. The device is now disabled and
physical Realtek playback defaults restored. The original build handoff and
November 6 build report below are historical; this is now a fix/rebuild task.

Read [the runtime report](native-windows-driver-runtime-result-2026-11-07.md),
[machine-readable results](native-windows-driver-runtime-result-2026-11-07.json),
and the exact probe source/output under
`tests/results/native-driver/2026-11-07-win11-26300/` before changing code.

### Diagnose and fix on the development machine

1. Fetch this PR branch and any newer relevant native stack work; preserve the
   separate volume-control PRs. Start from a clean isolated worktree and record
   the failing source `59dbb99b126654a7e53b208a387fba87574c0385`. The raw probe's
   endpoint GUIDs are local to the destination PC, not portable device identities.
   Adapt discovery to the exact SoundCurrent hardware/service identity and map
   the two render endpoints to playback vs microphone feed. Do not use friendly
   names alone or assume the probe's enumeration order establishes that mapping.
2. Reproduce the failures on the independent Windows VM hosted by Linux. Use a
   local test certificate/private key on that worker and the reviewed test-mode
   prerequisites; do not transfer private keys or alter the Linux host. The
   destination had Secure Boot off, runtime TESTSIGN enabled and HVCI running;
   its SYS/CAT signatures and catalog membership were verified. Do not attribute
   this format error to signing without contrary evidence, disable HVCI to hide
   it, or weaken the production manager's trust checks.
3. Inspect the actual native render/feed/capture WaveRT pin descriptors, data
   ranges and format intersections, supported processing-mode/default formats,
   channel masks/valid bits and block alignment, topology/endpoint property
   wiring, and Audio Engine/Kernel Streaming initialization diagnostics. These
   are investigation targets, not established diagnoses. Review both
   WAVEFORMATEX PCM and valid WAVEFORMATEXTENSIBLE PCM at 48 kHz/stereo/16 bits;
   do not blindly expand advertised formats beyond what the transport supports.
   GetMixFormat and endpoint-volume activation also failed independently of the
   probe's explicitly constructed WAVEFORMATEX, so merely changing that probe's
   input is not enough to claim a driver fix.
4. Make the smallest justified source fix and add meaningful regression coverage
   for the defect. Record failing and passing native runtime probes. For each
   render/feed/capture endpoint require GetMixFormat and supported-format checks
   to produce valid results (handle S_FALSE and free closest-format buffers),
   then initialize and safely start/stop a stream. Test render loopback and
   actual microphone feed-to-capture transport separately. Do not mistake silent
   buffers or endpoint enumeration for end-to-end audio success. Exercise mute/
   volume where implemented; document explicit unsupported controls rather than
   promising controls the driver does not provide.
5. Keep defaults on physical audio while direct endpoint probes run. Capture
   defaults before installing: Windows automatically selected the new virtual
   render endpoint on this PC, interrupting normal audio until it was restored.
   Make future tests restore original roles even after failures. Record exact
   hardware ID, assigned instance ID and published OEM INF. The first install
   produced `ROOT\MEDIA\0002`, not an instance path beginning with
   `ROOT\SOUNDCURRENTVAD`; match the HardwareID property, not that path assumption.
   Repeating DevCon install creates duplicate root nodes. Use the recorded device
   for update/re-enable, and stop on ambiguous matches. Coordinate the common
   driver fix with EQ without overwriting its unrelated work.

### Build and package the next test input

Freeze the fix in a new source commit and use the existing pinned Windows
MSVC/SDK/WDK build route. Repeat compiler, Universal API, InfVerif and Inf2Cat
checks, but runtime evidence is also required before calling the format bug
fixed. If worker reproduction/runtime is blocked, state precisely what remains
unverified and still provide the complete package for a clearly labeled retest.

Provide a **new, separately named unsigned** x64 test-input archive and download
link in this draft PR, with the exact fixed source revision/snapshot, licenses,
INF/SYS/CAT and dependencies, compiler and validator logs, native before/after
probe output, manifest, internal SHA256SUMS and external ZIP SHA-256 sidecar.
Do not overwrite the first failing archive or publish a production release.
Use a valid newer DriverVer for an update path and record it. Do not copy this
PC's test signature or reuse a catalog after changing its payload. We will sign
the new final package locally here and regenerate/re-sign its catalog.

Include a narrowly scoped retest/update/removal plan for this PC: its old test
device is disabled (`ROOT\MEDIA\0002`, `oem49.inf`), its service stopped, and
Realtek/VB-CABLE retained. These IDs must be reverified here, not blindly reused
on the worker or hard-coded into shipping code. Preserve local test certificate
ownership and original unsigned vs signed checksums. No new installation on the
destination PC is authorized by this handoff alone; report the new artifact and
evidence first so this chat can inspect, sign and perform the next controlled test.

Keep PR #13 draft. Update its description around the final fix and results;
retain the distinction between successful build, successful kernel load, valid
stream negotiation and measured audio behavior. Heavy work belongs on the Linux
machine's Windows worker, not this resource-limited Windows desktop.

Status: fresh build and unsigned-input packaging completed. The report below
supersedes the initial build-only request; runtime acceptance is still pending.

## Objective and destination

Codex on the Linux development machine should locate the preserved SoundCurrent
native Windows driver source, build an unsigned x64 driver package in its Windows
build environment, and deliver a traceable archive for inspection, test-signing,
and controlled runtime tests on the user's Windows PC. Build on the development
machine or its Windows VM, not on the resource-limited destination PC.

Destination observed in this chat: Dell OptiPlex 7040, Windows 11 Pro x64,
build 26300. Reconfirm the destination build before testing. Its working audio
route is currently Realtek plus VB-CABLE. Preserve that fallback. The earlier
low-volume investigation identified two computer outputs joined by Y-splitters;
do not confuse that analog wiring problem with native-driver behavior.

## 1. Locate the authoritative source in the development stacks

Fetch the latest refs in both `rhamenator/soundcurrent-studio` and
`rhamenator/soundcurrent-eq`. Inventory local branches, worktrees, stashes and
ignored build/cache archives on the Linux machine without deleting, resetting,
or overwriting existing development work. Do not assume this PR's documentation
commit is the driver source revision.

Refs observed when preparing this handoff:

| Repository | Latest reviewed main | Native bookmark |
| --- | --- | --- |
| Studio | `88f862bb7b879ab31a13933099e100c1b4eff6a2` | `4fbb1c33527173faca2db98db69003d0cd4702b1` |
| EQ | `d83759045621c0dc795e97b384d1d820c8ca66a7` | `97885ab907512e0f6851704792911f7da192fbe5` |

The bookmark ref in both repositories is
`origin/bookmark/windows-native-driver-2026-10-06`. Compare it with current main
and newer local stack work; the bookmark is a discovery anchor, not an instruction
to discard later fixes. Inspect:

- `native/windows/virtual-driver/{README.md,IMPLEMENTATION-STATUS.md,COMPLETION-AUDIT.md,verification-2026-10-06.json}`
- `native/windows/virtual-driver/vendor/` and `build-baseline.ps1`
- `native/windows/driver-manager/` and `packaging/windows/native-audio-setup.ps1`
- `docs/windows-native-driver-bookmark.md` and `docs/windows-signing.md`
- `.cache/windows-review-source/review-handoff.json` and its immutable source
  archive, if present on the development machine. These ignored files were not
  available in this Windows checkout.

Read the latest audit alongside dated notes: older README statements may be
superseded. Recorded builds and tests are historical evidence only. The earlier
own-kernel package was unsigned and never installed; loopback tests using
VB-CABLE do not prove SoundCurrent kernel runtime. Determine whether selected
source actually exposes playback only or also implements microphone transport.

Select a clean, immutable source revision in an isolated worktree. Compare the
driver trees in EQ and Studio and document whether one common package serves
both. If uncommitted changes are necessary, preserve them separately, review
them, and commit them on a dedicated branch before building. Record upstream
SYSVAD provenance and all changes needed to reproduce the chosen source.

## 2. Build in a Windows SDK/WDK environment

Use the existing Windows build VM/worker on the Linux machine if available.
Linux orchestration is fine; a Linux-native app build or Wine invocation is not
evidence of a successful WDK kernel build. If no suitable Windows worker exists,
report that prerequisite with discovered source refs and a concrete build plan.
Do not silently claim completion or move heavy builds to the destination PC.

Current source pins Microsoft Windows SDK CPP, SDK CPP x64 and WDK x64 NuGet
packages to `10.0.28000.2526`, with target SDK `10.0.28000.0`. Recheck pins at the
selected revision. Record Windows worker version, MSVC toolset, SDK/WDK versions,
architecture and package restore source. Preserve the script's NuGet signature
verification. Explain and commit any necessary build fixes or version changes.

Starting command, from the selected source checkout on the Windows worker:

```powershell
powershell.exe -NoProfile -File native/windows/virtual-driver/build-baseline.ps1 -Configuration Release
```

The script currently builds EndpointsCommon and TabletAudioSample for x64 with
`SignMode=Off`, stages `soundcurrentvad.sys` and `soundcurrentvad.inf` under
`vendor/driver-package/`, and generates a catalog with Inf2Cat. Capture full logs
and exit codes; add an MSBuild binary log if needed. Inspect INF CopyFiles and
catalog declarations and include every required package dependency. Run the
available INF/signability and universal-driver API checks, recording exact tool
versions, commands, supported OS identifiers, warnings and failures. Do not treat
the destination Insider build as validated solely by Inf2Cat OS labels.

Do not install the driver on the build host, enable test-signing, change Secure
Boot or Code Integrity, import certificates, replace physical drivers, or submit
anything to Microsoft's signing dashboard as part of this build-only handoff.

## 3. Export a complete, unsigned test-input bundle

Produce one clearly named archive such as
`soundcurrent-native-driver-unsigned-x64-<source-sha>.zip`, containing:

```text
driver/             final INF, SYS, CAT and all INF-referenced payload files
source/             exact source snapshot, build scripts, provenance and licenses
logs/               build, INF/catalog/API validation logs and exit-code summary
manifest.json       source and build identities plus per-file SHA-256 hashes
SHA256SUMS          relative-path hashes for every other file in this archive
README-TESTING.md   prerequisites, signing order, test matrix and rollback plan
```

Include MS-PL driver/SYSVAD notices and relevant source licenses; keep GPL app
licensing distinct. Exclude private keys, credentials, unrelated caches and huge
dependency trees. A source snapshot must include any required submodule content
or exact retrievable references, plus the pinned package restore instructions.

Manifest requirements: repository URL, full source commit, branch/ref as context,
clean-tree confirmation, submodule/provenance commits, UTC build time, Windows
worker/toolchain versions, x64/Release configuration, exact commands and exit
codes, final package filenames and SHA-256 digests. Record any source fixes as
commits. State explicitly: catalog unsigned, runtime untested, not a production
release. Do not confuse a pre-existing artifact with this build's output.

Avoid recursive checksums: manifest hashes the source/log/driver payload;
SHA256SUMS covers all other bundle files including the manifest, but not itself.
Also publish the final ZIP's SHA-256 as a sidecar outside the ZIP. Verify the
extracted archive against SHA256SUMS and the ZIP against its sidecar before
handoff. Upload to this draft PR as a linked CI artifact or provide an authorized
download location and exact retrieval steps; do not commit binaries to main or
publish an unsigned production release.

## 4. Prepare the Windows-PC test instructions, without executing them

Specify that the receiver must verify hashes and inspect INF identity, architecture,
signatures and install/remove operations before loading anything. Expected native
identity is `Root\SOUNDCURRENTVAD` / `SoundCurrentVAD`; never install unrelated
upstream SYSVAD sample INFs or demonstration APOs.

Explain the local test-signing sequence: inspect and freeze inputs, create/use a
local test certificate with its private key retained locally, sign the SYS as
required, regenerate the catalog after final package changes, sign the final
catalog, and verify signatures/catalog membership. Any post-signing modification
requires regenerating the affected signatures/catalog and hashes. Clearly separate
the delivered unsigned-input hashes from final test-signed-package hashes.

List the applicable Windows test-signing/Secure Boot/HVCI prerequisites as items
to check and discuss with the user, not automatic changes. Do not weaken release
installer/manager signature checks to accept this package. If existing native
manager policy rejects self-signed packages, document that and prepare a narrowly
scoped development test procedure; never bypass checks in production code.

Include baseline audio-device/default-role/volume inventory, a documented rollback
that removes only the exact test device/package/service, and recovery instructions
if boot/audio fails. Tests should cover actual native endpoint discovery, playback
and loopback, volume/mute, EQ/Studio compatibility, route restoration after normal
quit/crash, stream start/stop, reboot, and removal. Test microphone behavior only
if selected source implements it. Preserve Realtek and the working VB-CABLE route.
Do not label any of these tests passed until they actually run on the PC.

## Completion report on this draft PR

Report the selected source and why, package contents and download link, archive
SHA-256, build/validation results, supported formats/endpoints, remaining blockers,
and precise next steps on the Windows PC. Update the PR with any source/build
fixes and evidence. Leave it a draft until the requested handoff is reviewable.
This task is separate from the Windows Volume Control installer PRs (#12 here
and rhamenator/soundcurrent-eq#12); do not merge or disturb their work.

## Fresh build completion — November 6, 2026 (America/Detroit)

Source frozen before building: `59dbb99b126654a7e53b208a387fba87574c0385`,
this PR's original handoff revision. Native implementation matches current Studio
main and EQ main; the repositories differ only in dated native audit/evidence
files. The bookmark is older audit evidence, not newer implementation. The other
local EQ worktree has no newer native implementation and was left untouched.
No source/build fixes were required. This report commit changes documentation only.

The independent `soundcurrent-win11-dev` worker rebuilt both libraries/driver in a
new extracted source workspace, using the existing cache rather than copying or
reinstalling the SDK/WDK. Worker Windows 11 Pro `10.0.26300`, Visual Studio
`18.10.12201.205`, MSVC `14.51.36231`, SDK/WDK NuGet `10.0.28000.2526`, target
`10.0.28000.0`, x64 Release. Primary commands:

- `powershell.exe -NoProfile -NonInteractive -ExecutionPolicy RemoteSigned -File native/windows/virtual-driver/build-baseline.ps1 -Configuration Release`
- WDK `infverif.exe /u <fresh-package>/soundcurrentvad.inf`

The wrapper uses process-scoped RemoteSigned because the default script invocation
was rejected; no persistent execution or boot policy was changed. The unchanged
build script retains its signed Microsoft NuGet executable check and exact package
restore pins/source. Full commands, paths and output are in the bundle logs.

Results: EndpointsCommon and TabletAudioSample each built with zero warnings and
errors; the build's Universal ApiValidator passed; explicit InfVerif `/u` returned
zero; Inf2Cat signability and catalog generation passed without warnings/errors
for `10_NI_X64,10_GE_X64,10_25H2_X64`. PE inspection confirms x64. SYS and CAT
report NotSigned. INF is not independently Authenticode-signed; catalog membership
and trust must be checked again after local signing. These checks do not certify
runtime behavior or Insider-build compatibility.

Package has exactly `soundcurrentvad.inf`, `soundcurrentvad.sys`, and
`soundcurrentvad.cat`. INF CopyFiles references only that SYS. Current source
includes playback, dedicated mic feed and mic capture with SPSC transport;
48 kHz stereo 16-bit PCM at the kernel boundary. Neither kernel path was loaded
or tested. No driver, certificate or trust/security setting was installed/changed;
Realtek/VB-CABLE were preserved. The build VM was confirmed shut off afterward.

### Download and verification

The owner-review bundle is attached to a **draft test-input release**, keeping
unsigned binaries out of the source tree and ordinary public release downloads:

[Draft unsigned test-input bundle](https://github.com/rhamenator/soundcurrent-studio/releases/tag/untagged-2ce7dc46a2056e247657)

Authenticate to GitHub as an authorized repository owner/editor, then:

```sh
gh release download native-driver-test-inputs-59dbb99b --repo rhamenator/soundcurrent-studio
```

ZIP: `soundcurrent-native-driver-unsigned-x64-59dbb99b126654a7e53b208a387fba87574c0385.zip`

SHA-256: `07c7569c50e7f6587418a8d308297a4d1f59e17dd586d81f22ca73a86a1cdb63`

Its sidecar and internal SHA256SUMS are included. The archive contains all exact
tracked source files and licenses, INF/SYS/CAT, full compiler/validator output,
worker/build manifest with per-file hashes, and README-TESTING.md. Every source
blob was checked against Git; every archived payload file was read back and
checksum-verified. The uploaded assets were downloaded and verified independently.
The draft release is not a production release and does not change latest v1.3.0.

### Next Windows-PC step (not executed)

Verify hashes and inspect README-TESTING.md first. Reconfirm the PC build and its
working analog wiring/output. Review local test-signing, Secure Boot/HVCI and
recovery prerequisites before any change; use an independent disposable clone for
the first kernel load. The instructions separate unsigned inputs from signed test
outputs, require SYS signing followed by final catalog regeneration/signing, and
preserve the production manager/installer trust gates. The manager uses catalog
trust checks; acceptance of a locally trusted self-signed test certificate is
unverified. No release trust bypass was added. Rollback removes only the exact
recorded root device and OEM INF, retaining Realtek and the VB-CABLE fallback.
All audible playback/mic, volume/mute, reboot, crash/quit and removal tests remain
explicitly unrun. Leave this PR draft for review of those prerequisites.
