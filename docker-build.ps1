<#
.SYNOPSIS
    OPC Core Components - Docker Build

.DESCRIPTION
    Builds the OPC Core Components inside a Docker container.
    Automatically builds the Docker image if it does not exist.

.PARAMETER Platform
    Target platform: x86, x64, or both (default: both)

.PARAMETER OutDir
    Host directory for build output (default: .\out).

.PARAMETER NoBuild
    Skip building the Docker image (use existing image).

.PARAMETER Rebuild
    Force rebuild the Docker image even if it already exists.

.EXAMPLE
    .\docker-build.ps1                    # both platforms (default)
    .\docker-build.ps1 -Platform x86      # x86 only
    .\docker-build.ps1 -Rebuild           # force image rebuild
    .\docker-build.ps1 -OutDir C:\output  # custom output directory

.NOTES
    Prerequisites:
      - Docker Desktop with Windows containers enabled
#>

[CmdletBinding()]
param(
    [ValidateSet('x86', 'x64', 'both')]
    [string]$Platform = 'both',

    [string]$OutDir,

    [switch]$NoBuild,
    [switch]$Rebuild
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$ScriptDir  = Split-Path -Parent $MyInvocation.MyCommand.Definition
$ImageName  = 'opc-core-builder'
$DockerDir  = Join-Path $ScriptDir 'docker'

if (-not $OutDir) {
    $OutDir = Join-Path $ScriptDir 'out'
}

# ---------------------------------------------------------------------------
# Verify Docker is available and running Windows containers
# ---------------------------------------------------------------------------
try {
    $dockerVersion = & docker version --format '{{.Server.Os}}' 2>$null
    if ($LASTEXITCODE -ne 0) { throw }
}
catch {
    throw 'Docker is not running. Start Docker Desktop and ensure Windows containers are enabled.'
}

if ($dockerVersion -ne 'windows') {
    throw "Docker is running Linux containers. Switch to Windows containers (right-click Docker Desktop tray icon)."
}

# ---------------------------------------------------------------------------
# Build Docker image if needed
# ---------------------------------------------------------------------------
if (-not $NoBuild) {
    $imageExists = (& docker images -q $ImageName 2>$null) -ne ''

    if ($Rebuild -or -not $imageExists) {
        $action = if ($Rebuild) { 'Rebuilding' } else { 'Building' }
        Write-Host ''
        Write-Host "============================================================"
        Write-Host "  $action Docker image: $ImageName"
        Write-Host "============================================================"
        Write-Host ''

        & docker build -t $ImageName $DockerDir
        if ($LASTEXITCODE -ne 0) { throw "Docker image build failed." }

        Write-Host ''
        Write-Host "  Docker image ready: $ImageName"
        Write-Host ''
    }
    else {
        Write-Host "Docker image '$ImageName' exists. Use -Rebuild to force rebuild."
    }
}

# ---------------------------------------------------------------------------
# Run the build container
# ---------------------------------------------------------------------------
New-Item -ItemType Directory -Path $OutDir -Force | Out-Null

Write-Host ''
Write-Host '============================================================'
Write-Host "  Running Docker build ($Platform)"
Write-Host '============================================================'
Write-Host ''
Write-Host "  Source:  $ScriptDir"
Write-Host "  Output:  $OutDir"
Write-Host ''

& docker run --rm `
    -v "${ScriptDir}:C:\src" `
    -v "${OutDir}:C:\out" `
    $ImageName $Platform

if ($LASTEXITCODE -ne 0) {
    throw "Docker build failed."
}

Write-Host ''
Write-Host '============================================================'
Write-Host '  DOCKER BUILD COMPLETE'
Write-Host '============================================================'
Write-Host ''
Write-Host "  Output: $OutDir"
Write-Host ''
