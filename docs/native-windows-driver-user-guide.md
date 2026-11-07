# Native virtual driver: user setup and rollback

## Choose the correct package

The November 7 package is a **development test**, not a production-signed
driver release. It was tested on Windows 11 x64 build 26300; the INF targets
Windows 11 x64 build 22621 or later. Other target builds still need independent
validation. No Dell-specific hardware or measurement microphone is required.

For normal use with Secure Boot enabled, wait for a release whose native driver
meets Microsoft's production signing requirements. Do not install the unsigned
build or treat a local test signature as production approval. The current
signed VB-CABLE route remains the normal-mode fallback. Microsoft documents
[driver signing options](https://learn.microsoft.com/windows-hardware/drivers/dashboard/driver-signing-offerings)
and [test-signing prerequisites](https://learn.microsoft.com/windows-hardware/drivers/install/the-testsigning-boot-configuration-option).

Normal end users need:

- A supported Windows 11 x64 system and a matching SoundCurrent Studio or EQ
  release, including its matching runtime, recovery helper and driver manager.
- Administrator access under the same Windows account that owns the app.
- A complete signed package (`soundcurrentvad.inf`, `.sys`, `.cat`), release
  checksums, publisher identity and the bundled `soundcurrent-driver-manager.exe`.
- A working physical audio output, and the previous app/driver package or
  VB-CABLE fallback retained until the new route is verified.
- Saved work and a record of current playback/recording defaults, volume/mute,
  presets and app startup preference. Quit both SoundCurrent apps using Quit;
  closing their windows can leave them running in the tray.

End users do **not** need Visual Studio, WDK, a compiler, a signing certificate,
or a microphone to use a production package. No script below changes Secure
Boot, test-signing, HVCI, certificate trust or reboots Windows automatically.

## Setup using the release driver manager

The wrapper is `native/windows/virtual-driver/manage-driver.ps1`. A release
must bundle that script with its reviewed static driver-manager executable,
signed package and checksums. The existing November 7 local installation is
not a complete example of that future production distribution.

1. Download the matching release and verify its published ZIP and manager hashes.
   Keep the original files and extract into a new directory.
2. Back up presets and the previous installation. Note physical output and all
   audio default roles. Do not manually set the new virtual output as default
   before Studio/its recovery helper is running.
3. In PowerShell, verify the package before installation:

   ```powershell
   .\manage-driver.ps1 -Action Verify -ManagerPath .\soundcurrent-driver-manager.exe `
     -ExpectedManagerSha256 '<published manager SHA-256>' -PackageDirectory .\driver
   ```

4. Quit both apps. Open PowerShell as administrator under the original user's
   account, then install or update (the same action handles an existing device):

   ```powershell
   .\manage-driver.ps1 -Action Install -App studio `
     -ManagerPath .\soundcurrent-driver-manager.exe `
     -ExpectedManagerSha256 '<published manager SHA-256>' -PackageDirectory .\driver
   ```

5. If Windows requests a restart, save work and restart when ready. Open Studio,
   choose your physical speakers/headphones, and enable playback. Compatible
   builds prefer the native playback endpoint automatically and exclude the
   microphone feed from physical output choices. Windows should use native
   playback while Studio processes it to the physical output.
6. Check music, Windows volume, mute/unmute and Studio quit/reopen. Quitting
   should restore physical defaults and the latest volume/mute. Start with flat
   EQ, 0 dB output gain and no speaker profile to compare direct/native levels;
   the tested driver matches Realtek under those conditions. Do not add driver
   gain compensation for an EQ/profile difference.

Logs are retained under `%LOCALAPPDATA%\SoundCurrent\driver-setup-logs`.
The manager validates package identity, trust and catalog binding, records
ownership and imported packages, avoids duplicate root creation, and retains
shared EQ/Studio ownership. Status absence is distinct from a setup error.
A successful import/enumeration is not proof of signal transport.

## Rollback

1. Quit Studio and EQ and confirm normal physical audio is available. Preserve
   setup/error logs. If an abnormal shutdown left a virtual default selected,
   select the known physical output in Windows Sound settings before proceeding.
2. Run in an elevated PowerShell under the same owning account:

   ```powershell
   .\manage-driver.ps1 -Action Rollback -App studio `
     -ManagerPath .\soundcurrent-driver-manager.exe `
     -ExpectedManagerSha256 '<published manager SHA-256>'
   ```

3. This releases Studio's registered driver ownership. If EQ or another user
   still owns the shared driver, it remains installed; removing it forcibly
   would break that installation. For each app being removed, release its own
   ownership under its original account. Only the final owner removal cleans
   up the manager-owned native device/packages. No Realtek or VB-CABLE driver
   should be removed.
4. Restore the previous application build if required, then reopen Studio on
   its retained VB-CABLE fallback or use Realtek directly. Check all default
   roles, volume/mute and startup preference. Restart only if Windows requests it.

The development PC's original DevCon test device was **not** installed through
the shared manager and therefore has no matching manager ownership record.
`-Action Rollback` is not a removal recipe for that legacy installation. Its
separate journaled rollback validates the actual instance/hardware ID/OEM INF.
Never copy this PC's `ROOT\MEDIA\0002` or `oem54.inf` into another machine's
removal commands, or use a broad `oem*.inf` deletion.

## Development test users only

Use the [developer reproduction guide](native-windows-driver-reproduction.md)
on an explicitly authorized Windows test worker. Requirements include a local
test key/trust journal, runtime test-signing, firmware/BitLocker readiness,
and exact-device rollback prepared **before** installation. Changing boot flags
requires elevation/restart, and Secure Boot firmware steps belong to the user.
Keep HVCI enabled; do not disable it to hide format or signing failures.

When returning to normal mode: quit processing, restore a working physical or
VB-CABLE path, disable/remove only the journaled test device, turn test-signing
off, remove only trust entries added by that test, restart and re-enable Secure
Boot in firmware. Verify the resulting boot mode and fallback volume/mute.
Retain source, bundles, public certificate and logs; WDK may remain installed.
The signing private key stays local and is never part of a distributable bundle.
