# SPDX-License-Identifier: GPL-3.0-only
# Destructive native-driver qualification for an explicitly marked disposable VM.
[CmdletBinding()]
param(
    [ValidateSet('Plan','Preflight','Install','Runtime','Stress','PostResume','Uninstall','Final')]
    [string]$Phase = 'Plan',
    [switch]$Run,
    [ValidateSet('studio','eq','both')][string]$App = 'both',
    [string]$ManagerPath,
    [string]$PackageDirectory,
    [string]$BinaryDirectory,
    [string]$RepositoryRoot = (Split-Path $PSScriptRoot -Parent),
    [string]$ResultDirectory,
    [string]$DisposableVmApprovalFile,
    [ValidateRange(1,240)][int]$StressMinutes = 30,
    [switch]$EnableDriverVerifier,
    [switch]$RequireHvci,
    [switch]$AllowExistingDriver
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
$programData = if ($env:ProgramData) { $env:ProgramData } else { [IO.Path]::GetTempPath() }
if (!$ResultDirectory) { $ResultDirectory = Join-Path $programData 'SoundCurrent\NativeQualification' }
if (!$DisposableVmApprovalFile) { $DisposableVmApprovalFile = Join-Path $programData 'SoundCurrent\disposable-native-qualification.json' }
$driverHardwareId = 'ROOT\SOUNDCURRENTVAD'
$driverName = 'soundcurrentvad.sys'
$protectedNames = @('copperfin-clean-win11','copperfin-vfp9-win11','copperfin-access365-win11','soundcurrent-win11-dev')

function Write-Json([string]$Path, [object]$Value) {
    $parent = Split-Path $Path -Parent
    if ($parent) { New-Item -ItemType Directory -Force $parent | Out-Null }
    $Value | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $Path -Encoding UTF8
}

function Invoke-Native([string]$Name, [string]$File, [string[]]$Arguments, [int[]]$Expected = @(0)) {
    $log = Join-Path $ResultDirectory ($Name + '.log')
    $start = New-Object Diagnostics.ProcessStartInfo
    $start.FileName = $File
    $start.Arguments = ($Arguments | ForEach-Object { if ($_ -match '[\s"]') { '"' + $_.Replace('"','\"') + '"' } else { $_ } }) -join ' '
    $start.UseShellExecute = $false; $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true; $start.CreateNoWindow = $true
    $process = New-Object Diagnostics.Process; $process.StartInfo = $start
    try {
        if (!$process.Start()) { throw "Could not start $Name" }
        $stdout = $process.StandardOutput.ReadToEndAsync(); $stderr = $process.StandardError.ReadToEndAsync()
        if (!$process.WaitForExit(600000)) { $process.Kill(); $process.WaitForExit(); throw "$Name timed out" }
        ($stdout.Result + "`r`n" + $stderr.Result + "`r`nExit=" + $process.ExitCode) | Set-Content -LiteralPath $log -Encoding UTF8
        if ($process.ExitCode -notin $Expected) { throw "$Name failed with exit code $($process.ExitCode); see $log" }
    } finally { $process.Dispose() }
}

function Get-SecurityState {
    $secureBoot = try { [bool](Confirm-SecureBootUEFI) } catch { $null }
    $deviceGuard = @(Get-CimInstance Win32_DeviceGuard -Namespace root\Microsoft\Windows\DeviceGuard -ErrorAction SilentlyContinue)
    [ordered]@{
        computerName = $env:COMPUTERNAME; capturedUtc = [DateTime]::UtcNow.ToString('o')
        os = (Get-CimInstance Win32_OperatingSystem | Select-Object Caption,Version,BuildNumber)
        secureBoot = $secureBoot
        hvciRunning = [bool]($deviceGuard.SecurityServicesRunning -contains 2)
        verifier = (& verifier.exe /querysettings 2>&1 | Out-String)
        soundCurrentDevices = @(Get-PnpDevice -PresentOnly -ErrorAction SilentlyContinue |
            Where-Object InstanceId -like ($driverHardwareId + '*') | Select-Object InstanceId,Status,Class,FriendlyName)
    }
}

function Assert-DisposableVm {
    if (!$Run) { throw 'Mutating phases require -Run.' }
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    if (!([Security.Principal.WindowsPrincipal]$identity).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        throw 'Run from an elevated shell inside the disposable VM.'
    }
    $computer = Get-CimInstance Win32_ComputerSystem
    if (!$computer.HypervisorPresent -and $computer.Model -notmatch 'Virtual|QEMU|VMware|KVM|Hyper-V') { throw 'Qualification refuses non-virtual hardware.' }
    $name = $env:COMPUTERNAME.ToLowerInvariant()
    if ($protectedNames -contains $name -or $name -notmatch '(audit|qual|clone|test)') { throw "Qualification refuses protected or ambiguously named VM: $name" }
    if (!(Test-Path -LiteralPath $DisposableVmApprovalFile -PathType Leaf)) { throw "Missing disposable-VM approval marker: $DisposableVmApprovalFile" }
    $approval = Get-Content -LiteralPath $DisposableVmApprovalFile -Raw | ConvertFrom-Json
    if ($approval.purpose -ne 'soundcurrent-native-driver-qualification' -or $approval.computerName -ne $env:COMPUTERNAME -or !$approval.allowDriverChanges) {
        throw 'Disposable-VM approval marker does not authorize this machine.'
    }
    if ([DateTime]::Parse($approval.expiresUtc).ToUniversalTime() -le [DateTime]::UtcNow) { throw 'Disposable-VM approval marker has expired.' }
}

$plan = [ordered]@{
    schema = 1; purpose = 'SoundCurrent native-driver release qualification'
    phases = @('Preflight','Install','Runtime','Stress','PostResume','Uninstall','Final')
    safeguards = @(
        'Disposable virtual machine and time-limited approval marker required',
        'Known original, pristine, and development VMs refused',
        'No Secure Boot, test-signing, certificate-trust, or HVCI policy changes',
        'Driver Verifier targets only soundcurrentvad.sys and is reset during uninstall/finalization',
        'Each phase writes durable evidence before the next reboot or host-controlled suspend'
    )
    externalActions = @(
        'Create a full independent clone and checkpoint it while powered off',
        'Provide a separately signed package and manager; never provide a private signing key',
        'Reboot after Install when Driver Verifier is enabled',
        'Suspend and resume the disposable VM from the host between Runtime and PostResume',
        'Restore or delete only the disposable clone after evidence is copied'
    )
}
if ($Phase -eq 'Plan') { $plan | ConvertTo-Json -Depth 6; return }

Assert-DisposableVm
New-Item -ItemType Directory -Force $ResultDirectory | Out-Null
$statePath = Join-Path $ResultDirectory 'qualification-state.json'
$security = Get-SecurityState
Write-Json (Join-Path $ResultDirectory ("security-$($Phase.ToLowerInvariant()).json")) $security
if ($RequireHvci -and !$security.hvciRunning) { throw 'HVCI/Memory Integrity is required for this qualification profile.' }
if (Get-Process soundcurrent-studio,soundcurrent-eq -ErrorAction SilentlyContinue) { throw 'Quit all SoundCurrent applications before native-driver qualification.' }
if ($Phase -ne 'Preflight') {
    if (!(Test-Path -LiteralPath $statePath -PathType Leaf)) { throw 'Run Preflight first and retain its result directory.' }
    $savedState = Get-Content -LiteralPath $statePath -Raw | ConvertFrom-Json
    if ($savedState.computerName -ne $env:COMPUTERNAME) { throw 'Qualification state belongs to another VM.' }
}

$owners = if ($App -eq 'both') { @('studio','eq') } else { @($App) }
$manager = if ($ManagerPath) { (Resolve-Path -LiteralPath $ManagerPath).Path } else { $null }
$package = if ($PackageDirectory) { (Resolve-Path -LiteralPath $PackageDirectory).Path } else { $null }
$binaries = if ($BinaryDirectory) { (Resolve-Path -LiteralPath $BinaryDirectory).Path } else { $null }

switch ($Phase) {
    'Preflight' {
        if (Test-Path $statePath) { throw 'Use a new result directory for Preflight.' }
        if (!$AllowExistingDriver -and $security.soundCurrentDevices.Count) { throw 'SoundCurrent driver already exists; use a fresh clone.' }
        if (!$binaries) { throw 'Preflight requires -BinaryDirectory.' }
        $audioTool = Join-Path $binaries 'soundcurrent-audio-state.exe'
        if (!(Test-Path -LiteralPath $audioTool)) { throw 'Missing soundcurrent-audio-state.exe.' }
        Invoke-Native 'audio-before' $audioTool @('--snapshot',(Join-Path $ResultDirectory 'audio-before.json'))
        Write-Json $statePath ([ordered]@{schema=1;startedUtc=[DateTime]::UtcNow.ToString('o');computerName=$env:COMPUTERNAME;app=$App;phase='Preflight'})
    }
    'Install' {
        if (!$manager -or !$package) { throw 'Install requires -ManagerPath and -PackageDirectory.' }
        Invoke-Native 'package-verify' $manager @('--verify',$package)
        foreach ($owner in $owners) { Invoke-Native ("install-$owner") $manager @('--install',$owner,$package) @(0,3010) }
        if ($EnableDriverVerifier) {
            & verifier.exe /standard /driver $driverName | Set-Content -LiteralPath (Join-Path $ResultDirectory 'verifier-enable.log')
            if ($LASTEXITCODE -ne 0) { throw 'Driver Verifier configuration failed.' }
        }
        Write-Json $statePath ([ordered]@{schema=1;computerName=$env:COMPUTERNAME;app=$App;phase='Install';verifierEnabled=[bool]$EnableDriverVerifier;rebootRequired=[bool]$EnableDriverVerifier})
    }
    'Runtime' {
        if (!$binaries) { throw 'Runtime requires -BinaryDirectory.' }
        & powershell.exe -NoProfile -NonInteractive -File (Join-Path $RepositoryRoot 'tests\windows_native_integration.ps1') -Run -BinaryDirectory $binaries -ResultDirectory (Join-Path $ResultDirectory 'runtime')
        if ($LASTEXITCODE -ne 0) { throw 'Native integration suite failed.' }
    }
    'Stress' {
        if (!$binaries) { throw 'Stress requires -BinaryDirectory.' }
        $smoke = Join-Path $binaries 'soundcurrent-windows-audio-smoke.exe'
        if (!(Test-Path -LiteralPath $smoke)) { throw 'Missing native audio smoke binary.' }
        $deadline = [DateTime]::UtcNow.AddMinutes($StressMinutes); $iteration = 0
        while ([DateTime]::UtcNow -lt $deadline) {
            $iteration++; Invoke-Native ("stress-{0:D4}" -f $iteration) $smoke @('--run-native')
            Invoke-Native ("stress-mic-{0:D4}" -f $iteration) $smoke @('--run-native-mic')
        }
        Write-Json (Join-Path $ResultDirectory 'stress-summary.json') ([ordered]@{minutes=$StressMinutes;iterations=$iteration;completedUtc=[DateTime]::UtcNow.ToString('o')})
    }
    'PostResume' {
        if (!$binaries) { throw 'PostResume requires -BinaryDirectory.' }
        & powershell.exe -NoProfile -NonInteractive -File (Join-Path $RepositoryRoot 'tests\windows_native_integration.ps1') -Run -BinaryDirectory $binaries -ResultDirectory (Join-Path $ResultDirectory 'post-resume')
        if ($LASTEXITCODE -ne 0) { throw 'Post-resume integration suite failed.' }
    }
    'Uninstall' {
        if (!$manager) { throw 'Uninstall requires -ManagerPath.' }
        [array]::Reverse($owners)
        foreach ($owner in $owners) { Invoke-Native ("remove-$owner") $manager @('--remove',$owner) @(0,3010) }
        & verifier.exe /reset | Set-Content -LiteralPath (Join-Path $ResultDirectory 'verifier-reset.log')
        if ($LASTEXITCODE -ne 0) { throw 'Driver Verifier reset failed.' }
    }
    'Final' {
        if (!$binaries) { throw 'Final requires -BinaryDirectory.' }
        $audioTool = Join-Path $binaries 'soundcurrent-audio-state.exe'; $before = Join-Path $ResultDirectory 'audio-before.json'
        if (!(Test-Path -LiteralPath $before)) { throw 'Missing Preflight audio snapshot.' }
        Invoke-Native 'audio-final-compare' $audioTool @('--compare',$before)
        $final = Get-SecurityState
        if ($final.soundCurrentDevices.Count) { throw 'SoundCurrent device remains after uninstall.' }
        if ($final.verifier -match [regex]::Escape($driverName)) { throw 'SoundCurrent remains selected in Driver Verifier.' }
        Write-Json (Join-Path $ResultDirectory 'qualification-result.json') ([ordered]@{schema=1;result='passed';completedUtc=[DateTime]::UtcNow.ToString('o');computerName=$env:COMPUTERNAME;security=$final})
    }
}
Write-Output "PASS: $Phase evidence saved under $ResultDirectory"
