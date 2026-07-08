# OPC Classic .NET API (Redistributable)

Managed .NET assemblies and NuGet packages for OPC Classic (COM/DCOM) clients
and servers. This is the **.NET** side of the OPC Classic Core Components — the
Runtime Callable Wrapper (RCW) for the OPC COM interfaces plus the OPC Classic
.NET API and its COM wrapper.

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

## Contents

```
NuGet.Config                    local package source pointed at .\packages
LICENSE.md                      OPC Foundation MIT License
redistributable-README.md       this file
packages\
  OpcComRcw.<version>.nupkg
  OpcNetApi.<version>.nupkg
  OpcNetApi.Com.<version>.nupkg
```

## Using the packages

The bundled `NuGet.Config` registers the `packages\` folder as a local package
source (alongside nuget.org, which supplies the transitive dependencies). Place
`NuGet.Config` next to your solution — adjusting the `packages` path if you move
the folder — then add a reference:

```powershell
dotnet add package OpcNetApi.Com --version <version>
```

`OpcNetApi.Com` pulls in `OpcComRcw` and `OpcNetApi` automatically.

## License

Distributed under the OPC Foundation MIT License — see `LICENSE.md`.
