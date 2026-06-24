<#
.SYNOPSIS
    OPC Core Components - Build Script

.DESCRIPTION
    Builds the 8 OPC COM proxy/stub DLLs and packages them into
    merge modules (.msm) and MSI installers (.msi).

.PARAMETER Platform
    Target platform: x86, x64, or both (default: both)

.PARAMETER Clean
    Remove build directory before building.

.PARAMETER SkipTests
    Do not build the OPC DA test server and client applications.

.PARAMETER BuildRoot
    Override build directory (default: .\build).
    Also settable via BUILD_ROOT environment variable.

.PARAMETER OutDir
    Override output directory (default: .\out).
    Also settable via OUT_DIR environment variable.

.PARAMETER WixCmd
    Override path to wix executable (default: wix).
    Also settable via WIX_CMD environment variable.

.EXAMPLE
    .\build.ps1                    # both platforms (default)
    .\build.ps1 -Platform x86     # x86 only
    .\build.ps1 -Platform x64     # x64 only
    .\build.ps1 -Clean            # clean rebuild
    . .\signing-key.ps1; .\build.ps1   # with code signing

.NOTES
    Prerequisites:
      - Visual Studio 2022 or 2026 (any edition: Community/Professional/Enterprise)
        with the "Desktop development with C++" workload AND the individual
        component "MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)".
        The v143 toolset is required for Windows 7 SP1 CRT compatibility;
        on VS2026 it is NOT installed by default and must be added via the
        Visual Studio Installer.
      - CMake 3.20+ (included with Visual Studio or install separately)
      - WiX Toolset v4+: dotnet tool install --global wix
      - WiX UI extension: wix extension add WixToolset.UI.wixext
      - AzureSignTool (for code signing): dotnet tool install --global AzureSignTool
#>

[CmdletBinding()]
param(
    [ValidateSet('x86', 'x64', 'both')]
    [string]$Platform = 'both',

    [switch]$Clean,

    [switch]$SkipTests,

    [string]$BuildRoot,
    [string]$OutDir,
    [string]$WixCmd
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# ---------------------------------------------------------------------------
# Resolve paths
# ---------------------------------------------------------------------------
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

if (-not $BuildRoot) {
    $BuildRoot = if ($env:BUILD_ROOT) { $env:BUILD_ROOT } else { Join-Path $ScriptDir 'build' }
}
if (-not $OutDir) {
    $OutDir = if ($env:OUT_DIR) { $env:OUT_DIR } else { Join-Path $ScriptDir 'out' }
}
if (-not $WixCmd) {
    $WixCmd = if ($env:WIX_CMD) { $env:WIX_CMD } else { 'wix' }
}

$WixDir = Join-Path $ScriptDir 'WiX'

# ---------------------------------------------------------------------------
# Visual Studio detection
#
# Locate any installed VS edition (Community/Professional/Enterprise, 2022 or
# 2026) that has the v143 toolset. v143 is required for Windows 7 SP1 CRT
# compatibility; it is not installed by default with VS2026 and must be added
# as the "MSVC v143 - VS 2022 C++ x64/x86 build tools" individual component.
# ---------------------------------------------------------------------------
function Find-VisualStudioWithToolset {
    param([string]$Toolset = 'v143')

    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) {
        throw "vswhere.exe not found at $vswhere. Install Visual Studio 2022 or 2026 with the 'Desktop development with C++' workload."
    }

    $allVs = & $vswhere `
        -products * `
        -requires Microsoft.VisualStudio.Workload.NativeDesktop `
        -sort `
        -format json | ConvertFrom-Json

    if (-not $allVs) {
        throw "No Visual Studio install with the C++ desktop workload was found. Install VS 2022 or VS 2026 and add the 'Desktop development with C++' workload."
    }

    foreach ($vs in $allVs) {
        # The toolset marker is either a .txt (VS2022 and the default toolset
        # on VS2026) or a .props (older toolsets installed as add-ons on a
        # newer VS, e.g. v143 added to VS2026).
        $markerTxt   = Join-Path $vs.installationPath "VC\Auxiliary\Build\Microsoft.VCToolsVersion.$Toolset.default.txt"
        $markerProps = Join-Path $vs.installationPath "VC\Auxiliary\Build\Microsoft.VCToolsVersion.$Toolset.default.props"
        if ((Test-Path $markerTxt) -or (Test-Path $markerProps)) {
            $major = [int]($vs.installationVersion.Split('.')[0])
            $generator = switch ($major) {
                17      { 'Visual Studio 17 2022' }
                18      { 'Visual Studio 18 2026' }
                default { throw "Unsupported Visual Studio major version $major ($($vs.displayName)). Update build.ps1 to add a generator mapping." }
            }
            return [pscustomobject]@{
                InstallPath = $vs.installationPath
                DisplayName = $vs.displayName
                Version     = $vs.installationVersion
                Generator   = $generator
                Toolset     = $Toolset
            }
        }
    }

    # No install has the requested toolset — build a diagnostic listing every
    # detected VS install and which toolsets it does have.
    $foundList = ($allVs | ForEach-Object {
        $tools = Join-Path $_.installationPath 'VC\Auxiliary\Build'
        $available = if (Test-Path $tools) {
            $names = Get-ChildItem $tools -Filter 'Microsoft.VCToolsVersion.v*.default.*' -ErrorAction SilentlyContinue |
                Where-Object { $_.Name -match '\.default\.(props|txt)$' } |
                ForEach-Object { $_.Name -replace '^Microsoft\.VCToolsVersion\.','' -replace '\.default\.(props|txt)$','' } |
                Sort-Object -Unique
            if ($names) { $names -join ', ' } else { '(none)' }
        } else { '(none)' }
        "  - $($_.displayName) ($($_.installationVersion)) - toolsets: $available"
    }) -join [Environment]::NewLine

    throw @"
MSVC $Toolset toolset (VS 2022 C++ x86/x64 build tools) was not found on this
machine. This toolset is required for Windows 7 SP1 CRT compatibility.

Detected Visual Studio installations:
$foundList

Fix: open the Visual Studio Installer, click 'Modify' on your VS install,
switch to 'Individual components', and enable:
  MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)
"@
}

$VSInfo = Find-VisualStudioWithToolset -Toolset 'v143'
Write-Host "Visual Studio:   $($VSInfo.DisplayName) ($($VSInfo.Version))"
Write-Host "Install path:    $($VSInfo.InstallPath)"
Write-Host "CMake generator: $($VSInfo.Generator)"
Write-Host "Toolset:         $($VSInfo.Toolset) (VS 2022, Win7 SP1 CRT compatible)"

# ---------------------------------------------------------------------------
# Code signing detection
# ---------------------------------------------------------------------------
$SigningEnabled = -not [string]::IsNullOrWhiteSpace($env:SigningClientSecret)
if ($SigningEnabled) {
    # Verify AzureSignTool is available
    try {
        & AzureSignTool --version 2>$null | Out-Null
        if ($LASTEXITCODE -ne 0) { throw }
    }
    catch {
        throw "Code signing is enabled but AzureSignTool was not found. Install with: dotnet tool install --global AzureSignTool"
    }
    Write-Host 'Code signing: ENABLED'
} else {
    Write-Host 'Code signing: DISABLED (SigningClientSecret not set)'
}

# ---------------------------------------------------------------------------
# Version: read version.txt + auto-increment build.txt
# ---------------------------------------------------------------------------
$VersionFile = Join-Path $ScriptDir 'version.txt'
$BuildFile   = Join-Path $ScriptDir 'build.txt'

if (-not (Test-Path $VersionFile)) {
    throw "Version file not found: $VersionFile"
}
if (-not (Test-Path $BuildFile)) {
    throw "Build file not found: $BuildFile"
}

# Parse MAJOR, MINOR, REVISION from version.txt
$Major    = $null
$Minor    = $null
$Revision = $null
foreach ($line in (Get-Content $VersionFile)) {
    if ($line -match '^\s*MAJOR\s*=\s*(\d+)')    { $Major    = [int]$Matches[1] }
    if ($line -match '^\s*MINOR\s*=\s*(\d+)')    { $Minor    = [int]$Matches[1] }
    if ($line -match '^\s*REVISION\s*=\s*(\d+)') { $Revision = [int]$Matches[1] }
}
if ($null -eq $Major -or $null -eq $Minor -or $null -eq $Revision) {
    throw "Could not parse MAJOR, MINOR, and REVISION from $VersionFile"
}

# Read, increment, and write back build number
$Build = [int](Get-Content $BuildFile -Raw).Trim() + 1
Set-Content -Path $BuildFile -Value $Build -NoNewline

$Version     = "$Major.$Minor.$Revision.$Build"
$FileVersion = "$Major.$Minor.$Revision"
$CopyrightYear = (Get-Date).Year

# Generate version.h
$VersionH = Join-Path $ScriptDir 'Source\Include\version.h'
$VersionHContent = @"
/* Auto-generated by build scripts. Do not edit manually.
   This placeholder provides defaults for IDE/IntelliSense when
   the build script has not yet been run. */

#define xstr(s) str(s)
#define str(s) #s

#define MAJOR_VERSION $Major
#define MINOR_VERSION $Minor
#define REVISION_VERSION $Revision
#define BUILD_VERSION $Build

#define COPYRIGHT_DATE "$CopyrightYear"
#define FILE_VERSION_TEXT xstr(MAJOR_VERSION) "." xstr(MINOR_VERSION) "." xstr(REVISION_VERSION) "." xstr(BUILD_VERSION)
#define PRODUCT_VERSION_TEXT xstr(MAJOR_VERSION) "." xstr(MINOR_VERSION) "." xstr(REVISION_VERSION) ".0"
"@
Set-Content -Path $VersionH -Value $VersionHContent

Write-Host ''
Write-Host "Building version $Version"
Write-Host ''

# ---------------------------------------------------------------------------
# Helper: build proxy/stub DLLs for one platform
# ---------------------------------------------------------------------------
function Build-Platform {
    param([string]$Arch)

    $BuildDir = Join-Path $BuildRoot $Arch
    $CmakeArch = if ($Arch -eq 'x86') { 'Win32' } else { 'x64' }

    Write-Host ''
    Write-Host '============================================================'
    Write-Host "  Building OPC Proxy/Stub DLLs ($Arch)"
    Write-Host '============================================================'
    Write-Host ''

    # Clean if requested
    if ($Clean -and (Test-Path $BuildDir)) {
        Write-Host "Cleaning $BuildDir..."
        Remove-Item $BuildDir -Recurse -Force
    }

    # Create directories
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $OutDir "$Arch\bin") -Force | Out-Null

    # Configure. Generator and toolset come from the VS detection step at the
    # top of the script. Toolset is pinned to v143 (VS2022) for Windows 7 SP1
    # CRT compatibility, regardless of whether the host VS is 2022 or 2026.
    Write-Host "Configuring CMake for $Arch..."
    $cmakeArgs = @('-S', $ScriptDir, '-B', $BuildDir, '-G', $VSInfo.Generator, '-A', $CmakeArch, '-T', $VSInfo.Toolset, "-DCMAKE_GENERATOR_INSTANCE=$($VSInfo.InstallPath)", "-DCMAKE_INSTALL_PREFIX=$OutDir\$Arch")
    if ($SkipTests) {
        $cmakeArgs += '-DOPC_BUILD_TESTS=OFF'
    }
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed for $Arch." }

    # Build
    Write-Host 'Building...'
    & cmake --build $BuildDir --config Release
    if ($LASTEXITCODE -ne 0) { throw "Build failed for $Arch." }

    # Install
    Write-Host "Installing to $OutDir\$Arch..."
    & cmake --install $BuildDir --config Release
    if ($LASTEXITCODE -ne 0) { throw "Install failed for $Arch." }

    # Sign binaries before WiX packaging
    if ($SigningEnabled) {
        Write-Host "Signing $Arch binaries..."
        $binFiles = @(Get-ChildItem (Join-Path $OutDir "$Arch\bin") -Include '*.dll','*.exe' -Recurse | Select-Object -ExpandProperty FullName)
        $sdkDlls  = @(Get-ChildItem (Join-Path $OutDir "$Arch\include") -Filter '*.dll' -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName)
        Sign-Files ($binFiles + $sdkDlls)
    }

    Write-Host ''
    Write-Host "  $Arch build complete. DLLs in: $OutDir\$Arch\bin"
    Write-Host ''
}

# ---------------------------------------------------------------------------
# Helper: sign files with AzureSignTool
# ---------------------------------------------------------------------------
function Sign-Files {
    param([string[]]$Files)
    if (-not $SigningEnabled) { return }
    $existing = @($Files | Where-Object { Test-Path $_ })
    if ($existing.Count -eq 0) { return }
    Write-Host "  Signing $($existing.Count) file(s)..."
    & AzureSignTool sign `
        -kvu $env:SigningVaultURL `
        -kvi $env:SigningClientId `
        -kvs $env:SigningClientSecret `
        -kvt $env:SigningTenantId `
        -kvc $env:SigningCertName `
        -tr  $env:SigningURL `
        -td  sha256 `
        @existing
    if ($LASTEXITCODE -ne 0) { throw "Code signing failed." }
}

# ---------------------------------------------------------------------------
# Helper: build WiX merge module + installer for one platform
# ---------------------------------------------------------------------------
function Build-Wix {
    param([string]$Arch)

    $BinDir     = Join-Path $OutDir "$Arch\bin"
    $IncludeDir = Join-Path $OutDir "$Arch\include"

    # Platform-specific IDs and filenames
    if ($Arch -eq 'x86') {
        $ModuleGuid  = 'E25DF0F2-0F29-47E8-8537-265285ED9757'
        $MsmName     = "opc-com-proxystub-mergemodule-$FileVersion-x86.msm"
        $SdkGuid     = 'E25DF0F3-0F29-47E8-8537-265285ED9758'
        $SdkMsmName  = "opc-com-sdk-mergemodule-$FileVersion-x86.msm"
        $UpgradeCode = 'ABE618F1-0CCE-4F84-9124-65DB0EF16E00'
        $ProductName = 'OPC Core Components Redistributable (x86)'
        $MsiName     = "opc-core-components-redistributable-$FileVersion-x86.msi"
        $PlatformTag = '(x86)'
    }
    else {
        $ModuleGuid  = 'E68AB89F-6DF7-4750-8BC1-F14B2064F313'
        $MsmName     = "opc-com-proxystub-mergemodule-$FileVersion-x64.msm"
        $SdkGuid     = 'E68AB8A0-6DF7-4750-8BC1-F14B2064F314'
        $SdkMsmName  = "opc-com-sdk-mergemodule-$FileVersion-x64.msm"
        $UpgradeCode = '282C4D0F-722D-4D30-B09C-61F6D4149DE0'
        # Combined installer: delivers BOTH 32-bit and 64-bit components, so it is
        # named "x86-x64" (not "x64") to make that clear. UpgradeCode stays the x64
        # code (282C4D0F) - do not change it; upgrades key off the UpgradeCode.
        $ProductName = 'OPC Core Components Redistributable (x86-x64)'
        $MsiName     = "opc-core-components-redistributable-$FileVersion-x86-x64.msi"
        $PlatformTag = '(x64)'
    }

    Write-Host ''
    Write-Host "--- $Arch ---"

    # Verify DLLs exist
    if (-not (Test-Path (Join-Path $BinDir 'opccomn_ps.dll'))) {
        throw "DLLs not found in $BinDir. Build step may have failed."
    }

    # Build Merge Module (runtime DLLs)
    Write-Host "Building merge module: $MsmName"
    & $WixCmd build (Join-Path $WixDir 'MergeModule.wxs') `
        -arch $Arch `
        -d "BinDir=$BinDir" `
        -d "ModuleGuid=$ModuleGuid" `
        -d "PlatformTag=$PlatformTag" `
        -d "Platform=$Arch" `
        -d "Version=$Version" `
        -bindpath $WixDir `
        -o (Join-Path $MsmDir $MsmName)
    if ($LASTEXITCODE -ne 0) { throw "Merge module build failed for $Arch." }
    Sign-Files @(Join-Path $MsmDir $MsmName)
    Write-Host "  Merge module: $MsmDir\$MsmName"

    # Build SDK Merge Module (headers, IDLs, DLL reference copies)
    Write-Host "Building SDK merge module: $SdkMsmName"
    & $WixCmd build (Join-Path $WixDir 'MergeModuleSdk.wxs') `
        -arch $Arch `
        -d "IncludeDir=$IncludeDir" `
        -d "SdkGuid=$SdkGuid" `
        -d "PlatformTag=$PlatformTag" `
        -d "Platform=$Arch" `
        -d "Version=$Version" `
        -bindpath $WixDir `
        -o (Join-Path $MsmDir $SdkMsmName)
    if ($LASTEXITCODE -ne 0) { throw "SDK merge module build failed for $Arch." }
    Sign-Files @(Join-Path $MsmDir $SdkMsmName)
    Write-Host "  SDK module:   $MsmDir\$SdkMsmName"

    # Build MSI Installer
    Write-Host "Building installer: $MsiName"
    $wixInstallerArgs = @(
        'build', (Join-Path $WixDir 'Installer.wxs'),
        '-arch', $Arch,
        '-d', "MsmFile=$(Join-Path $MsmDir $MsmName)",
        '-d', "SdkMsmFile=$(Join-Path $MsmDir $SdkMsmName)",
        '-d', "UpgradeCode=$UpgradeCode",
        '-d', "ProductName=$ProductName",
        '-d', "Version=$Version",
        '-d', "BinDir=$BinDir",
        '-d', "Platform=$Arch",
        '-ext', 'WixToolset.UI.wixext',
        '-bindpath', $WixDir,
        '-o', (Join-Path $MsmDir $MsiName)
    )

    # Include test applications if they were built
    $HasTests = Test-Path (Join-Path $BinDir "OpcTestServer_$Arch.exe")
    if ($HasTests) {
        $wixInstallerArgs += '-d'
        $wixInstallerArgs += 'IncludeTests=1'
    }

    # x64 installer is a combined package: it bundles the x86 merge modules (32-bit
    # COM support) and optionally the x86 test binaries. The x86 .msm/.msi must have
    # been built first (build 'both' or x86 before x64).
    if ($Arch -eq 'x64') {
        $MsmFileX86 = Join-Path $MsmDir "opc-com-proxystub-mergemodule-$FileVersion-x86.msm"
        if (-not (Test-Path $MsmFileX86)) {
            throw "x86 merge module not found: $MsmFileX86. Build x86 platform first."
        }
        $SdkMsmFileX86 = Join-Path $MsmDir "opc-com-sdk-mergemodule-$FileVersion-x86.msm"
        if (-not (Test-Path $SdkMsmFileX86)) {
            throw "x86 SDK merge module not found: $SdkMsmFileX86. Build x86 platform first."
        }
        $wixInstallerArgs += '-d'
        $wixInstallerArgs += "MsmFileX86=$MsmFileX86"
        $wixInstallerArgs += '-d'
        $wixInstallerArgs += "SdkMsmFileX86=$SdkMsmFileX86"

        $BinDirX86 = Join-Path $OutDir 'x86\bin'
        if ($HasTests) {
            if (-not (Test-Path (Join-Path $BinDirX86 'OpcTestServer_x86.exe'))) {
                throw "x86 test binaries not found in $BinDirX86. Build x86 platform first (without -SkipTests)."
            }
            $wixInstallerArgs += '-d'
            $wixInstallerArgs += "BinDirX86=$BinDirX86"
        }
    }
    & $WixCmd @wixInstallerArgs
    if ($LASTEXITCODE -ne 0) { throw "Installer build failed for $Arch." }
    Sign-Files @(Join-Path $MsmDir $MsiName)
    Write-Host "  Installer:    $MsmDir\$MsiName"

}

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------
try {
    # Create top-level output directories
    New-Item -ItemType Directory -Path $BuildRoot -Force | Out-Null
    New-Item -ItemType Directory -Path $OutDir -Force | Out-Null

    # Build DLLs
    $platforms = if ($Platform -eq 'both') { @('x86', 'x64') } else { @($Platform) }

    foreach ($arch in $platforms) {
        Build-Platform $arch
    }

    # Build WiX packages
    Write-Host ''
    Write-Host '============================================================'
    Write-Host '  Building WiX Merge Module and Installer'
    Write-Host '============================================================'
    Write-Host ''

    # Check for wix tool
    try {
        & $WixCmd --version 2>$null | Out-Null
        if ($LASTEXITCODE -ne 0) { throw }
    }
    catch {
        Write-Host 'ERROR: WiX Toolset not found. Install with: dotnet tool install --global wix'
        Write-Host '       Or set -WixCmd / WIX_CMD to the full path of wix.exe'
        exit 1
    }

    # Get WiX version and ensure matching UI extension is installed
    $wixVer = (& $WixCmd --version 2>$null) -replace '\+.*', ''
    Write-Host "Using WiX $wixVer"
    & $WixCmd extension add "WixToolset.UI.wixext/$wixVer" 2>$null | Out-Null

    $MsmDir = Join-Path $OutDir 'wix'
    New-Item -ItemType Directory -Path $MsmDir -Force | Out-Null

    foreach ($arch in $platforms) {
        Build-Wix $arch
    }

    # -------------------------------------------------------------------
    # Create redistributable ZIP
    # -------------------------------------------------------------------
    Write-Host ''
    Write-Host '============================================================'
    Write-Host '  Creating Redistributable ZIP'
    Write-Host '============================================================'
    Write-Host ''

    $DistDir = Join-Path $ScriptDir 'dist'
    New-Item -ItemType Directory -Path $DistDir -Force | Out-Null

    $ZipName = "OPC-Core-Components-Redistributables-$Major.$Minor.$Revision.zip"
    $ZipPath = Join-Path $DistDir $ZipName

    # Remove existing ZIP if present
    if (Test-Path $ZipPath) { Remove-Item $ZipPath -Force }

    # Stage the ZIP contents in a clean directory so we control file names (the
    # end-user redistributable README ships as README.md; the on-demand cleanup
    # utility and LICENSE are included alongside the .msm/.msi files).
    $StageDir = Join-Path $OutDir 'zip-staging'
    if (Test-Path $StageDir) { Remove-Item $StageDir -Recurse -Force }
    New-Item -ItemType Directory -Path $StageDir -Force | Out-Null

    # Only the current version's artifacts (out\wix accumulates prior builds; we
    # must not ship stale .msm/.msi from an earlier version in the redistributable).
    $msmMsi = @(Get-ChildItem $MsmDir -Include "*-$FileVersion-*.msm","*-$FileVersion-*.msi" -Recurse | Select-Object -ExpandProperty FullName)
    if ($msmMsi.Count -eq 0) {
        throw "No $FileVersion .msm/.msi found in $MsmDir."
    }
    foreach ($f in $msmMsi) { Copy-Item -LiteralPath $f -Destination $StageDir }

    # End-user README (explains installation + why the cleanup script exists).
    $redistReadme = Join-Path $ScriptDir 'Redistributable-README.md'
    if (Test-Path $redistReadme) {
        Copy-Item -LiteralPath $redistReadme -Destination (Join-Path $StageDir 'README.md')
    } else {
        throw "Redistributable README not found: $redistReadme"
    }

    # On-demand cleanup utility (removes orphaned SysWOW64 DLLs / corrupt SharedDLLs).
    $cleanupScript = Join-Path $ScriptDir 'Cleanup-OpcCoreComponents.ps1'
    if (Test-Path $cleanupScript) {
        Copy-Item -LiteralPath $cleanupScript -Destination $StageDir
    } else {
        throw "Cleanup script not found: $cleanupScript"
    }

    $licensePath = Join-Path $ScriptDir 'LICENSE.md'
    if (Test-Path $licensePath) { Copy-Item -LiteralPath $licensePath -Destination $StageDir }

    $stagedFiles = @(Get-ChildItem $StageDir -File | Select-Object -ExpandProperty FullName)
    if ($stagedFiles.Count -eq 0) {
        throw "No files found to include in redistributable ZIP."
    }

    Write-Host "Creating $ZipName with $($stagedFiles.Count) file(s)..."
    Compress-Archive -Path $stagedFiles -DestinationPath $ZipPath -CompressionLevel Optimal
    Write-Host "  ZIP: $ZipPath"

    Write-Host ''
    Write-Host '============================================================'
    Write-Host '  BUILD COMPLETE'
    Write-Host '============================================================'
    Write-Host ''
    Write-Host "  Output files in: $MsmDir"
    Write-Host "  Redistributable: $ZipPath"
    Write-Host ''
}
catch {
    Write-Host "ERROR: $_" -ForegroundColor Red
    exit 1
}
