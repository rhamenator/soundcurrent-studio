# Windows review handoff — 2026-10-06

These are local unsigned review artifacts. They have not been published and do
not establish completion of the native-driver goal.

| Item | Local location | Scope |
|---|---|---|
| Frozen application/helper/driver source | `.cache/windows-review-source/soundcurrent-studio-source-review-2026-10-06.tar.gz` | 352 files, including uncommitted work and separate MS-PL driver source |
| Qt 6.12.0 source | `.cache/windows-review-source/qtbase-everywhere-src-6.12.0.tar.xz` | Matching pinned Qt source archive; verify SHA-256 below |
| Handoff hashes | `.cache/windows-review-source/review-handoff.json` | Source, Qt and recorded unsigned build-artifact hashes |
| Build evidence | `native/windows/virtual-driver/verification-2026-10-06.json` | Detailed test scopes and remaining gates |

The source tarball contains `SOURCE-REVIEW-MANIFEST.json` with per-file hashes,
base Git HEAD and matching Windows build-workspace inputs. All
230 selected source/resource/build inputs for this
repository matched the current tested Windows workspaces. The total across both
apps was 448. Exact binary reproducibility has not been established. SDK/WDK,
MSVC and runtime prerequisites remain governed by their original terms; they are
not included as application source.

Source SHA-256: `0aa1cce032ee4a64be3e77d031f16df85466897026cb238a93dee461ca586d0c`

Qt source SHA-256: `a951bd163c7b80fc6b8c88d7668fb56abf91c152373e13c10666763238131307`

The snapshot is immutable and was taken before this handoff index was added.
Later worktree edits do not change it. Source and Qt archive bytes were read back
and verified, and the recorded installer/payload/driver archive hashes matched.
No credentials, signing keys, ignored caches or Git internals were included.

The driver remains unsigned and uninstalled. Existing cable fixtures are
userspace test infrastructure. Use [the completion audit](../native/windows/virtual-driver/COMPLETION-AUDIT.md)
and [signing handoff](windows-signing.md) for the remaining signed own-driver,
installer and lifecycle acceptance requirements.
