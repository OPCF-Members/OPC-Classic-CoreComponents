# Merge Module Test Harness

A minimal consumer MSI that embeds the OPC merge modules (`MergeModule.wxs` +
`MergeModuleSdk.wxs`) so you can install/uninstall them **in isolation** on a
clean VM — without the production installer's UI, upgrade logic, or combined-x64
bundling. Use it to confirm the modules leave no leftover files, services, or COM
registration, and don't throw custom-action errors (e.g. the error 2753 that fired
when `OpcEnum` / `OpcCategoryManager` lived in an unselected optional feature).

## Files

| File | Purpose |
|---|---|
| `MsmHarness.wxs` | The harness package. Embeds both merge modules, each in its own optional (`Level="2"`) feature. |
| `Build-MsmHarness.ps1` | Rebuilds the `.msm` from current `WiX\*.wxs` source and wraps them in `out\opc-msm-harness-<arch>.msi`. |
| `Run-HarnessTest.ps1` | Elevated install/uninstall test matrix with pass/fail checks. |

## Prerequisites

1. WiX 4+ (`dotnet tool install --global wix`).
2. The DLLs must already be built — run `.\build.ps1` at the repo root once so
   `out\<arch>\bin` and `out\<arch>\include` are populated. The harness reuses
   those binaries; it does not recompile.

## Build

```powershell
# From the repo root or the test folder:
.\test\Build-MsmHarness.ps1            # both arches
.\test\Build-MsmHarness.ps1 -Platform x64
```

Produces `test\out\opc-msm-harness-x86.msi` and/or `...-x64.msi`. The `.msm` are
rebuilt from the **current** source, so the harness always reflects the latest
`WiX\*.wxs`.

## Test on a VM

Copy the `test\` folder to a clean VM, open an **elevated** PowerShell, and run:

```powershell
.\Run-HarnessTest.ps1 -Platform x64
.\Run-HarnessTest.ps1 -Platform x86
```

It runs three scenarios, each install **and** uninstall, scanning the verbose
logs for error 2753 and checking for leftover files/services:

| Scenario | Feature selection | Expectation |
|---|---|---|
| Nothing optional | (default — nothing) | installs/uninstalls clean, no 2753 |
| Everything | `ADDLOCAL=ProxyStub,Sdk` | components register on install, fully removed on uninstall |
| SDK only | `ADDLOCAL=Sdk` (proxy/stub OFF) | **the 2753 regression case** — must install clean |

### Manual invocation

```powershell
msiexec /i out\opc-msm-harness-x64.msi /qn /l*v inst.log                  # nothing
msiexec /i out\opc-msm-harness-x64.msi ADDLOCAL=ProxyStub,Sdk /qn /l*v inst.log  # everything
msiexec /i out\opc-msm-harness-x64.msi ADDLOCAL=Sdk /qn /l*v inst.log      # SDK only (2753 case)
msiexec /x out\opc-msm-harness-x64.msi /qn /l*v unin.log                   # uninstall
```

## Note

This harness is a **native per-arch** MSI, so it does **not** reproduce the
WIN64DUALFOLDERS SysWOW64 file leak that the production *combined-x64* installer
exhibits by design. A fully clean uninstall is expected here; that leak (and its
`Cleanup-OpcCoreComponents.ps1` remedy) is a property of the combined package
only.
