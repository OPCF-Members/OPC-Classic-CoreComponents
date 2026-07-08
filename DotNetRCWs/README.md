# OPC Classic .NET API (DotNetRCWs)

Managed .NET assemblies and NuGet packages for OPC Classic (COM/DCOM) clients
and servers. This is the **.NET** side of the OPC Classic Core Components — the
Runtime Callable Wrapper (RCW) for the OPC COM interfaces plus the OPC Classic
.NET API and its COM wrapper.

These projects build with only the **.NET SDK** installed — no Visual Studio C++
workload, no legacy MSVC CRT, and no .NET Framework targeting pack are required.

## ⚠️ CRITICAL NOTICE & DISCLAIMER

### 1. Legacy and End-of-Life (EOL) Status
**This codebase is strictly legacy, entirely unmaintained, and has reached its final End-of-Life (EOL).** It will not receive any feature updates, maintenance, compatibility patches, or security vulnerability fixes. 

### 2. Regulatory Compliance & Non-Commercial Safe Harbor
This repository is made available strictly as a public, open-source archive for historical reference, research, and educational purposes. 
* **No Commercial Activity:** This software is provided entirely free of charge, outside the course of any commercial activity, and without any commercial support, monetization, or intent to place it on any commercial market.
* **User Responsibility:** Any downstream integration, deployment, or utilization of this code is performed entirely at the user's own risk. The user assumes full responsibility for ensuring compliance with all applicable cybersecurity regulations, including but not limited to the EU Cyber Resilience Act (CRA).

### 3. Limitation of Liability (MIT License Affirmation)
AS PER THE MIT LICENSE UNDER WHICH THIS CODE IS DISTRIBUTED, THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS, COPYRIGHT HOLDERS, OR CONTRIBUTORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Packages

| Package | Assembly | Description |
|---|---|---|
| `OpcComRcw` | `OpcComRcw.dll` | OPC Classic Runtime Callable Wrapper (RCW) for the OPC COM interfaces |
| `OpcNetApi` | `OpcNetApi.dll` | OPC Classic .NET API |
| `OpcNetApi.Com` | `OpcNetApi.Com.dll` | OPC Classic .NET API COM wrapper (depends on `OpcComRcw` and `OpcNetApi`) |

Each package multi-targets:

`netstandard2.0` · `netstandard2.1` · `net46` · `net6.0-windows` ·
`net8.0-windows` · **`net10.0-windows`**

> The `-windows` targets carry the COM interop surface; the `netstandard`
> targets exist for broad consumer compatibility.

## Prerequisites

- [.NET SDK 10.0](https://dotnet.microsoft.com/download) or later.

That's it. The `net46` target is satisfied by the
`Microsoft.NETFramework.ReferenceAssemblies` package (restored automatically),
and the reference packs for `net6`/`net8` are downloaded on demand by the SDK.

## Build

From this directory:

```powershell
.\build.ps1                                  # Release build + NuGet packages -> .\dist
.\build.ps1 -Version 2.1.131                 # set the package version
.\build.ps1 -Configuration Debug             # Debug build
.\build.ps1 -Clean                           # remove bin/obj/dist first
. .\signing-key.ps1; .\build.ps1             # with signing (see below)
```

Or build directly with the SDK:

```powershell
dotnet build DotNetRCWs.slnx -c Release
dotnet pack  DotNetRCWs.slnx -c Release -o .\dist
```

### Parameters

| Parameter | Default | Description |
|---|---|---|
| `-Version` | value in `version.txt` (`2.1.0`) | NuGet package version |
| `-Configuration` | `Release` | `Release` or `Debug` |
| `-OutDir` | `.\dist` | Output folder for `.nupkg` / `.snupkg` |
| `-SkipSign` | off | Skip Authenticode/Key Vault signing even when configured |
| `-Clean` | off | Remove `bin`/`obj` and the output folder before building |

## Signing

Signing follows the same model as the other tools in this repository. Copy the
values into `signing-key.ps1`, dot-source it, then build:

```powershell
. .\signing-key.ps1
.\build.ps1
```

Two independent mechanisms, each activated by environment variables set in
`signing-key.ps1`:

| Mechanism | Env variable(s) | Effect |
|---|---|---|
| **Strong-name** | `SigningKeyFile` | Path to the OPC `.snk`. When set, assemblies are strong-named with it; otherwise the placeholder key in `.\Keys` is used. |
| **Authenticode** (Azure Key Vault) | `SigningCertName`, `SigningClientId`, `SigningClientSecret`, `SigningTenantId`, `SigningVaultURL` (+ optional `SigningURL` timestamp) | When all five are set, binaries are signed with **AzureSignTool** and packages with **NuGetKeyVaultSignTool** (installed as global tools on demand). |

`signing-key.ps1` is committed with blank values as a template — fill it in
locally and never commit real secrets.

## Output

```
dist\
  OpcComRcw.<version>.nupkg
  OpcNetApi.<version>.nupkg
  OpcNetApi.Com.<version>.nupkg
  *.snupkg                       (symbol packages)
```

## Strong naming

The projects are strong-named with the **placeholder** key in `Keys\` by
default, which lets the assemblies build and load locally. To produce assemblies
with the official OPC Foundation identity, set `$env:SigningKeyFile` to the real
key in `signing-key.ps1` (see [Signing](#signing)).

## Layout

```
DotNetRCWs\
  Directory.Build.props     shared metadata, target frameworks, package settings
  DotNetRCWs.slnx           solution
  build.ps1                 build + pack script
  version.txt               default package version
  Keys\                     strong-name key (placeholder)
  nuget\                    package icon + license
  OpcComRcw\                RCW project
  OpcNetApi\                .NET API project
  OpcNetApi.Com\            COM wrapper project
```

