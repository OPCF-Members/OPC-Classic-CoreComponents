<#
.SYNOPSIS
    Builds the OPC merge modules from the current WiX source and wraps each in a
    test-harness MSI (test\out\opc-msm-harness-<arch>.msi).

.DESCRIPTION
    Use this to validate the merge modules in isolation on a clean VM. It does
    NOT recompile the DLLs - it reuses the binaries already produced by build.ps1
    in out\<arch>\bin and out\<arch>\include, then rebuilds the .msm from the
    current WiX\*.wxs (so it always reflects the latest source) and packages the
    harness MSI around them.

    Run build.ps1 (at the repo root) at least once first so the binaries exist.

.PARAMETER Platform
    x86, x64, or both (default: both).

.PARAMETER WixCmd
    Path to the wix executable (default: wix, or $env:WIX_CMD).

.EXAMPLE
    .\test\Build-MsmHarness.ps1
    .\test\Build-MsmHarness.ps1 -Platform x64
#>
[CmdletBinding()]
param(
    [ValidateSet('x86','x64','both')]
    [string]$Platform = 'both',
    [string]$WixCmd
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$Root      = Split-Path -Parent $ScriptDir
$WixDir    = Join-Path $Root 'WiX'
$OutDir    = Join-Path $ScriptDir 'out'
if (-not $WixCmd) { $WixCmd = if ($env:WIX_CMD) { $env:WIX_CMD } else { 'wix' } }

# Version from version.txt (MAJOR.MINOR.REVISION.0) - 4th field is irrelevant here.
$Major = $Minor = $Revision = $null
foreach ($line in (Get-Content (Join-Path $Root 'version.txt'))) {
    if ($line -match '^\s*MAJOR\s*=\s*(\d+)')    { $Major    = [int]$Matches[1] }
    if ($line -match '^\s*MINOR\s*=\s*(\d+)')    { $Minor    = [int]$Matches[1] }
    if ($line -match '^\s*REVISION\s*=\s*(\d+)') { $Revision = [int]$Matches[1] }
}
if ($null -eq $Major) { $Major = 3; $Minor = 1; $Revision = 2 }
$Version = "$Major.$Minor.$Revision.0"

# Per-arch identifiers. Module/SDK GUIDs MUST match build.ps1 so the harness
# tests the real modules. Harness UpgradeCodes are test-only and arbitrary.
$cfg = @{
    x86 = @{
        ModuleGuid  = 'E25DF0F2-0F29-47E8-8537-265285ED9757'
        SdkGuid     = 'E25DF0F3-0F29-47E8-8537-265285ED9758'
        UpgradeCode = 'C3D4E5F6-A7B8-49AA-CDEF-34567890ABCD'
    }
    x64 = @{
        ModuleGuid  = 'E68AB89F-6DF7-4750-8BC1-F14B2064F313'
        SdkGuid     = 'E68AB8A0-6DF7-4750-8BC1-F14B2064F314'
        UpgradeCode = 'D4E5F6A7-B8C9-49BB-DEF0-4567890ABCDE'
    }
}

function Build-One {
    param([string]$Arch)

    $BinDir     = Join-Path $Root "out\$Arch\bin"
    $IncludeDir = Join-Path $Root "out\$Arch\include"
    if (-not (Test-Path (Join-Path $BinDir 'opccomn_ps.dll'))) {
        throw "Binaries not found in $BinDir. Run .\build.ps1 -Platform $Arch (at the repo root) first."
    }
    if (-not (Test-Path $IncludeDir)) {
        throw "SDK include files not found in $IncludeDir. Run .\build.ps1 -Platform $Arch first."
    }

    New-Item -ItemType Directory -Path $OutDir -Force | Out-Null
    $c = $cfg[$Arch]
    $tag = "($Arch)"
    $proxyMsm = Join-Path $OutDir "harness-proxystub-$Arch.msm"
    $sdkMsm   = Join-Path $OutDir "harness-sdk-$Arch.msm"
    $harness  = Join-Path $OutDir "opc-msm-harness-$Arch.msi"

    Write-Host "`n=== $Arch : building merge modules from current source ==="
    & $WixCmd build (Join-Path $WixDir 'MergeModule.wxs') -arch $Arch `
        -d "BinDir=$BinDir" -d "ModuleGuid=$($c.ModuleGuid)" `
        -d "PlatformTag=$tag" -d "Platform=$Arch" -d "Version=$Version" `
        -bindpath $WixDir -o $proxyMsm
    if ($LASTEXITCODE -ne 0) { throw "Proxy/stub merge module build failed ($Arch)." }

    & $WixCmd build (Join-Path $WixDir 'MergeModuleSdk.wxs') -arch $Arch `
        -d "IncludeDir=$IncludeDir" -d "SdkGuid=$($c.SdkGuid)" `
        -d "PlatformTag=$tag" -d "Platform=$Arch" -d "Version=$Version" `
        -bindpath $WixDir -o $sdkMsm
    if ($LASTEXITCODE -ne 0) { throw "SDK merge module build failed ($Arch)." }

    Write-Host "=== $Arch : building harness MSI ==="
    & $WixCmd build (Join-Path $ScriptDir 'MsmHarness.wxs') -arch $Arch `
        -d "Platform=$Arch" -d "Version=$Version" -d "UpgradeCode=$($c.UpgradeCode)" `
        -d "ProxyStubMsm=$proxyMsm" -d "SdkMsm=$sdkMsm" `
        -o $harness
    if ($LASTEXITCODE -ne 0) { throw "Harness MSI build failed ($Arch)." }

    Write-Host "  Built: $harness" -ForegroundColor Green
}

# Verify wix is available
try { & $WixCmd --version | Out-Null; if ($LASTEXITCODE -ne 0) { throw } }
catch { throw "WiX not found. Install with: dotnet tool install --global wix (or set -WixCmd)." }

$arches = if ($Platform -eq 'both') { @('x86','x64') } else { @($Platform) }
foreach ($a in $arches) { Build-One $a }

Write-Host "`nDone. Harness MSIs in: $OutDir" -ForegroundColor Cyan
Write-Host "Copy the test\ folder to a clean VM and run Run-HarnessTest.ps1 elevated."
