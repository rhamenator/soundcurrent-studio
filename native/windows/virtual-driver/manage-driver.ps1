# SPDX-License-Identifier: GPL-3.0-only
# End-user wrapper for the existing shared-owner/trust-checking manager.
# No test-signing, certificate import, Secure Boot or reboot changes.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][ValidateSet('Status','Verify','Install','Rollback')][string]$Action,
 [Parameter(Mandatory=$true)][string]$ManagerPath,
 [ValidateSet('studio','eq')][string]$App='studio',
 [string]$PackageDirectory,
 [Parameter(Mandatory=$true)][ValidatePattern('^[A-Fa-f0-9]{64}$')][string]$ExpectedManagerSha256,
 [string]$LogDirectory=(Join-Path $env:LOCALAPPDATA 'SoundCurrent\driver-setup-logs')
)
$ErrorActionPreference='Stop'
$ManagerPath=(Resolve-Path $ManagerPath).Path
if(!(Test-Path $ManagerPath -PathType Leaf)){throw 'Driver manager executable missing'}
if($ExpectedManagerSha256 -and (Get-FileHash $ManagerPath).Hash -ne $ExpectedManagerSha256){throw 'Driver manager checksum mismatch'}
if($Action -in 'Install','Rollback'){
 $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
 if(!([Security.Principal.WindowsPrincipal]$identity).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)){throw 'Run in an elevated PowerShell under the original signed-in user account'}
 if(Get-Process soundcurrent-studio,soundcurrent-eq -ErrorAction SilentlyContinue){throw 'Quit both SoundCurrent apps before setup or rollback; closing a window may leave the app running'}
}
New-Item -ItemType Directory -Force $LogDirectory|Out-Null
$log=Join-Path $LogDirectory ((Get-Date -Format 'yyyyMMdd-HHmmss-fff')+'-'+$Action+'.txt')
$arguments=switch($Action){
 'Status' {@('--status')}
 'Verify' {if(!$PackageDirectory){throw 'PackageDirectory required'};@('--verify',(Resolve-Path $PackageDirectory).Path)}
 'Install' {if(!$PackageDirectory){throw 'PackageDirectory required'};@('--install',$App,(Resolve-Path $PackageDirectory).Path)}
 'Rollback' {@('--remove',$App)}
}
& $ManagerPath @arguments 2>&1|Tee-Object $log
$code=$LASTEXITCODE
"Exit=$code"|Add-Content $log
if($Action -eq 'Status' -and $code -eq 10){Write-Output 'Native device not installed';return}
if($code -eq 3010){Write-Output 'Operation succeeded; Windows requests a restart. Save work and restart when ready.';return}
if($code -ne 0){throw "Driver manager failed ($code). Retain $log; do not bypass its trust checks."}
Write-Output "Completed $Action. Log: $log"
