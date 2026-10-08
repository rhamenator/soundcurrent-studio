; SPDX-License-Identifier: GPL-3.0-only
Unicode true
!include "MUI2.nsh"
!include "nsDialogs.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"

!ifndef APP_EXE
  !error "Pass /DAPP_EXE=path-to-soundcurrent-studio.exe"
!endif
!ifndef OUTPUT
  !error "Pass /DOUTPUT=path-to-installer.exe"
!endif
!ifndef SOURCE_ROOT
  !error "Pass /DSOURCE_ROOT=path-to-repository"
!endif
!ifndef DLL_DIR
  !error "Pass /DDLL_DIR=complete-app-and-helper-payload"
!endif
!ifndef UNINSTALL_PAYLOAD
  !error "Pass /DUNINSTALL_PAYLOAD=exact-payload-deletion-manifest"
!endif
!ifndef DRIVER_DIR
  !error "Pass /DDRIVER_DIR=path-to-SoundCurrent-INF-SYS-CAT-package"
!endif

!ifndef APP_VERSION
  !define APP_VERSION "0.8.4"
!endif

Var DriverCheck
Var DriverChoice
Var InstallDriver

Name "SoundCurrent Studio"
OutFile "${OUTPUT}"
InstallDir "$LOCALAPPDATA\Programs\SoundCurrent Studio"
InstallDirRegKey HKCU "Software\SoundCurrent\SoundCurrent Studio" "InstallDir"
RequestExecutionLevel user
SetCompressor /SOLID lzma
BrandingText "SoundCurrent Studio • GPL-3.0-only"
Icon "${SOURCE_ROOT}\data\soundcurrent-studio.ico"
UninstallIcon "${SOURCE_ROOT}\data\soundcurrent-studio.ico"

!define MUI_WELCOMEPAGE_TEXT "Install or update SoundCurrent Studio. You do not need to uninstall an older version. Your settings, presets and equipment profiles will be kept.$\r$\n$\r$\nSave your work and use Quit to exit the running app before continuing. Closing its window keeps it running in the background."
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${SOURCE_ROOT}\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
Page custom AudioPage AudioPageLeave
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_REBOOTLATER_DEFAULT
!define MUI_FINISHPAGE_RUN "$INSTDIR\soundcurrent-studio.exe"
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"
LangString SCConnectAudio ${LANG_ENGLISH} "Connect your audio"
LangString SCSetupAudio ${LANG_ENGLISH} "Set up SoundCurrent Audio for SoundCurrent Studio."

Function .onInit
  ; Migrate the old install location, including custom folders.
  ReadRegStr $1 HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "DisplayIcon"
  ${If} $1 != ""
    ${GetParent} $1 $2
    IfFileExists "$2\soundcurrent-studio.exe" 0 +2
      StrCpy $INSTDIR $2
  ${EndIf}
  StrCpy $InstallDriver 0 ; Silent app updates never install/elevate a driver.
  InitPluginsDir
  SetOutPath "$PLUGINSDIR"
  File "/oname=audio-setup.ps1" "${SOURCE_ROOT}\packaging\windows\native-audio-setup.ps1"
  nsExec::ExecToStack /TIMEOUT=20000 '"$SYSDIR\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -NonInteractive -ExecutionPolicy RemoteSigned -File "$PLUGINSDIR\audio-setup.ps1" -Check'
  Pop $DriverCheck
  Pop $0
FunctionEnd

Function AudioPage
  !insertmacro MUI_HEADER_TEXT "$(SCConnectAudio)" "$(SCSetupAudio)"
  nsDialogs::Create 1018
  Pop $0
  ${If} $0 == error
    Abort
  ${EndIf}
  ${NSD_CreateLabel} 0 0 100% 32u "SoundCurrent Audio routes playback through the app. Choose your physical speakers or headphones inside the app. Their hardware drivers are preserved."
  Pop $0
  ${NSD_CreateCheckbox} 0 38u 100% 18u "Install or update the shared SoundCurrent Audio driver"
  Pop $DriverChoice
  ${If} $DriverCheck == 0
    ${NSD_Check} $DriverChoice
    ${NSD_CreateLabel} 0 65u 100% 35u "SoundCurrent Audio is already present. Setup will register this app and keep the shared driver available for the other SoundCurrent app."
  ${ElseIf} $DriverCheck == 10
    ${NSD_Check} $DriverChoice
    ${NSD_CreateLabel} 0 65u 100% 35u "Windows will request administrator approval for the signed driver manager. Setup will tell you if a restart is required."
  ${Else}
    ${NSD_CreateLabel} 0 65u 100% 35u "Setup could not check the driver. You can retry with Audio driver setup in the app or Start menu."
  ${EndIf}
  Pop $0
  ${NSD_CreateLabel} 0 108u 100% 40u "Quit both EQ and Studio before changing the shared driver. Removing one app keeps the driver if the other app still uses it."
  Pop $0
  nsDialogs::Show
FunctionEnd
Function AudioPageLeave
  ${NSD_GetState} $DriverChoice $InstallDriver
FunctionEnd

Section "SoundCurrent Studio" main
  FindWindow $0 "" "SoundCurrent Studio"
  StrCmp $0 0 +3
    MessageBox MB_ICONEXCLAMATION "Quit SoundCurrent Studio before updating. Closing the window keeps it running. No uninstall is needed."
    Abort
  SetOutPath "$INSTDIR"
!ifdef DLL_DIR
  File /r "${DLL_DIR}\*"
!else
  File "/oname=soundcurrent-studio.exe" "${APP_EXE}"
!endif
  File "/oname=soundcurrent-driver-manager.exe" "${DLL_DIR}\soundcurrent-driver-manager.exe"
  File "/oname=soundcurrent-route-guardian.exe" "${DLL_DIR}\soundcurrent-route-guardian.exe"
  File "${SOURCE_ROOT}\LICENSE"
  File "${SOURCE_ROOT}\COPYRIGHT"
  File "${SOURCE_ROOT}\README.md"
  File "/oname=audio-setup.ps1" "${SOURCE_ROOT}\packaging\windows\native-audio-setup.ps1"
  File "${SOURCE_ROOT}\THIRD-PARTY-NOTICES.md"
  SetOutPath "$INSTDIR\licenses"
  File "/oname=SoundCurrent-driver-MS-PL.txt" "${SOURCE_ROOT}\native\windows\virtual-driver\vendor\LICENSE-MS-PL.txt"
  SetOutPath "$INSTDIR\audio-driver"
  File "${DRIVER_DIR}\soundcurrentvad.inf"
  File "${DRIVER_DIR}\soundcurrentvad.sys"
  File "${DRIVER_DIR}\soundcurrentvad.cat"
  SetOutPath "$INSTDIR"
  ; Remove old bundled files/shortcut, preserving any existing VB-Audio driver.
  Delete "$SMPROGRAMS\SoundCurrent Studio\Install VB-CABLE.lnk"
  Delete "$INSTDIR\VBCABLE_Driver_Pack45.zip"
  Delete "$INSTDIR\VB-CABLE-NOTICE.txt"
  WriteRegStr HKCU "Software\SoundCurrent\SoundCurrent Studio" "InstallDir" "$INSTDIR"
  WriteUninstaller "$INSTDIR\uninstall.exe"
  CreateDirectory "$SMPROGRAMS\SoundCurrent Studio"
  CreateShortcut "$SMPROGRAMS\SoundCurrent Studio\SoundCurrent Studio.lnk" "$INSTDIR\soundcurrent-studio.exe"
  CreateShortcut "$SMPROGRAMS\SoundCurrent Studio\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortcut "$SMPROGRAMS\SoundCurrent Studio\Audio driver setup.lnk" "$SYSDIR\WindowsPowerShell\v1.0\powershell.exe" '-NoProfile -ExecutionPolicy RemoteSigned -File "$INSTDIR\audio-setup.ps1" -Install' "$INSTDIR\soundcurrent-studio.exe"
  CreateShortcut "$DESKTOP\SoundCurrent Studio.lnk" "$INSTDIR\soundcurrent-studio.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "DisplayName" "SoundCurrent Studio"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "DisplayIcon" "$INSTDIR\soundcurrent-studio.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "Publisher" "SoundCurrent Studio contributors"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "DisplayVersion" "${APP_VERSION}"
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio" "NoRepair" 1
  ${If} $InstallDriver == ${BST_CHECKED}
    DetailPrint "Setting up the shared SoundCurrent Audio driver..."
    nsExec::ExecToStack '"$SYSDIR\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -ExecutionPolicy RemoteSigned -File "$INSTDIR\audio-setup.ps1" -Install -Quiet'
    Pop $0
    Pop $1
    DetailPrint $1
    ${If} $0 == 3010
      SetRebootFlag true
    ${ElseIf} $0 != 0
      DetailPrint "SoundCurrent Audio setup did not finish. Retry using the Start menu shortcut."
      MessageBox MB_OK|MB_ICONINFORMATION "SoundCurrent Audio was not installed. SoundCurrent Studio itself is installed. Use Audio driver setup in the Start menu to retry; see setup details for the reason."
    ${EndIf}
  ${EndIf}
SectionEnd

Section "Uninstall"
  FindWindow $0 "" "SoundCurrent Studio"
  StrCmp $0 0 +3
    MessageBox MB_ICONEXCLAMATION "Quit SoundCurrent Studio before uninstalling it."
    Abort
  ; Remove only this application's shared per-user login entry.
  ReadRegStr $1 HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "SoundCurrent"
  StrCmp $1 '"$INSTDIR\soundcurrent-studio.exe" --background' 0 +2
    DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "SoundCurrent"
  ; Release shared ownership before deleting the manager or setup script.
  nsExec::ExecToStack '"$SYSDIR\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -ExecutionPolicy RemoteSigned -File "$INSTDIR\audio-setup.ps1" -Remove -Quiet'
  Pop $0
  Pop $1
  DetailPrint $1
  ${If} $0 == 3010
    SetRebootFlag true
  ${ElseIf} $0 != 0
    MessageBox MB_ICONEXCLAMATION "Shared audio driver removal did not finish. This app was kept so you can retry. Quit EQ and Studio, then retry uninstalling."
    Abort
  ${EndIf}
  Delete "$DESKTOP\SoundCurrent Studio.lnk"
  Delete "$SMPROGRAMS\SoundCurrent Studio\SoundCurrent Studio.lnk"
  Delete "$SMPROGRAMS\SoundCurrent Studio\Uninstall.lnk"
  Delete "$SMPROGRAMS\SoundCurrent Studio\Audio driver setup.lnk"
  Delete "$SMPROGRAMS\SoundCurrent Studio\Install VB-CABLE.lnk"
  RMDir "$SMPROGRAMS\SoundCurrent Studio"
!ifdef UNINSTALL_PAYLOAD
  !include "${UNINSTALL_PAYLOAD}"
!else
  Delete "$INSTDIR\soundcurrent-studio.exe"
!endif
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\COPYRIGHT"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\audio-setup.ps1"
  Delete "$INSTDIR\audio-driver\soundcurrentvad.inf"
  Delete "$INSTDIR\audio-driver\soundcurrentvad.sys"
  Delete "$INSTDIR\audio-driver\soundcurrentvad.cat"
  RMDir "$INSTDIR\audio-driver"
  Delete "$INSTDIR\licenses\SoundCurrent-driver-MS-PL.txt"
  RMDir "$INSTDIR\licenses"
  Delete "$INSTDIR\THIRD-PARTY-NOTICES.md"
  Delete "$INSTDIR\VBCABLE_Driver_Pack45.zip"
  Delete "$INSTDIR\VB-CABLE-NOTICE.txt"
  Delete "$INSTDIR\uninstall.exe"
  RMDir "$INSTDIR"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoundCurrentStudio"
SectionEnd
