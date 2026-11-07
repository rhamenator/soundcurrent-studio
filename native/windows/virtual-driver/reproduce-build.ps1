# SPDX-License-Identifier: GPL-3.0-only
# Build/validate unsigned inputs only. Never installs or changes Windows trust.
[CmdletBinding()]
param(
 [string]$SourceRoot=(Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path,
 [Parameter(Mandatory=$true)][string]$OutputDirectory,
 [string]$ExpectedRevision,
 [string]$PackageCache=(Join-Path $env:LOCALAPPDATA 'SoundCurrent\driver-build-cache')
)
$ErrorActionPreference='Stop'
$ProgressPreference='SilentlyContinue'
if([Environment]::OSVersion.Platform -ne 'Win32NT' -or ![Environment]::Is64BitOperatingSystem){throw 'Run on native Windows x64, including a Windows worker hosted by Linux'}
if(Test-Path $OutputDirectory){throw 'Choose a new output directory; existing evidence is never overwritten'}
$SourceRoot=(Resolve-Path $SourceRoot).Path
$revision=(& git -C $SourceRoot rev-parse HEAD).Trim()
if($LASTEXITCODE -ne 0){throw 'A Git checkout is required'}
if($ExpectedRevision -and $revision -ne $ExpectedRevision){throw 'Source revision mismatch'}
if(& git -C $SourceRoot status --porcelain){throw 'Use a clean source checkout and keep output outside it'}
$vendor=Join-Path $SourceRoot 'native\windows\virtual-driver\vendor'
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if(!(Test-Path $vswhere)){throw 'Install VS 2026 Desktop C++ and Windows Driver Kit component first'}
$vs=(& $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -property installationPath|Select-Object -First 1)
$msbuild=Join-Path $vs 'MSBuild\Current\Bin\amd64\MSBuild.exe'
if(!(Test-Path $msbuild)){throw 'MSBuild x64 missing'}
$toolset=Get-ChildItem (Join-Path $vs 'MSBuild\Microsoft\VC') -Filter Toolset.props -Recurse | Where-Object FullName -Match 'x64\\PlatformToolsets\\WindowsKernelModeDriver10.0\\'
if(!$toolset){throw 'Install the global WDK and VS Windows Driver Kit component; NuGet packages alone are insufficient'}
Get-Command dotnet -ErrorAction Stop|Out-Null
New-Item -ItemType Directory -Path $OutputDirectory|Out-Null
$OutputDirectory=(Resolve-Path $OutputDirectory).Path
New-Item -ItemType Directory -Path "$OutputDirectory\logs","$OutputDirectory\unsigned-driver"|Out-Null
New-Item -ItemType Directory -Force $PackageCache|Out-Null
$priority=(Get-Process -Id $PID).PriorityClass
$oldPath=$env:PATH
Start-Transcript -Path "$OutputDirectory\logs\build.txt"
try{
 (Get-Process -Id $PID).PriorityClass='BelowNormal'
 Add-Type -AssemblyName System.IO.Compression.FileSystem
 $version='10.0.28000.2526'
 $packageHashes=@()
 foreach($id in 'Microsoft.Windows.WDK.x64','Microsoft.Windows.SDK.CPP','Microsoft.Windows.SDK.CPP.x64'){
  $pkg=Join-Path $PackageCache "$id.$version.nupkg"
  if(!(Test-Path $pkg)){Invoke-WebRequest "https://api.nuget.org/v3-flatcontainer/$($id.ToLowerInvariant())/$version/$($id.ToLowerInvariant()).$version.nupkg" -OutFile $pkg}
  & dotnet nuget verify $pkg --all --verbosity quiet
  if($LASTEXITCODE -ne 0){throw "NuGet signature verification failed: $id"}
  $dest=Join-Path $vendor "packages\$id.$version"
  # Verify the archive every time; refresh restored contents from that archive.
  New-Item -ItemType Directory -Force $dest|Out-Null
  $archive=[IO.Compression.ZipFile]::OpenRead($pkg)
  try{
   foreach($entry in $archive.Entries){
    $path=[IO.Path]::GetFullPath((Join-Path $dest $entry.FullName))
    if(!$path.StartsWith($dest.TrimEnd('\')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Package entry outside restore directory'}
    if(!$entry.Name){New-Item -ItemType Directory -Force $path|Out-Null;continue}
    New-Item -ItemType Directory -Force ([IO.Path]::GetDirectoryName($path))|Out-Null
    [IO.Compression.ZipFileExtensions]::ExtractToFile($entry,$path,$true)
   }
  }finally{$archive.Dispose()}
  $packageHashes+=[ordered]@{id=$id;version=$version;sha256=(Get-FileHash $pkg).Hash}
 }
 $wdk=Join-Path $vendor "packages\Microsoft.Windows.WDK.x64.$version\c"
 $env:PATH=(Join-Path $wdk 'tools\10.0.28000.0\x64')+';'+$env:PATH
 foreach($project in 'EndpointsCommon\EndpointsCommon.vcxproj','TabletAudioSample\TabletAudioSample.vcxproj'){
  & $msbuild (Join-Path $vendor "sysvad\$project") /t:Rebuild /m:1 /p:Platform=x64 /p:Configuration=Release /p:WindowsTargetPlatformVersion=10.0.28000.0 /p:SignMode=Off /p:BuildInParallel=false /p:CL_MPCount=1 /p:UseMultiToolTask=false /verbosity:minimal
  if($LASTEXITCODE -ne 0){throw "Build failed: $project"}
 }
 $package="$OutputDirectory\unsigned-driver"
 $built=Join-Path $vendor 'sysvad\TabletAudioSample\x64\Release'
 Copy-Item "$built\soundcurrentvad.inf","$built\soundcurrentvad.sys" $package
 & "$wdk\tools\10.0.28000.0\x64\infverif.exe" /u "$package\soundcurrentvad.inf" *> "$OutputDirectory\logs\infverif.txt"
 if($LASTEXITCODE -ne 0){throw 'InfVerif failed'}
 & "$wdk\bin\10.0.28000.0\x64\apivalidator.exe" "-BinaryPath:$package\soundcurrentvad.sys" "-SupportedApiXmlFiles:$wdk\build\10.0.28000.0\universalDDIs\x64\UniversalDDIs.xml" -StrictCompliance:true *> "$OutputDirectory\logs\apivalidator.txt"
 if($LASTEXITCODE -ne 0){throw 'Universal API validation failed'}
 & "$wdk\bin\10.0.28000.0\x86\Inf2Cat.exe" "/driver:$package" /os:10_NI_X64,10_GE_X64,10_25H2_X64 /uselocaltime *> "$OutputDirectory\logs\inf2cat.txt"
 if($LASTEXITCODE -ne 0){throw 'Inf2Cat failed'}
 & git -C $SourceRoot archive --format=zip "--output=$OutputDirectory\source.zip" $revision
 if($LASTEXITCODE -ne 0){throw 'Source archive failed'}
 Get-ChildItem $package -File|Get-FileHash|Select-Object @{n='File';e={Split-Path $_.Path -Leaf}},Hash|ConvertTo-Json|Set-Content "$OutputDirectory\unsigned-driver-sha256.json"
 [ordered]@{sourceCommit=$revision;configuration='Release';architecture='x64';packages=$packageHashes;msbuild=$msbuild;msbuildVersion=(& $msbuild -version -nologo|Out-String).Trim();dotnet=(& dotnet --version|Out-String).Trim();builtAtUtc=(Get-Date).ToUniversalTime().ToString('o');runtimeTested=$false;signing='unsigned';reproducibility='Same source/toolchain and validations; timestamps/signatures are not byte-for-byte reproducible'}|ConvertTo-Json -Depth 5|Set-Content "$OutputDirectory\manifest.json"
 'BUILD AND VALIDATION PASSED; unsigned, not installed or runtime-tested.'|Set-Content "$OutputDirectory\result.txt"
}finally{
 $env:PATH=$oldPath
 (Get-Process -Id $PID).PriorityClass=$priority
 Stop-Transcript
}
