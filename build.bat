@echo off
setlocal enabledelayedexpansion

:: ============================================================================
:: OPC Core Components - Build Script
::
:: Prerequisites:
::   - Visual Studio 2019+ (or Build Tools) with C++ desktop workload
::   - CMake 3.20+ (included with Visual Studio or install separately)
::   - WiX Toolset v4/v5: dotnet tool install --global wix
::   - WiX UI extension:  wix extension add WixToolset.UI.wixext
::
:: Usage:
::   build.bat [x86|x64|both] [clean]
::
::   build.bat          - Build for current platform (default x64)
::   build.bat x86      - Build for 32-bit
::   build.bat x64      - Build for 64-bit
::   build.bat both     - Build for both platforms
::   build.bat x64 clean - Clean and rebuild
:: ============================================================================

:: Strip trailing backslash from %~dp0 to avoid quoting issues with cmake
set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "BUILD_ROOT=%SCRIPT_DIR%\build"
set "OUT_DIR=%SCRIPT_DIR%\out"
set "WIX_DIR=%SCRIPT_DIR%\WiX"

:: Parse arguments
set "PLATFORM=%~1"
set "CLEAN=%~2"
if "%PLATFORM%"=="" set "PLATFORM=x64"
if /i "%PLATFORM%"=="clean" (
    set "CLEAN=clean"
    set "PLATFORM=x64"
)

:: Create top-level output directories
if not exist "%BUILD_ROOT%" mkdir "%BUILD_ROOT%"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

:: Validate platform
if /i "%PLATFORM%"=="both" goto :build_both
if /i "%PLATFORM%"=="x86" goto :build_single
if /i "%PLATFORM%"=="x64" goto :build_single

echo ERROR: Invalid platform "%PLATFORM%". Use x86, x64, or both.
exit /b 1

:build_both
echo.
echo === Building for both x86 and x64 ===
echo.
call :do_build x86
if errorlevel 1 exit /b 1
call :do_build x64
if errorlevel 1 exit /b 1
goto :build_wix

:build_single
call :do_build %PLATFORM%
if errorlevel 1 exit /b 1
goto :build_wix

:: ============================================================================
:: Build proxy stub DLLs for one platform
:: ============================================================================
:do_build
set "ARCH=%~1"
set "BUILD_DIR=%BUILD_ROOT%\%ARCH%"

echo.
echo ============================================================
echo  Building OPC Proxy/Stub DLLs (%ARCH%)
echo ============================================================
echo.

:: Select CMake architecture
if /i "%ARCH%"=="x86" (
    set "CMAKE_ARCH=Win32"
) else (
    set "CMAKE_ARCH=x64"
)

:: Clean if requested
if /i "%CLEAN%"=="clean" (
    echo Cleaning %BUILD_DIR%...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

:: Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

:: Create install output directory
if not exist "%OUT_DIR%\%ARCH%\bin" mkdir "%OUT_DIR%\%ARCH%\bin"

:: Configure with CMake
echo Configuring CMake for %ARCH%...
cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -A "%CMAKE_ARCH%" -DCMAKE_INSTALL_PREFIX="%OUT_DIR%\%ARCH%"
if errorlevel 1 (
    echo ERROR: CMake configuration failed for %ARCH%.
    exit /b 1
)

:: Build
echo Building...
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 (
    echo ERROR: Build failed for %ARCH%.
    exit /b 1
)

:: Install (copies DLLs to out directory)
echo Installing to %OUT_DIR%\%ARCH%...
cmake --install "%BUILD_DIR%" --config Release
if errorlevel 1 (
    echo ERROR: Install failed for %ARCH%.
    exit /b 1
)

echo.
echo  %ARCH% build complete. DLLs in: %OUT_DIR%\%ARCH%\bin
echo.
exit /b 0

:: ============================================================================
:: Build WiX merge module and installer
:: ============================================================================
:build_wix

echo.
echo ============================================================
echo  Building WiX Merge Module and Installer
echo ============================================================
echo.

:: Determine which platform output to use for the installer
if /i "%PLATFORM%"=="both" (
    set "BIN_ARCH=x64"
) else (
    set "BIN_ARCH=%PLATFORM%"
)

set "BIN_DIR=%OUT_DIR%\%BIN_ARCH%\bin"
set "MSM_DIR=%OUT_DIR%\wix"

:: Verify DLLs exist
if not exist "%BIN_DIR%\opccomn_ps.dll" (
    echo ERROR: DLLs not found in %BIN_DIR%. Build step may have failed.
    exit /b 1
)

:: Create WiX output directory
if not exist "%MSM_DIR%" mkdir "%MSM_DIR%"

:: Check for wix tool
where wix >nul 2>&1
if errorlevel 1 (
    echo ERROR: WiX Toolset not found. Install with: dotnet tool install --global wix
    exit /b 1
)

:: Get WiX version and ensure matching UI extension is installed
for /f "tokens=1 delims=+" %%v in ('wix --version 2^>nul') do set "WIX_VER=%%v"
echo Using WiX %WIX_VER%
wix extension add WixToolset.UI.wixext/%WIX_VER% >nul 2>&1

:: Build Merge Module
echo Building merge module...
wix build "%WIX_DIR%\MergeModule.wxs" ^
    -d BinDir="%BIN_DIR%" ^
    -bindpath "%WIX_DIR%" ^
    -o "%MSM_DIR%\MergeModule.msm"
if errorlevel 1 (
    echo ERROR: Merge module build failed.
    exit /b 1
)
echo  Merge module: %MSM_DIR%\MergeModule.msm

:: Build MSI Installer
echo Building installer...
wix build "%WIX_DIR%\Installer.wxs" ^
    -d MsmDir="%MSM_DIR%" ^
    -ext WixToolset.UI.wixext ^
    -bindpath "%WIX_DIR%" ^
    -o "%MSM_DIR%\OpcCoreComponents.msi"
if errorlevel 1 (
    echo ERROR: Installer build failed.
    exit /b 1
)
echo  Installer:    %MSM_DIR%\OpcCoreComponents.msi

echo.
echo ============================================================
echo  BUILD COMPLETE
echo ============================================================
echo.
echo  Output files:
echo    DLLs:          %BIN_DIR%\
echo    Merge Module:  %MSM_DIR%\MergeModule.msm
echo    Installer:     %MSM_DIR%\OpcCoreComponents.msi
echo.

exit /b 0
