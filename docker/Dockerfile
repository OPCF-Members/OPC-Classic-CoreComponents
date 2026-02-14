# escape=`
# OPC Core Components - Build Environment
#
# Contains all tools needed to build the proxy/stub DLLs and WiX installer.
# Source is mounted at runtime (not baked into the image).
#
# Build:  docker build -t opc-core-builder docker
# Run:    docker run --rm -v <source>:C:\src -v <output>:C:\out opc-core-builder [x86|x64|both]

FROM mcr.microsoft.com/windows/servercore:ltsc2022

SHELL ["cmd", "/S", "/C"]

# ----------------------------------------------------------------------------
# 1. Visual Studio 2022 Build Tools
#    - C++ build tools workload (includes CMake, MSBuild, Windows SDK)
#    - ATL needed by OpcEnum.exe
# ----------------------------------------------------------------------------
RUN powershell -Command " `
    $ProgressPreference = 'SilentlyContinue'; `
    Invoke-WebRequest -Uri https://aka.ms/vs/17/release/vs_buildtools.exe -OutFile C:\vs_buildtools.exe" `
 && C:\vs_buildtools.exe --quiet --wait --norestart --nocache `
        --installPath C:\BuildTools `
        --add Microsoft.VisualStudio.Workload.VCTools `
        --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        --add Microsoft.VisualStudio.Component.VC.ATL `
        --add Microsoft.VisualStudio.Component.VC.CMake.Project `
        --add Microsoft.VisualStudio.Component.Windows11SDK.22621 `
 && del /q C:\vs_buildtools.exe

# ----------------------------------------------------------------------------
# 2. .NET SDK 8.0 (needed for WiX dotnet tool)
# ----------------------------------------------------------------------------
RUN powershell -Command " `
    $ProgressPreference = 'SilentlyContinue'; `
    Invoke-WebRequest -Uri https://dot.net/v1/dotnet-install.ps1 -OutFile C:\dotnet-install.ps1; `
    C:\dotnet-install.ps1 -Channel 8.0 -InstallDir C:\dotnet; `
    Remove-Item C:\dotnet-install.ps1"

ENV DOTNET_ROOT="C:\dotnet" `
    PATH="C:\dotnet;${PATH}"

# ----------------------------------------------------------------------------
# 3. WiX Toolset + UI extension (install to known path)
# ----------------------------------------------------------------------------
RUN dotnet tool install wix --tool-path C:\wix

ENV PATH="C:\wix;${PATH}"

RUN for /f "tokens=1 delims=+" %v in ('wix --version') do `
        wix extension add WixToolset.UI.wixext/%v

# Place wix/dotnet wrappers in C:\Windows so they survive vcvarsall PATH overwrite
RUN echo @set "DOTNET_ROOT=C:\dotnet" ^& "C:\wix\wix.exe" %* > C:\Windows\System32\wix.cmd `
 && echo @"C:\dotnet\dotnet.exe" %* > C:\Windows\System32\dotnet.cmd

# ----------------------------------------------------------------------------
# Entrypoint
# ----------------------------------------------------------------------------
COPY entrypoint.cmd C:\entrypoint.cmd

ENTRYPOINT ["cmd", "/S", "/C", "C:\\entrypoint.cmd"]
CMD ["both"]
