# Native Windows security review — 7 November 2026

This is a bounded review of the PR #13 workflow, privileged manager and selected modified driver paths. It is not a kernel security certification. See [independent runtime audit](native-windows-driver-independent-audit-2026-11-07.md) for exact candidate hashes, provenance, VM results and limitations.

## Finding fixed: incomplete signing input coverage

The old local signing helper accepted three differently spelled records if Select-Object -Unique considered their names distinct. That comparison is case-sensitive while the Windows file names are not. A manifest containing INF, uppercase INF and CAT could omit the SYS checksum yet pass the coverage check. An altered unsigned SYS could then be signed without matching its recorded hash.

The replacement requires exactly the closed three-name set using an OrdinalIgnoreCase HashSet, validates record types and SHA-256 syntax, then verifies every file. The helper copies INF/SYS without overwriting an existing destination and checks the copied bytes against the manifest before invoking signing tools. This also detects changes between the source hash check and copying. The destination is a new empty local signing directory; this is not protection against an attacker who already controls the signing account or its private key. Hash consistency is not authentication of an untrusted source manifest.

`tests/windows_driver_signing_inputs.ps1` uses synthetic files only. Valid inputs, case aliases, missing records, traversal names, wrong hashes/types and non-overwriting verified snapshots pass/reject as expected. The test passed in Windows PowerShell 5.1 on CI at 2472c7b, and PowerShell 7 on Linux. No private certificate was created, accessed by these tests or uploaded.

## Package/privilege boundaries checked

* The independent Windows 11 VM accepted the unchanged signed package and rejected separately modified INF, SYS and CAT files. The manager verifies exact catalog membership, INF identity and Windows trust before installation, including after copying into Program Files staging. It does not enable test mode, import trust or force a rejected kernel signature.
* Installation/removal require elevation. Per-app ownership and imported OEM package journals are in HKLM; setup is serialized. Cleanup restricts hardware identity and OEM INF identity, and does not remove physical audio drivers. Recovery paths constrain staging to the expected Program Files hierarchy and reject ancestor reparse redirection. This was source inspection plus lifecycle tests, not an exhaustive malicious-ACL/reparse race test.
* The tested MSVC manager has ASLR/high-entropy VA, DEP and Guard CF PE flags and a static CRT. The privileged manager has no Qt dependency. These flags are hardening, not proof that the implementation is memory-safe.
* Exact public test trust was added only to the approved disposable Windows 11 clone, then removed. Normal reboot verified Secure Boot restored, test-signing absent, native device removed and original audio defaults/levels/mutes restored. Original VMs and the Linux host security policy were not changed.
* No private test key is required to reproduce a build. Each developer supplies their own local certificate; public CER files confer test trust only when explicitly installed. Self-signing is a development workflow, not a production-signing exemption.

## Modified kernel transport checks

The microphone transport uses a fixed-capacity single-producer/single-consumer ring. Epoch/state checks prevent stale audio after pause/restart, overflow drops new frames, and underflow supplies silence. Reviewed WaveRT copy paths bound replay after a stalled timer to one DMA buffer; microphone feed uses its ring rather than the sample driver's file logging path.

The userspace `windows_mic_transport.cpp` fixture passed Clang 21 AddressSanitizer + UndefinedBehaviorSanitizer and a separate ThreadSanitizer run, including concurrent stress. These tools exercised the portable transport fixture, not the kernel binary or the entirety of vendored SysVAD. Functional Windows tests covered gain/EQ/mute, routing and recovery. They do not establish resistance to malformed Kernel Streaming requests, low-resource failures or hostile local applications.

## Open release qualifications

1. Actual kernel Driver Verifier, malformed format/property request testing and longer low-resource/concurrency stress on a disposable VM are still required before a production security claim. Hardware/HVCI combinations need their own coverage.
2. The unchanged signed candidate and INF currently target Windows 11 build 22621+. Windows 10 compatibility is not established, and editing the INF invalidates its catalog. A lower-floor build must be rebuilt, cataloged, signed and independently tested.
3. The EQ repository must receive the reviewed shared manager/workflow changes before claiming the same native package behavior. Sharing the SYS file alone does not update the EQ app's routing or installer code.
4. Manager default restoration has a bounded settling interval; abrupt termination and arbitrarily late endpoint publication are not fully qualified. This is a recovery limitation, not evidence of an exploit.
5. No exhaustive secret scanner, third-party vulnerability scanner, kernel fuzzer or external penetration test was run. The source and complete Windows shipping dependencies must be reevaluated at the eventual release commit.
