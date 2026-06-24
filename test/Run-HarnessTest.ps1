<#
.SYNOPSIS
    Install/uninstall test matrix for the OPC merge-module harness MSIs.
    RUN ELEVATED (Run as administrator) on a clean VM.

.DESCRIPTION
    Drives the harness built by Build-MsmHarness.ps1 through several feature
    selections, checking each install/uninstall for:
      - msiexec success (exit 0)
      - no error 2753 in the verbose log (the custom-action regression)
      - expected components present after install / absent after uninstall

    Scenarios:
      1. Nothing optional  - installs the product with NO components.
      2. Everything        - ADDLOCAL=ProxyStub,Sdk; verifies registration.
      3. SDK only          - ADDLOCAL=Sdk (proxy/stub feature OFF); this is the
                             exact configuration that produced error 2753.

    NOTE: this harness is a NATIVE per-arch MSI, so it does NOT reproduce the
    WIN64DUALFOLDERS SysWOW64 file leak of the production combined-x64 installer -
    a fully clean uninstall is expected here.

.PARAMETER Platform
    x86 or x64 (default: x64).
#>
[CmdletBinding()]
param([ValidateSet('x86','x64')][string]$Platform = 'x64')

$ErrorActionPreference = 'Stop'
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$Msi    = Join-Path $ScriptDir "out\opc-msm-harness-$Platform.msi"
$LogDir = Join-Path $ScriptDir "logs-$Platform"

if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Run this script from an ELEVATED PowerShell (Run as administrator).'
}
if (-not (Test-Path $Msi)) { throw "Harness MSI not found: $Msi. Build it first: .\Build-MsmHarness.ps1 -Platform $Platform" }
New-Item -ItemType Directory -Path $LogDir -Force | Out-Null

# Expected component (core proxy/stub DLL) and service per arch.
if ($Platform -eq 'x86') {
    $coreDll = 'C:\Windows\SysWOW64\opccomn_ps.dll'
    $svcName = 'OpcEnum'
} else {
    $coreDll = Join-Path $env:CommonProgramFiles 'OPC Foundation\Bin\opccomn_ps.dll'
    $svcName = $null   # x64 uses OpcCategoryManager (COM /RegServer), not a service
}

function Invoke-Msi { param([string]$Args,[string]$Log)
    $p = Start-Process msiexec.exe -ArgumentList "$Args /qn /l*v `"$Log`"" -Wait -PassThru
    return $p.ExitCode
}
function Has2753($Log){ [bool](Select-String -Path $Log -Pattern '2753' -SimpleMatch -Quiet) }
function SvcPresent { if ($svcName) { [bool](Get-Service $svcName -ErrorAction SilentlyContinue) } else { $false } }
function Pass($m){ Write-Host "  PASS: $m" -ForegroundColor Green; $true }
function Fail($m){ Write-Host "  FAIL: $m" -ForegroundColor Red;   $false }

$results = @()

function Test-Scenario {
    param([string]$Name,[string]$AddLocal,[bool]$ExpectComponents)
    Write-Host "`n--- $Name ---"
    $safe = ($Name -replace '[^A-Za-z0-9]','-')
    $instLog = Join-Path $LogDir "$safe-install.log"
    $uninLog = Join-Path $LogDir "$safe-uninstall.log"

    $addArg = if ($AddLocal) { "ADDLOCAL=$AddLocal" } else { '' }
    $rc = Invoke-Msi "/i `"$Msi`" $addArg" $instLog
    $ok = $true
    if ($rc -ne 0)        { $ok = (Fail "install exit=$rc (expected 0)") -and $ok }
    if (Has2753 $instLog) { $ok = (Fail "error 2753 in install log") -and $ok }
    if ($ExpectComponents) {
        if (-not (Test-Path $coreDll)) { $ok = (Fail "expected $coreDll present after install") -and $ok }
        if ($svcName -and -not (SvcPresent)) { $ok = (Fail "expected $svcName service present") -and $ok }
    }
    if ($ok) { Pass "install clean (exit 0, no 2753)" | Out-Null }

    # Uninstall and confirm clean
    $rc = Invoke-Msi "/x `"$Msi`"" $uninLog
    if ($rc -ne 0)              { $ok = (Fail "uninstall exit=$rc") -and $ok }
    if (Test-Path $coreDll)     { $ok = (Fail "$coreDll left behind after uninstall") -and $ok }
    if ($svcName -and (SvcPresent)) { $ok = (Fail "$svcName service left behind") -and $ok }
    if ($rc -eq 0 -and -not (Test-Path $coreDll) -and -not ($svcName -and (SvcPresent))) {
        Pass "uninstall clean (no leftover files/service)" | Out-Null
    }
    $script:results += [pscustomobject]@{ Scenario = $Name; Ok = $ok }
}

Write-Host "Harness: $Msi"
Test-Scenario -Name 'Nothing optional' -AddLocal ''               -ExpectComponents:$false
Test-Scenario -Name 'Everything'       -AddLocal 'ProxyStub,Sdk'  -ExpectComponents:$true
Test-Scenario -Name 'SDK only (2753 regression)' -AddLocal 'Sdk'  -ExpectComponents:$false

Write-Host "`n==================== SUMMARY ($Platform) ===================="
$results | ForEach-Object { Write-Host ("  {0,-32} {1}" -f $_.Scenario, $(if($_.Ok){'PASS'}else{'FAIL'})) -ForegroundColor $(if($_.Ok){'Green'}else{'Red'}) }
if ($results.Ok -contains $false) { Write-Host "RESULT: FAIL" -ForegroundColor Red; exit 1 }
else { Write-Host "RESULT: ALL PASS" -ForegroundColor Green }
Write-Host "Logs: $LogDir"
