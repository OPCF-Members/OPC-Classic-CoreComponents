@echo off
setlocal enabledelayedexpansion

set "PLATFORM=%~1"
if "%PLATFORM%"=="" set "PLATFORM=both"

echo.
echo ============================================================
echo  OPC Core Components - Docker Build (%PLATFORM%)
echo ============================================================
echo.

:: Verify mounts
if not exist "C:\src\build.bat" (
    echo ERROR: Source not mounted. Mount the Installer directory to C:\src
    echo   docker run --rm -v path\to\Installer:C:\src -v path\to\out:C:\out IMAGE
    exit /b 1
)
if not exist "C:\out" (
    echo ERROR: Output directory not mounted to C:\out
    exit /b 1
)

:: Set up VS build environment
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if errorlevel 1 (
    echo ERROR: Failed to set up Visual Studio environment.
    exit /b 1
)

:: Build with intermediate files on container-local disk (PDB locking fails on volume mounts)
set "BUILD_ROOT=C:\build"
set "OUT_DIR=C:\out_local"
set "DOTNET_ROOT=C:\dotnet"
set "WIX_CMD=C:\wix\wix.exe"
cd /d "C:\src"
call build.bat %PLATFORM%
if errorlevel 1 (
    echo ERROR: Build failed.
    exit /b 1
)

:: Copy output to mounted output directory
echo.
echo Copying output to C:\out ...
%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -Command "Copy-Item -Path 'C:\out_local\*' -Destination 'C:\out' -Recurse -Force"

echo.
echo  Docker build complete. Output copied to mounted output directory.
echo.
