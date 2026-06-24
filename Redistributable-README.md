# OPC Core Components Redistributable

These are the OPC Foundation Core Components — the COM proxy/stub DLLs, the OPC
server enumerator (`OpcEnum`), and the component category manager that OPC Classic
(COM/DCOM) clients and servers need to communicate.

## What's in this package

| File | Purpose |
|---|---|
| `opc-core-components-redistributable-<version>-x86-x64.msi` | **Install this on 64-bit Windows.** A combined installer that delivers BOTH the 64-bit and 32-bit components. |
| `opc-core-components-redistributable-<version>-x86.msi` | Install this on 32-bit Windows only. |
| `opc-com-proxystub-mergemodule-<version>-{x86,x64}.msm` | Merge modules for embedding the runtime in your own installer. |
| `opc-com-sdk-mergemodule-<version>-{x86,x64}.msm` | Merge modules with headers/IDLs/reference DLLs for developers. |
| `Cleanup-OpcCoreComponents.ps1` | On-demand cleanup utility (see below). |

## Installing

- **64-bit Windows:** run the **x86-x64** MSI. It contains the 32-bit components too,
  so a single install provides both 32-bit and 64-bit COM support.
- **32-bit Windows:** run the **x86** MSI.

Both require administrator privileges. Installing the latest version automatically
upgrades any previous OPC Core Components release.

## Why `Cleanup-OpcCoreComponents.ps1` is needed

Two known issues can leave files or registry data behind if version 3.1.1 was installed on
a machine. The issues are not present if version 3.1.1 was never installed (i.e. <=3.0.X or >=3.1.2). 
The cleanup script remediates both, on demand. 

1. **32-bit DLLs left in `SysWOW64` after uninstalling on 64-bit Windows.**
   The 32-bit proxy/stub DLLs install into the system folder. When they are
   delivered by the combined 64-bit MSI, Windows Installer's WIN64DUALFOLDERS
   behavior resolves those 32-bit system-folder components to "no action" on
   uninstall, so the DLL *files* are left in `C:\Windows\SysWOW64` even though
   their COM registration is correctly removed. This is a cosmetic file leftover,
   not a functional problem. (It cannot be fixed reliably inside the MSI without
   risking clean installs — so it is handled here instead.)

2. **Corrupt SharedDLLs reference counts from the broken 3.1.1.89 release.**
   The 3.1.1.89 build wrote an invalid SharedDLLs reference count on the six
   core 32-bit DLLs. Windows Installer honors that count when deciding whether to
   remove a shared file, which can prevent later uninstalls from deleting them.

### What the script does

For files/values that are actually present, it:

- unregisters COM for the proxy/stub DLLs and the `OpcEnum` / `OpcCategoryManager`
  servers,
- deletes the orphaned 32-bit proxy/stub DLLs and `OpcEnum.exe` from `SysWOW64` /
  `System32`,
- removes the `OPC Foundation\Bin` and `OPC Foundation\Include` folders under
  Common Files,
- clears corrupt (high-bit) SharedDLLs reference counts.

**Safety:** by default it refuses to run while an OPC Core Components product is
still installed (those files belong to the product). Uninstall first, or pass
`-Force`.

**Recovery:** everything it removes is fully restored by reinstalling this
redistributable. If a removal ever affects another product that shared these DLLs,
repair that product (`msiexec /f <ProductCode>`).

### Running it

Open an **elevated** PowerShell (Run as administrator):

```powershell
# Preview what would be removed (changes nothing):
powershell -NoProfile -ExecutionPolicy Bypass -File .\Cleanup-OpcCoreComponents.ps1 -WhatIf

# Perform the cleanup:
powershell -NoProfile -ExecutionPolicy Bypass -File .\Cleanup-OpcCoreComponents.ps1
```

## License

See `LICENSE.md` included in this package.
