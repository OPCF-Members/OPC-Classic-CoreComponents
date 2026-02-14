@echo off
setlocal enabledelayedexpansion

:: Save user's platform choice before vcvarsall overwrites %PLATFORM%
set "BUILD_PLATFORM=%~1"
if "%BUILD_PLATFORM%"=="" set "BUILD_PLATFORM=both"

echo.
echo ============================================================
echo  OPC Core Components - Docker Build (%BUILD_PLATFORM%)
echo ============================================================
echo.

:: Verify mounts
if not exist "C:\src\build.ps1" (
    echo ERROR: Source not mounted. Mount the source directory to C:\src
    echo   docker run --rm -v path\to\source:C:\src -v path\to\out:C:\out IMAGE
    exit /b 1
)
if not exist "C:\out" (
    echo ERROR: Output directory not mounted to C:\out
    exit /b 1
)

:: Set up VS build environment (note: vcvarsall overwrites %PLATFORM%)
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if errorlevel 1 (
    echo ERROR: Failed to set up Visual Studio environment.
    exit /b 1
)

:: Build with intermediate files on container-local disk (PDB locking fails on volume mounts)
set "DOTNET_ROOT=C:\dotnet"
cd /d "C:\src"
%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -ExecutionPolicy Bypass -File "C:\src\build.ps1" -Platform %BUILD_PLATFORM% -BuildRoot C:\build -OutDir C:\out_local -WixCmd C:\wix\wix.exe
if errorlevel 1 (
    echo ERROR: Build failed.
    exit /b 1
)

:: Copy output to mounted output directory
echo.
echo Copying output to C:\out ...
%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -Command "Copy-Item -Path 'C:\out_local\*' -Destination 'C:\out' -Recurse -Force"

:: Copy dist/ output (ZIP redistributable) if it exists
if exist "C:\src\dist" (
    echo Copying redistributable ZIP to C:\out\dist ...
    %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -Command "New-Item -ItemType Directory -Path 'C:\out\dist' -Force | Out-Null; Copy-Item -Path 'C:\src\dist\*' -Destination 'C:\out\dist' -Recurse -Force"
)

echo.
echo  Docker build complete. Output copied to mounted output directory.
echo.
