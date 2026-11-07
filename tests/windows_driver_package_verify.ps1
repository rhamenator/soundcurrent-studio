# SPDX-License-Identifier: GPL-3.0-only
# Opt-in read-only manager checks. No installation, trust, or boot changes.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$Manager,
 [Parameter(Mandatory=$true)][ValidatePattern('^[a-fA-F0-9]{64}$')][string]$ManagerSHA256,
 [Parameter(Mandatory=$true)][string]$PackageDirectory,
 [Parameter(Mandatory=$true)][string]$ResultDirectory
)
$ErrorActionPreference='Stop'
$Manager=(Resolve-Path $Manager).Path
$PackageDirectory=(Resolve-Path $PackageDirectory).Path
if((Get-FileHash $Manager -Algorithm SHA256).Hash -ne $ManagerSHA256){throw 'Unexpected manager binary'}
if(Test-Path $ResultDirectory){throw 'Choose a new result directory'}
New-Item -ItemType Directory $ResultDirectory|Out-Null
$ResultDirectory=(Resolve-Path $ResultDirectory).Path
function Check-Package([string]$Directory,[string]$Name,[bool]$ExpectedValid){
 # ProcessStartInfo keeps stderr as evidence, without PowerShell 5.1 turning it
 # into a terminating error before the exit status can be inspected.
 $info=New-Object Diagnostics.ProcessStartInfo
 $info.FileName=$Manager
 $info.Arguments='--verify "'+$Directory+'"'
 $info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $info.RedirectStandardOutput=$true;$info.RedirectStandardError=$true
 $process=New-Object Diagnostics.Process;$process.StartInfo=$info
 if(!$process.Start()){throw 'Manager did not start'}
 try{
  $stdout=$process.StandardOutput.ReadToEndAsync();$stderr=$process.StandardError.ReadToEndAsync()
  if(!$process.WaitForExit(60000)){$process.Kill();$process.WaitForExit();throw 'Manager verification timed out'}
  [IO.File]::WriteAllText((Join-Path $ResultDirectory ($Name+'-stdout.txt')),$stdout.Result)
  [IO.File]::WriteAllText((Join-Path $ResultDirectory ($Name+'-stderr.txt')),$stderr.Result)
  $exit=$process.ExitCode
  if(($exit -eq 0) -ne $ExpectedValid){throw ('Unexpected package verification result: '+$Name+' exit '+$exit)}
  [ordered]@{name=$Name;exitCode=$exit;expectedValid=$ExpectedValid}
 }finally{$process.Dispose()}
}
$results=@()
$results+=Check-Package $PackageDirectory 'valid' $true
foreach($member in 'soundcurrentvad.inf','soundcurrentvad.sys','soundcurrentvad.cat'){
 $directory=Join-Path $ResultDirectory ('altered-'+$member)
 New-Item -ItemType Directory $directory|Out-Null
 foreach($file in 'soundcurrentvad.inf','soundcurrentvad.sys','soundcurrentvad.cat'){
  Copy-Item (Join-Path $PackageDirectory $file) $directory
  (Get-Item (Join-Path $directory $file)).IsReadOnly=$false
 }
 $path=Join-Path $directory $member
 $bytes=[IO.File]::ReadAllBytes($path)
 if(!$bytes.Length){throw 'Empty package member'}
 $at=[Math]::Min(5000,$bytes.Length-1);$bytes[$at]=$bytes[$at] -bxor 1
 [IO.File]::WriteAllBytes($path,$bytes)
 $results+=Check-Package $directory $member $false
}
$results|ConvertTo-Json|Set-Content -Encoding UTF8 (Join-Path $ResultDirectory 'results.json')
'PASS: trusted package accepted; altered INF, SYS and CAT rejected'|Set-Content (Join-Path $ResultDirectory 'result.txt')
