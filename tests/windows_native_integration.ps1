# SPDX-License-Identifier: GPL-3.0-only
# Opt-in test on an independent Windows worker with a trusted test driver.
[CmdletBinding()]
param([switch]$Run,[Parameter(Mandatory=$true)][string]$BinaryDirectory,
      [Parameter(Mandatory=$true)][string]$ResultDirectory)
$ErrorActionPreference='Stop'
if(!$Run){throw 'Use -Run only on an independent authorized test worker; tests change audio defaults and levels'}
$BinaryDirectory=(Resolve-Path $BinaryDirectory).Path
if(Test-Path $ResultDirectory){throw 'Use a new result directory'}
if(Get-Process soundcurrent-studio,soundcurrent-eq -ErrorAction SilentlyContinue){throw 'Quit active SoundCurrent apps before tests'}
$required=@('soundcurrent-audio-state.exe','soundcurrent-native-format-probe.exe','soundcurrent-endpoint-identity-test.exe','soundcurrent-windows-audio-smoke.exe','soundcurrent-studio.exe','soundcurrent-windows-volume-smoke.exe','soundcurrent-route-guardian.exe')
foreach($name in $required){if(!(Test-Path (Join-Path $BinaryDirectory $name))){throw "Missing test binary: $name"}}
New-Item -ItemType Directory $ResultDirectory|Out-Null
$ResultDirectory=(Resolve-Path $ResultDirectory).Path
function Invoke-Test([string]$Name,[string]$Executable,[string]$Arguments,[int]$Timeout=120000){
 $start=New-Object Diagnostics.ProcessStartInfo
 $start.FileName=Join-Path $BinaryDirectory $Executable;$start.Arguments=$Arguments
 $start.UseShellExecute=$false;$start.RedirectStandardOutput=$true;$start.RedirectStandardError=$true;$start.CreateNoWindow=$true
 $process=New-Object Diagnostics.Process;$process.StartInfo=$start
 try{
  if(!$process.Start()){throw "Launch failed: $Name"};$process.PriorityClass='BelowNormal'
  $stdout=$process.StandardOutput.ReadToEndAsync();$stderr=$process.StandardError.ReadToEndAsync()
  if(!$process.WaitForExit($Timeout)){$process.Kill();$process.WaitForExit();throw "Timed out: $Name"}
  $text=$stdout.Result+"`r`n"+$stderr.Result+"`r`nExit="+$process.ExitCode
  $text|Set-Content -Encoding UTF8 (Join-Path $ResultDirectory ($Name+'.txt'))
  if($process.ExitCode -ne 0){throw "Failed: $Name ($($process.ExitCode))"}
  if($Name -eq 'studio-live-ui' -and $text -notmatch 'PASS: live UI enable'){throw 'GUI completion marker missing'}
 }finally{$process.Dispose()}
}
$oldPlatform=$env:QT_QPA_PLATFORM;$oldPath=$env:PATH
$env:QT_QPA_PLATFORM='offscreen';$env:PATH=$BinaryDirectory+';'+$env:PATH
$journal=Join-Path $ResultDirectory 'audio-before.json';$captured=$false;$success=$false
try{
 Invoke-Test 'snapshot' 'soundcurrent-audio-state.exe' ('--snapshot "'+$journal+'"');$captured=$true
 $state=Get-Content $journal -Raw|ConvertFrom-Json
 foreach($role in 'playback','feed','capture'){
  $endpoint=@($state.levels|Where-Object nativeRole -eq $role)
  if($endpoint.Count -ne 1){throw "Expected one native $role endpoint"}
  $flow=if($role -eq 'capture'){'capture'}else{'render'}
  Invoke-Test ('format-'+$role) 'soundcurrent-native-format-probe.exe' ($flow+' "'+$endpoint[0].id+'"')
 }
 Invoke-Test 'identity' 'soundcurrent-endpoint-identity-test.exe' ''
 Invoke-Test 'native-transport' 'soundcurrent-windows-audio-smoke.exe' '--run-native'
 Invoke-Test 'native-microphone' 'soundcurrent-windows-audio-smoke.exe' '--run-native-mic'
 Invoke-Test 'studio-live-ui' 'soundcurrent-studio.exe' '--windows-live-ui-test'
 Invoke-Test 'studio-manual-route' 'soundcurrent-studio.exe' '--windows-live-manual-route-test'
 Invoke-Test 'volume-recovery' 'soundcurrent-windows-volume-smoke.exe' '--run-native'
 $success=$true
}catch{$_|Out-String|Set-Content -Encoding UTF8 (Join-Path $ResultDirectory 'failure.txt');throw}
finally{
 try{
  if($captured){
   Invoke-Test 'restore' 'soundcurrent-audio-state.exe' ('--restore "'+$journal+'"')
   Invoke-Test 'compare' 'soundcurrent-audio-state.exe' ('--compare "'+$journal+'"')
   Invoke-Test 'after' 'soundcurrent-audio-state.exe' ('--snapshot "'+(Join-Path $ResultDirectory 'audio-after.json')+'"')
  }
 }finally{$env:QT_QPA_PLATFORM=$oldPlatform;$env:PATH=$oldPath}
}
if($success){'ALL INTEGRATION TESTS PASSED WITH RESTORED AUDIO STATE'|Set-Content -Encoding UTF8 (Join-Path $ResultDirectory 'result.txt')}
