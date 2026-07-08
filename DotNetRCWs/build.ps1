<#
.SYNOPSIS
    Builds the OPC Classic .NET API (DotNetRCWs) assemblies and produces NuGet packages.

.DESCRIPTION
    Restores, builds, and packs the three projects:

        OpcComRcw       - OPC Classic Runtime Callable Wrapper (RCW)
        OpcNetApi       - OPC Classic .NET API
        OpcNetApi.Com   - OPC Classic .NET API COM Wrapper (depends on the two above)

    Each project multi-targets netstandard2.0, netstandard2.1, net46,
    net6.0-windows, net8.0-windows and net10.0-windows. Only the .NET SDK is
    required to build - the .NET Framework targeting pack is supplied through the
    Microsoft.NETFramework.ReferenceAssemblies package.

    Signing follows the same model as the other tools in this repository. Set the
    values in signing-key.ps1 and dot-source it first:

        . .\signing-key.ps1
        .\build.ps1

    - Strong-name signing: if $env:SigningKeyFile points at a .snk the assemblies
      are strong-named with it; otherwise the placeholder key under .\Keys is used.
    - Authenticode signing via Azure Key Vault: when SigningCertName, SigningClientId,
      SigningClientSecret, SigningTenantId and SigningVaultURL are all set, the
      binaries are signed with AzureSignTool and the packages with
      NuGetKeyVaultSignTool (both installed as global tools on demand).

.PARAMETER Version
    NuGet package version (e.g. 2.2.131). When not supplied it is composed from
    major.minor in version.txt plus the build number in build.txt
    (version.txt=2.2 + build.txt=131 -> 2.2.131). FileVersion / ProductVersion is
    that value padded to four fields (2.2.131.0). Edit build.txt by hand before
    building to stamp a new build; the strong-name AssemblyVersion stays pinned
    at 2.0.0.0.

.PARAMETER Configuration
    Build configuration: Release (default) or Debug.

.PARAMETER OutDir
    Directory the .nupkg / .snupkg files are written to. Default: .\dist.

.PARAMETER SkipSign
    Skip Authenticode/Key Vault signing even when the signing environment
    variables are set.

.PARAMETER Clean
    Remove bin/obj and the output directory before building.

.EXAMPLE
    .\build.ps1
    .\build.ps1 -Version 2.1.131
    . .\signing-key.ps1; .\build.ps1        # strong-name + Key Vault signing
    .\build.ps1 -Clean
#>
[CmdletBinding()]
param(
    [string]$Version,
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [string]$OutDir,
    [switch]$SkipSign,
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$solution = Join-Path $root 'DotNetRCWs.slnx'

if (-not $OutDir) { $OutDir = Join-Path $root 'dist' }

# --- Resolve version -------------------------------------------------------
# Build number = patch field, read from build.txt. Unlike the native build's
# build.txt this is NOT auto-incremented; edit it by hand before building to
# stamp a new build. The strong-name AssemblyVersion stays pinned at 2.0.0.0.
$buildFile = Join-Path $root 'build.txt'
if (Test-Path $buildFile) {
    $build = (Get-Content $buildFile -Raw).Trim()
} else {
    $build = '0'
}
if ($build -notmatch '^\d+$') {
    throw "build.txt must contain a single integer build number (found '$build')."
}

# Package version = major.minor (from version.txt) + build (from build.txt),
# e.g. version.txt=2.2 (or 2.2.0) + build.txt=131 -> 2.2.131. Only the first two
# fields of version.txt are used; any patch there is replaced by build.txt.
if (-not $Version) {
    $versionFile = Join-Path $root 'version.txt'
    if (Test-Path $versionFile) {
        $baseVersion = (Get-Content $versionFile -Raw).Trim()
    } else {
        $baseVersion = '2.1'
    }
    $vparts = ($baseVersion -split '-')[0] -split '\.'
    $major  = $vparts[0]
    $minor  = if ($vparts.Count -ge 2) { $vparts[1] } else { '0' }
    $Version = "$major.$minor.$build"
}

# 4-part numeric FileVersion from the package version (drop any -prerelease tag),
# padded with trailing zeros -> e.g. 2.2.131 -> 2.2.131.0.
$numeric = ($Version -split '-')[0]
$parts = @($numeric -split '\.') + @('0', '0', '0', '0')
$fileVersion = ($parts[0..3]) -join '.'

Write-Host "==> OPC Classic .NET API build" -ForegroundColor Cyan
Write-Host "    Version       : $Version"
Write-Host "    File version  : $fileVersion"
Write-Host "    Configuration : $Configuration"
Write-Host "    Output        : $OutDir"

# --- Verify the .NET SDK is available --------------------------------------
if (-not (Get-Command dotnet -ErrorAction SilentlyContinue)) {
    throw "The .NET SDK ('dotnet') was not found on PATH. Install it from https://dotnet.microsoft.com."
}
Write-Host "    dotnet SDK    : $(dotnet --version)"

$msbuildProps = @("-p:Version=$Version", "-p:FileVersion=$fileVersion", "-p:InformationalVersion=$fileVersion")

# --- Strong-name key (from signing-key.ps1 via $env:SigningKeyFile) ---------
if ($env:SigningKeyFile) {
    if (-not (Test-Path $env:SigningKeyFile)) { throw "SigningKeyFile not found: $($env:SigningKeyFile)" }
    $keyPath = (Resolve-Path $env:SigningKeyFile).Path
    Write-Host "    Strong-name   : $keyPath"
    $msbuildProps += "-p:AssemblyOriginatorKeyFile=$keyPath"
    $msbuildProps += "-p:SignAssembly=true"
} else {
    Write-Host "    Strong-name   : placeholder key (.\Keys)"
}

# --- Authenticode / Key Vault signing detection -----------------------------
$CanSign = (-not $SkipSign) `
    -and $env:SigningCertName `
    -and $env:SigningClientId `
    -and $env:SigningClientSecret `
    -and $env:SigningTenantId `
    -and $env:SigningVaultURL
if ($CanSign) {
    Write-Host "    Code signing  : ENABLED (Azure Key Vault)" -ForegroundColor Green
} else {
    Write-Host "    Code signing  : disabled (signing-key.ps1 not set)" -ForegroundColor Yellow
}
$timestampUrl = if ($env:SigningURL) { $env:SigningURL } else { 'http://timestamp.digicert.com' }

# --- Clean ------------------------------------------------------------------
if ($Clean) {
    Write-Host "==> Cleaning" -ForegroundColor Cyan
    Get-ChildItem -Path $root -Recurse -Directory -Include bin, obj -ErrorAction SilentlyContinue |
        ForEach-Object { Remove-Item $_.FullName -Recurse -Force -ErrorAction SilentlyContinue }
    if (Test-Path $OutDir) { Remove-Item $OutDir -Recurse -Force }
}

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

# --- Restore + build --------------------------------------------------------
Write-Host "==> Restoring and building" -ForegroundColor Cyan
dotnet build $solution -c $Configuration @msbuildProps
if ($LASTEXITCODE -ne 0) { throw "Build failed (exit $LASTEXITCODE)." }

# --- Sign binaries (before packing, so packages contain signed assemblies) --
if ($CanSign) {
    Write-Host "==> Signing binaries (AzureSignTool)" -ForegroundColor Cyan
    if (-not (Get-Command AzureSignTool -ErrorAction SilentlyContinue)) {
        Write-Host "    installing AzureSignTool..." -ForegroundColor DarkGray
        dotnet tool install --global AzureSignTool | Out-Null
    }

    $binFiles = Get-ChildItem -Path $root -Recurse -Include '*.dll', '*.exe' -ErrorAction SilentlyContinue |
        Where-Object {
            $_.FullName -match "\\bin\\$Configuration\\" -and
            $_.Name -notmatch '^(System\.|Microsoft\.)'
        } |
        Select-Object -ExpandProperty FullName -Unique

    if ($binFiles) {
        $fileList = [System.IO.Path]::GetTempFileName()
        try {
            Set-Content -Path $fileList -Value $binFiles -Encoding utf8
            AzureSignTool sign `
                --azure-key-vault-url            $env:SigningVaultURL `
                --azure-key-vault-client-id      $env:SigningClientId `
                --azure-key-vault-client-secret  $env:SigningClientSecret `
                --azure-key-vault-tenant-id      $env:SigningTenantId `
                --azure-key-vault-certificate    $env:SigningCertName `
                --timestamp-rfc3161              $timestampUrl `
                --description                    'OPC Classic .NET API' `
                -ifl $fileList
            if ($LASTEXITCODE -ne 0) { throw "Binary signing failed." }
        } finally {
            Remove-Item $fileList -ErrorAction SilentlyContinue
        }
    }
}

# --- Pack -------------------------------------------------------------------
# OpcNetApi.Com is packed last; its ProjectReferences make the resulting package
# depend on the OpcComRcw and OpcNetApi packages at the same version.
$projects = @(
    'OpcComRcw\OpcComRcw.csproj',
    'OpcNetApi\OpcNetApi.csproj',
    'OpcNetApi.Com\OpcNetApi.Com.csproj'
)

Write-Host "==> Packing NuGet packages" -ForegroundColor Cyan
foreach ($proj in $projects) {
    $projPath = Join-Path $root $proj
    dotnet pack $projPath -c $Configuration --no-build -o $OutDir @msbuildProps
    if ($LASTEXITCODE -ne 0) { throw "Pack failed for $proj (exit $LASTEXITCODE)." }
}

# --- Sign NuGet packages ----------------------------------------------------
if ($CanSign) {
    Write-Host "==> Signing NuGet packages (NuGetKeyVaultSignTool)" -ForegroundColor Cyan
    if (-not (Get-Command NuGetKeyVaultSignTool -ErrorAction SilentlyContinue)) {
        Write-Host "    installing NuGetKeyVaultSignTool..." -ForegroundColor DarkGray
        dotnet tool install --global NuGetKeyVaultSignTool | Out-Null
    }

    Get-ChildItem -Path $OutDir -Filter '*.nupkg' | ForEach-Object {
        Write-Host "    signing $($_.Name)" -ForegroundColor DarkGray
        NuGetKeyVaultSignTool sign $_.FullName `
            -kvu $env:SigningVaultURL `
            -kvc $env:SigningCertName `
            -kvt $env:SigningTenantId `
            -kvi $env:SigningClientId `
            -kvs $env:SigningClientSecret `
            -tr  $timestampUrl `
            -td  sha256
        if ($LASTEXITCODE -ne 0) { throw "NuGet package signing failed for $($_.Name)" }
    }
}

# --- Assemble redistributable zip -------------------------------------------
# Self-contained bundle: the .nupkg files under packages\, the OPC Foundation
# license as LICENSE.md, the consumer README, and a NuGet.Config whose local
# source is repointed from .\dist to the bundled .\packages folder.
Write-Host "==> Assembling redistributable zip" -ForegroundColor Cyan
$stage = Join-Path $OutDir 'redistributable'
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
$stagePackages = Join-Path $stage 'packages'
New-Item -ItemType Directory -Force -Path $stagePackages | Out-Null

Get-ChildItem -Path $OutDir -Filter '*.nupkg' | Copy-Item -Destination $stagePackages

Copy-Item (Join-Path $root 'nuget\redistributables-license.txt') (Join-Path $stage 'LICENSE.md')
Copy-Item (Join-Path $root 'redistributable-README.md')          (Join-Path $stage 'redistributable-README.md')

# NuGet.Config with the local source pointed at the bundled packages folder.
$nugetConfig = Get-Content (Join-Path $root 'NuGet.Config') -Raw
$nugetConfig = $nugetConfig -replace '\.\\dist', '.\packages'
Set-Content -Path (Join-Path $stage 'NuGet.Config') -Value $nugetConfig -Encoding utf8

$zipPath = Join-Path $OutDir "OpcNetApi-redistributable-$Version.zip"
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zipPath
Remove-Item $stage -Recurse -Force

Write-Host ""
Write-Host "==> Done. Output in $OutDir" -ForegroundColor Green
Get-ChildItem -Path $OutDir -Filter *.nupkg | Sort-Object Name | ForEach-Object { Write-Host "    $($_.Name)" }
Write-Host "    $(Split-Path $zipPath -Leaf)" -ForegroundColor Green
