# SPDX-License-Identifier: GPL-3.0-only
# Use only on an explicitly configured Windows test worker. Does not import trust.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$BuildDirectory,
 [Parameter(Mandatory=$true)][string]$OutputDirectory,
 [Parameter(Mandatory=$true)][ValidatePattern('^[A-Fa-f0-9]{40}$')][string]$CertificateThumbprint,
 [Parameter(Mandatory=$true)][string]$SignToolPath,
 [Parameter(Mandatory=$true)][string]$Inf2CatPath
)
$ErrorActionPreference='Stop'
if(Test-Path $OutputDirectory){throw 'Choose a new output directory'}
$BuildDirectory=(Resolve-Path $BuildDirectory).Path
$SignToolPath=(Resolve-Path $SignToolPath).Path
$Inf2CatPath=(Resolve-Path $Inf2CatPath).Path
$certificate=Get-Item "Cert:\CurrentUser\My\$CertificateThumbprint"
if(!$certificate.HasPrivateKey){throw 'Signing certificate has no local private key'}
. "$PSScriptRoot\driver-inputs.ps1"
$null=@(Get-VerifiedUnsignedDriverInputs -BuildDirectory $BuildDirectory)
New-Item -ItemType Directory -Path $OutputDirectory|Out-Null
$OutputDirectory=(Resolve-Path $OutputDirectory).Path
New-Item -ItemType Directory "$OutputDirectory\signed-driver","$OutputDirectory\logs"|Out-Null
$package="$OutputDirectory\signed-driver"
Copy-VerifiedUnsignedDriverInputs -BuildDirectory $BuildDirectory -DestinationDirectory $package
Start-Transcript "$OutputDirectory\logs\test-sign.txt"
try{
 & $SignToolPath sign /v /fd SHA256 /s My /sha1 $CertificateThumbprint "$package\soundcurrentvad.sys"
 if($LASTEXITCODE -ne 0){throw 'SYS signing failed'}
 & $Inf2CatPath "/driver:$package" /os:10_NI_X64,10_GE_X64,10_25H2_X64 /uselocaltime
 if($LASTEXITCODE -ne 0){throw 'Catalog regeneration failed'}
 & $SignToolPath sign /v /fd SHA256 /s My /sha1 $CertificateThumbprint "$package\soundcurrentvad.cat"
 if($LASTEXITCODE -ne 0){throw 'Catalog signing failed'}
 foreach($file in 'soundcurrentvad.sys','soundcurrentvad.inf'){
  & $SignToolPath verify /pa /v /c "$package\soundcurrentvad.cat" (Join-Path $package $file)
  if($LASTEXITCODE -ne 0){throw 'Signature/catalog validation failed; check explicitly configured test-worker trust, do not bypass verification'}
 }
 Export-Certificate -Cert $certificate -FilePath "$OutputDirectory\public-test-certificate.cer"|Out-Null
 Get-ChildItem $package -File|Get-FileHash|Select-Object @{n='File';e={Split-Path $_.Path -Leaf}},Hash|ConvertTo-Json|Set-Content "$OutputDirectory\signed-driver-sha256.json"
 $manifest=Get-Content "$BuildDirectory\manifest.json" -Raw|ConvertFrom-Json
 $manifest.signing='local test only'
 $manifest|Add-Member testCertificateThumbprint $CertificateThumbprint
 $manifest.runtimeTested=$false
 $manifest|ConvertTo-Json -Depth 6|Set-Content "$OutputDirectory\manifest.json"
}finally{Stop-Transcript}
