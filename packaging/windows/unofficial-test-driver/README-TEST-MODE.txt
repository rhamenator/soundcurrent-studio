SoundCurrent Studio - UNOFFICIAL native-driver build (test mode only)
=====================================================================

This installer bundles the SoundCurrent virtual audio driver, which is signed
only with a self-signed development certificate (SoundCurrent-Local-Test.cer).
Windows will load it ONLY if the PC is in TEST MODE and trusts that certificate.

  * Test mode lowers Windows driver-signature protection for the WHOLE PC.
    It is NOT recommended for everyday or production machines.
  * Use a virtual machine or a dedicated test PC. Windows 11 x64 (build 22621
    or newer) is required; Windows 10 and Windows 7 are not supported by this
    driver.
  * Secure Boot must be turned off in firmware before test mode can be enabled.
  * The normal SoundCurrent installers (VB-CABLE route) need none of this.
  * This software is provided under GPL-3.0-only WITHOUT WARRANTY. You alone
    are responsible for changing your own system's security settings.

This installer does NOT change test mode, Secure Boot or certificate trust for
you. Do these steps yourself in an ELEVATED PowerShell, then restart:

  bcdedit /set testsigning on
  certutil -f -addstore Root "<install folder>\unofficial-test-mode\SoundCurrent-Local-Test.cer"
  certutil -f -addstore TrustedPublisher "<install folder>\unofficial-test-mode\SoundCurrent-Local-Test.cer"

(-f is required: a fresh Windows has no TrustedPublisher store yet.)

After the restart, quit any SoundCurrent app and choose "Audio driver setup"
from the Start menu. In this unofficial build the driver manager is not
Authenticode-signed; setup accepts it only because its exact SHA-256 is pinned
in the setup script. Windows still verifies the driver itself against the
certificate you trusted above. Manual alternative, in an elevated PowerShell:

  $sid = [Security.Principal.WindowsIdentity]::GetCurrent().User.Value
  & "<install folder>\soundcurrent-driver-manager.exe" --install studio "<install folder>\audio-driver" $sid

"SoundCurrent Audio" (speakers and microphone) should then appear in Windows
Sound settings. Tested on Windows 11 Pro build 26100 in a virtual machine.

To undo everything:

  1. Quit SoundCurrent Studio, then in an elevated PowerShell:
     & "<install folder>\soundcurrent-driver-manager.exe" --remove studio $sid
  2. Uninstall SoundCurrent Studio.
  3. bcdedit /set testsigning off
  4. certutil -delstore Root "SoundCurrent Local Driver Test 2026-11-07"
     certutil -delstore TrustedPublisher "SoundCurrent Local Driver Test 2026-11-07"
  5. Restart, then re-enable Secure Boot in firmware if you turned it off.

Driver build: development test bundle from source commit 30d064a (see
docs/native-windows-driver-user-guide.md). The driver is derived from
Microsoft's SYSVAD sample (MS-PL).
