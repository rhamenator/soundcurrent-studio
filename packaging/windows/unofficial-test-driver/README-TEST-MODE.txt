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
you. To try the driver, you do those steps yourself, in an elevated PowerShell,
then reboot:

  bcdedit /set testsigning on
  certutil -addstore Root          "<install folder>\unofficial-test-mode\SoundCurrent-Local-Test.cer"
  certutil -addstore TrustedPublisher "<install folder>\unofficial-test-mode\SoundCurrent-Local-Test.cer"

Then use "Audio driver setup" from the Start menu.

To undo everything:

  1. Uninstall SoundCurrent Studio (this removes the shared driver if no other
     SoundCurrent app uses it).
  2. bcdedit /set testsigning off
  3. certutil -delstore Root          <thumbprint of SoundCurrent Local Test>
     certutil -delstore TrustedPublisher <same thumbprint>
  4. Reboot, then re-enable Secure Boot in firmware if you turned it off.

Driver build: development test bundle from source commit 30d064a (see
docs/native-windows-driver-user-guide.md). The driver is derived from
Microsoft's SYSVAD sample (MS-PL).
