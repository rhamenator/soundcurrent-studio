# Retrieve the final Windows native-driver retest bundle from Linux

All driver and Studio integration fixes are pushed on Studio PR #13, branch
`codex/native-windows-driver-build-handoff`. The frozen tested build source is
`30d064a0c9f7d75297cf8c9819263b8353a1d98f`; later commits add deployment,
startup and standard-mode preparation evidence. Fetch the current PR branch
for the latest reports without overwriting another task's checkout.

```sh
git fetch origin codex/native-windows-driver-build-handoff
gh release download native-driver-test-inputs-30d064a \
  --repo rhamenator/soundcurrent-studio \
  --dir native-driver-retest-30d064a
cd native-driver-retest-30d064a
sha256sum SoundCurrent-native-driver-retest-30d064a.zip
```

Use authenticated `gh` with access to this repository: the release is deliberately
draft, not a public production release. Its tag is
`native-driver-test-inputs-30d064a`, even if GitHub's draft web URL displays an
`untagged-...` identifier.

Expected ZIP SHA-256:
`367e3ddf132c565f8c6054feb1f764884fd3ae1604f6f6214e3cfcc61447fc15`.

The ZIP and SHA-256 sidecar were downloaded afresh from GitHub on November 7
and the ZIP hash matched. Its manifest, all unsigned/signed INF/SYS/CAT inputs,
source archive and required source files were checked in the downloaded copy.
Verification record: `tests/results/native-driver/2026-11-07-standard-mode-preparation/github-bundle-verification.json`.

## Included changes and evidence

- Processing-mode data-range attributes that fix format negotiation on all
  three endpoints.
- Native endpoint-role discovery through connected topology reference strings,
  with wrong-direction/unknown-role handling and distinct microphone labels.
- Studio loopback source mute handling without double volume attenuation.
- Native capture topology using Windows software volume/mute instead of
  SYSVAD's nonfunctional simulated hardware mixer controls.
- Explicit live GUI completion evidence and digital signal/control/recovery
  regression coverage, including microphone capture volume/mute.

The bundle contains `source-30d064a.zip`, separate unsigned and locally
test-signed driver directories, manifests and per-file hashes, public test
certificate, build/package/update/rollback scripts, and build, validator,
signature and runtime logs. The source archive contains the full frozen Git
revision and its notices/licenses. WDK/SDK package versions are recorded for
reproduction; those external toolchain downloads are not vendored in the ZIP.

The private signing key is not transferred. For an independent Windows worker,
use its own test key and prerequisites. The locally signed directory is evidence
from the destination PC, not a production-trusted distributable.

Read `docs/native-windows-driver-integration-result-2026-11-07.md` on the latest
PR branch for later machine-state changes. The Windows PC's native test device
is now disabled, test-signing is configured OFF for its next boot, and Studio
uses the validated VB-CABLE fallback. WDK and all original/retest bundles remain
installed/preserved. Actual standard-mode boot and Secure Boot verification
await the user's restart. No physical microphone was connected for this run.

The original failing `59dbb99b...` draft release remains intact. Do not overwrite
that artifact or treat its format failures as the final candidate's results.
