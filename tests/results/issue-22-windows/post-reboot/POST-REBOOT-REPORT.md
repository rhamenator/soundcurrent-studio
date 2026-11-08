Post-reboot verification, 2026-11-08

Windows restarted at 03:04:30 Eastern. Studio started automatically, with studioOfflineEditing=false and serialized offline=false; output gain remained -10 dB. The user reported Channels & Effects working. Realtek Audio enhancements remained Off (Disable_SysFx=1), and Secure Boot remained enabled.

Volume persistence FAILED on that first reboot: Software VBAudioCableWDM_VolCtl=1 remained saved, but both installed live probes returned 12. Real audio at Windows 50% measured 0.00 dB attenuation against an expected -10.51 dB. The SYSTEM fallback value was missing. This supersedes the earlier session's not-yet-rebooted result.

Static inspection of the signed installed Pack45 driver 3.3.1.7 shows the Software path queried first and the SYSTEM fallback queried next. The pinned vendor control panel's persistence routine writes the same REG_DWORD to BOTH HKLM\SOFTWARE\VB-Audio\Cable and HKLM\SYSTEM\CurrentControlSet\Services\VB-Cable. Our original helper omitted the second location. The revised fixed-purpose helper mirrors both writes, flushes and reads both back, migrates Software-only state, then applies live volume control. Unit tests cover both locations, migration, missing/disabled fallback, boot-write failure, live state and idempotence. This is vendor-driver configuration, with no Realtek-specific compensation and no native-driver changes.

The explicit administrator-approved helper completed with exit 0. Both DWORDs now read 1. Repaired Studio transport passed Windows volume attenuation (-10.10 dB; endpoint -10.51 dB), real media mute (RMS 0/0), unmute and volume-down, gain/EQ/input trim, effects, balance and stream restart. A SECOND full Windows reboot has not occurred since the dual-location correction, so revised boot persistence is still pending that test.

UI revision: compact mutually exclusive Live and Offline command buttons, colored only when selected. Apply stays visible beside them and is disabled when already live. Full shared UI self-test passed. Offline caption was added to all 34 catalogs; structural/catalog validation passed with 1009 messages and no unfinished entries (translations not independently reviewed).

Published files and original driver rollback folders remain intact. The quarantined combined PowerShell rollback script was not restored or executed; rollback can use the verified original installers and preserved files instead.

Both repaired Studio and EQ audio transport tests passed. Studio's compact-button live UI measured Apply -5.99126 dB, trim -8.98456 dB, mute 0 / 0.00110174, solo 0.00110172 / 0, effects -2.79169 dB, with normal route restoration. Installed visual inspection confirmed compact Live/Offline/Apply controls, teal Live and amber Offline selection, fixed Apply placement and Live selected on exit. Installed Studio SHA256: 9938BD00C2E8690909FAFE6C70C78121C39151F4351F455A1217862AF486EAF7. Both installed helpers report ready (0).
