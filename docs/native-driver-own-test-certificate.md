# Use your own local certificate to test-sign the native driver

This workflow creates a **self-signed development certificate**, then uses its
private key to sign a driver package. It does not obtain a Microsoft production
signature or make the test driver installable on an arbitrary normal-mode PC.
No purchased certificate or the original developer machine's private key is an
input to these local test-signing steps. Certificate creation is a separate,
explicit action for the owner to perform when ready.

The existing draft bundle's certificate and signatures are historical test
artifacts. Its public certificate cannot sign a new build: it contains no private
key. Do not copy its thumbprint into the commands for your own certificate.

## 1. Build and validate fresh unsigned inputs

Follow [the reproduction guide](native-windows-driver-reproduction.md), using a
clean, reviewed source revision, a native Windows MSVC/WDK worker, and a new
output directory. Keep the unsigned INF/SYS/CAT, source archive, manifest and
validation logs intact. The `reproduce-build.ps1` script does not create a
certificate, import trust, enable test mode or install a driver.

Check the target OS before proceeding. Current native INF/manager inputs require
Windows 11 build 22621 or newer. **Windows 10 installation/runtime support is
unverified and requires a separate compatibility change and independent VM test.**
Do not infer Windows 10 support from the use of Windows 10 SDK/WDK APIs or from a
Windows 11 test. The ordinary VB-CABLE app route and the new native driver are
separate installation paths and need separate evidence.

## 2. Create your own certificate when ready

Run in your own Windows account's PowerShell. Choose a new output directory;
do not overwrite another build, another user's certificate, or an existing key.
For example:

```powershell
$publicDirectory = Join-Path $env:USERPROFILE 'SoundCurrent-test-certificate-public'
if (Test-Path $publicDirectory) { throw 'Choose a new public output directory' }
New-Item -ItemType Directory $publicDirectory | Out-Null

$certificate = New-SelfSignedCertificate -Type CodeSigningCert `
  -Subject 'CN=SoundCurrent Local Development Test' `
  -CertStoreLocation Cert:\CurrentUser\My `
  -KeyExportPolicy NonExportable -HashAlgorithm SHA256

Export-Certificate -Cert $certificate `
  -FilePath (Join-Path $publicDirectory 'local-test-public.cer')
$certificate | Select-Object Subject, Thumbprint, NotBefore, NotAfter, HasPrivateKey
Get-FileHash (Join-Path $publicDirectory 'local-test-public.cer') -Algorithm SHA256
```

The private key remains in the Windows certificate/key store for this account.
Only the public `.cer` is written to the public directory. A nonexportable key
cannot be transferred to another worker; that worker creates its own local test
certificate. Preserve the original unsigned inputs so they can be signed again
with a different development certificate if needed. If recoverable/private-key
backup is required, decide that key-storage policy before certificate creation;
never put a private key or PFX into the repository, logs or public test bundle.

Creating/signing with the certificate does not require changing Secure Boot or
boot policy. Loading the resulting test driver is a later, explicitly configured
test-machine operation. A local certificate is not a production-distribution
signing workaround, including on modern 64-bit Windows 10.

## 3. Sign and verify using the matching local private key

Use the same account that created the certificate, verified Microsoft SignTool
and Inf2Cat tools, and a new output directory:

```powershell
# Set these to the reviewed checkout, unsigned build and verified WDK tools.
$repository = 'C:\work\soundcurrent-studio'
$unsignedBuild = 'C:\artifacts\native-unsigned-run1'
$signedOutput = 'C:\artifacts\native-locally-signed-run1'
$signTool = '<verified SignTool executable path>'
$inf2Cat = '<verified Inf2Cat executable path>'

& "$repository\native\windows\virtual-driver\test-sign-package.ps1" `
  -BuildDirectory $unsignedBuild -OutputDirectory $signedOutput `
  -CertificateThumbprint $certificate.Thumbprint `
  -SignToolPath $signTool -Inf2CatPath $inf2Cat
```

The script verifies the three unsigned input hashes, signs SYS first, regenerates
CAT from the final package, signs CAT, verifies INF/SYS catalog membership and
trust, and emits new signed hashes and a manifest. It exports only the public
certificate. It does not alter trust stores or boot settings. Its final trust
verification requires that the explicitly authorized test worker trusts the
chosen local test certificate. Journal the exact public thumbprint and whether
it was already in each relevant test trust store before making that separate
trust change. Retain any failed-run logs; do not bypass verification or overwrite
the original inputs to make a failure disappear.

Never modify INF or SYS after catalog/signature generation and reuse the old
CAT. Rebuild/re-sign/re-hash the affected package, keeping the original unsigned
and final signed identities distinct. Record source/script revisions and tool
versions: a locally rebuilt timestamp or a different certificate normally changes
binary/catalog hashes, and byte-identical signatures are not promised.

## 4. Independent test-machine validation and cleanup

Use a full independent disposable VM first. Record Secure Boot, test-signing,
HVCI, disk protection/recovery readiness, devices, defaults and levels before
any mutation. Review the actual Windows build's requirements; leave other
machines' boot, trust and driver settings untouched. Import only the reviewed
public certificate into explicitly authorized test trust stores, recording which
entries the test added. Do not transfer the private key.

Follow the existing [user guide](native-windows-driver-user-guide.md) and
reproduction guide for reviewed test-mode prerequisites, manager identity/trust
checks, dynamic device/OEM-INF ownership, and targeted rollback. Do not assume
an endpoint enumeration or successful import proves audio. Run the tracked
`tests/windows_native_integration.ps1` workflow and install/update/shared-owner/
last-owner-removal checks; verify real transport, gain/mute, actual reboot and
before/after state. Record Windows 10 and Windows 11 results separately.

After testing, remove only the test-owned native device/package and only trust
entries added by this test; restore recorded boot/firmware settings and verify
a subsequent normal-mode boot. Keep public certificates, source, hashes and
logs. Do not remove an existing unrelated certificate or physical/VB-CABLE driver.

Microsoft references:
- [Test signing](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/test-signing)
- [New-SelfSignedCertificate](https://learn.microsoft.com/en-us/powershell/module/pki/new-selfsignedcertificate)
- [Loading test-signed code](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/the-testsigning-boot-configuration-option)
- [Kernel-mode signing requirements](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/kernel-mode-code-signing-requirements--windows-vista-and-later-)
