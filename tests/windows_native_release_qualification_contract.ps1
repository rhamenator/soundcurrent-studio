# SPDX-License-Identifier: GPL-3.0-only
$ErrorActionPreference = 'Stop'
$script = Join-Path $PSScriptRoot 'windows_native_release_qualification.ps1'
$tokens = $null; $errors = $null
[Management.Automation.Language.Parser]::ParseFile($script,[ref]$tokens,[ref]$errors) | Out-Null
if ($errors.Count) { throw ($errors | Out-String) }
$text = Get-Content -LiteralPath $script -Raw
if ($text -match '\[string\]\$RepositoryRoot\s*=') { throw 'RepositoryRoot must not evaluate PSScriptRoot inside the parameter default on Windows PowerShell 5.1.' }
foreach ($required in @(
    'Assert-DisposableVm','copperfin-clean-win11','soundcurrent-win11-dev','HypervisorPresent','allowDriverChanges',
    "verifier.exe /standard /driver `$driverName",'verifier.exe /reset','Confirm-SecureBootUEFI',
    'windows_native_integration.ps1','audio-final-compare'
)) { if (!$text.Contains($required)) { throw "Qualification safety contract missing: $required" } }
foreach ($forbidden in @('bcdedit /set','TESTSIGNING ON','Set-ExecutionPolicy','Import-Certificate','Disable-WindowsOptionalFeature')) {
    if ($text -match [regex]::Escape($forbidden)) { throw "Qualification runner must not weaken host security: $forbidden" }
}
$plan = & $script -Phase Plan | ConvertFrom-Json
if ($plan.schema -ne 1 -or $plan.phases.Count -ne 7 -or $plan.safeguards.Count -lt 5) { throw 'Qualification plan is incomplete.' }
Write-Output 'PASS: qualification script parses; disposable-VM, exact-driver, rollback, security and phase contracts retained'
