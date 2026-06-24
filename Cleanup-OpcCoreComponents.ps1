<#
.SYNOPSIS
    On-demand cleanup of leftover OPC Core Components files, COM registration, and
    SharedDLLs reference counts.

.DESCRIPTION
    Use this AFTER uninstalling OPC Core Components to scrub anything Windows
    Installer left behind. Two known cases it cleans up:

      1. On 64-bit Windows the 32-bit proxy/stub DLLs can be left in SysWOW64 after
         an uninstall (a side effect of how 32-bit system-folder components behave
         inside a 64-bit MSI / WIN64DUALFOLDERS).
      2. The broken 3.1.1.x release left a corrupt SharedDLLs reference count
         (0x80000000) on the x86 core DLLs.

    What it does (only for files that are actually present):
      - unregisters COM for the proxy/stub DLLs and the OpcEnum / OpcCategoryManager
        servers (so nothing is left pointing at a deleted file),
      - deletes the x86 proxy/stub DLLs + OpcEnum.exe from SysWOW64 / System32,
      - deletes the "OPC Foundation\Bin" and "OPC Foundation\Include" folders under
        both Common Files and Common Files (x86),
      - clears corrupt (high-bit) SharedDLLs counts for the x86 core DLLs.

    SAFETY: by default this REFUSES to run while an OPC Core Components product is
    still installed - those files belong to it. Uninstall first, or pass -Force.

    RECOVERY: everything removed here is fully restored by reinstalling the OPC Core
    Components redistributable(s). If a force-remove ever affects another product
    that shared these DLLs, repair/reinstall that product (msiexec /f <ProductCode>).

    Run ELEVATED.

.PARAMETER WhatIf
    Report what WOULD be removed; change nothing.

.PARAMETER Force
    Run even if an OPC Core Components product is still installed. Use only when you
    intend to scrub the machine and will reinstall/repair afterward.

.EXAMPLE
    powershell -NoProfile -ExecutionPolicy Bypass -File .\Cleanup-OpcCoreComponents.ps1 -WhatIf
.EXAMPLE
    powershell -NoProfile -ExecutionPolicy Bypass -File .\Cleanup-OpcCoreComponents.ps1
#>
[CmdletBinding()]
param([switch]$WhatIf, [switch]$Force)

$ErrorActionPreference = 'Continue'

# --- elevation ---------------------------------------------------------------
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) { Write-Warning "Not elevated. Re-run as Administrator (this writes to system folders and HKLM)."; if (-not $WhatIf) { exit 2 } }

# --- known locations / files -------------------------------------------------
$cf64   = $env:CommonProgramFiles
$cf86   = ${env:CommonProgramFiles(x86)}; if (-not $cf86) { $cf86 = $cf64 }
$sysDirs = @((Join-Path $env:windir 'SysWOW64'), (Join-Path $env:windir 'System32'))

# x86 core proxy/stub DLLs that land in the system folder (the ones that get orphaned)
$systemDlls = @('opccomn_ps.dll','opcproxy.dll','opc_aeps.dll','opcbc_ps.dll','opchda_ps.dll','opcsec_ps.dll')
$systemExes = @('OpcEnum.exe')
# all proxy/stub DLL names (for COM unregister wherever found)
$psDlls = $systemDlls + @('OpcCmdPs.dll','OpcDxPs.dll')
# OPC Foundation subtrees to delete wholesale (these belong only to this redistributable)
$opcSubtrees = @(
    (Join-Path $cf64 'OPC Foundation\Bin'),     (Join-Path $cf64 'OPC Foundation\Include'),
    (Join-Path $cf86 'OPC Foundation\Bin'),     (Join-Path $cf86 'OPC Foundation\Include')
)
$opcRoots = @((Join-Path $cf64 'OPC Foundation'), (Join-Path $cf86 'OPC Foundation'))

# --- is an OPC product still installed? (check the per-machine UpgradeCodes) --
function Get-Squished([string]$g) {
    $h = ($g -replace '[{}-]','').ToUpper()
    $rev  = { param($s) -join ($s.ToCharArray()[($s.Length-1)..0]) }
    $swap = { param($s) $o=''; for ($i=0;$i -lt $s.Length;$i+=2){ $o += $s[$i+1] + $s[$i] }; $o }
    (& $rev $h.Substring(0,8)) + (& $rev $h.Substring(8,4)) + (& $rev $h.Substring(12,4)) + (& $swap $h.Substring(16,4)) + (& $swap $h.Substring(20,12))
}
$upgradeCodes = @{ 'x86' = 'ABE618F1-0CCE-4F84-9124-65DB0EF16E00'; 'x64' = '282C4D0F-722D-4D30-B09C-61F6D4149DE0' }
$installed = @()
foreach ($k in $upgradeCodes.Keys) {
    if (Test-Path ("HKLM:\SOFTWARE\Classes\Installer\UpgradeCodes\" + (Get-Squished $upgradeCodes[$k]))) { $installed += $k }
}
if ($installed.Count -gt 0 -and -not $Force) {
    Write-Host ""
    Write-Host "OPC Core Components is still installed (platform: $($installed -join ', '))." -ForegroundColor Yellow
    Write-Host "These files belong to it - uninstall it first, or re-run with -Force to scrub anyway" -ForegroundColor Yellow
    Write-Host "(then reinstall/repair as needed)." -ForegroundColor Yellow
    exit 1
}

$tag = if ($WhatIf) { '[WhatIf] ' } else { '' }
Write-Host ""
Write-Host "${tag}OPC Core Components cleanup" -ForegroundColor Cyan
Write-Host ""
$removed = 0

# --- 1. unregister COM (best effort, only for present files) ------------------
Write-Host "1. Unregister COM servers" -ForegroundColor Cyan
$comTargets = @()
foreach ($d in ($sysDirs + $opcSubtrees)) { foreach ($f in $psDlls) { $p = Join-Path $d $f; if (Test-Path -LiteralPath $p) { $comTargets += $p } } }
foreach ($p in ($comTargets | Select-Object -Unique)) {
    if ($WhatIf) { Write-Host "   would: regsvr32 /u $p" }
    else { Start-Process regsvr32.exe -ArgumentList '/u','/s',"`"$p`"" -Wait -NoNewWindow; Write-Host "   regsvr32 /u $p" }
}
foreach ($d in ($sysDirs + $opcSubtrees)) {
    foreach ($exe in @('OpcEnum.exe','OpcCategoryManager.exe')) {
        $p = Join-Path $d $exe
        if (Test-Path -LiteralPath $p) {
            if ($WhatIf) { Write-Host "   would: $p /UnRegServer" }
            else { try { Start-Process $p -ArgumentList '/UnRegServer' -Wait -NoNewWindow -ErrorAction Stop; Write-Host "   $exe /UnRegServer" } catch {} }
        }
    }
}
# OpcEnum is a service - make sure it's stopped/removed before deleting the exe
$svc = Get-Service -Name 'OpcEnum' -ErrorAction SilentlyContinue
if ($svc) {
    if ($WhatIf) { Write-Host "   would: stop+delete service OpcEnum" }
    else { try { Stop-Service OpcEnum -Force -ErrorAction SilentlyContinue; sc.exe delete OpcEnum | Out-Null; Write-Host "   stopped + deleted service OpcEnum" } catch {} }
}

# --- 2. delete the orphaned system DLLs/exes ---------------------------------
Write-Host "2. Remove x86 system-folder files" -ForegroundColor Cyan
foreach ($d in $sysDirs) {
    foreach ($f in ($systemDlls + $systemExes)) {
        $p = Join-Path $d $f
        if (Test-Path -LiteralPath $p) {
            if ($WhatIf) { Write-Host "   would delete $p" }
            else { try { Remove-Item -LiteralPath $p -Force -ErrorAction Stop; Write-Host "   deleted $p" -ForegroundColor Green; $removed++ }
                   catch { Write-Host "   FAILED $p : $($_.Exception.Message)" -ForegroundColor Red } }
        }
    }
}

# --- 3. delete the OPC Foundation Bin/Include subtrees -----------------------
Write-Host "3. Remove OPC Foundation\Bin and \Include folders" -ForegroundColor Cyan
foreach ($p in $opcSubtrees) {
    if (Test-Path -LiteralPath $p) {
        if ($WhatIf) { Write-Host "   would delete $p" }
        else { try { Remove-Item -LiteralPath $p -Recurse -Force -ErrorAction Stop; Write-Host "   deleted $p" -ForegroundColor Green; $removed++ }
               catch { Write-Host "   FAILED $p : $($_.Exception.Message)" -ForegroundColor Red } }
    }
}
foreach ($p in $opcRoots) {
    if ((Test-Path -LiteralPath $p) -and -not $WhatIf) {
        if (-not (Get-ChildItem -LiteralPath $p -Force -ErrorAction SilentlyContinue)) { Remove-Item -LiteralPath $p -Force -ErrorAction SilentlyContinue; Write-Host "   removed empty $p" }
    }
}

# --- 4. clear corrupt SharedDLLs counts --------------------------------------
Write-Host "4. Clear corrupt SharedDLLs counts (high-bit values)" -ForegroundColor Cyan
foreach ($vw in [Microsoft.Win32.RegistryView]::Registry64, [Microsoft.Win32.RegistryView]::Registry32) {
    try {
        $base = [Microsoft.Win32.RegistryKey]::OpenBaseKey([Microsoft.Win32.RegistryHive]::LocalMachine, $vw)
        $k = $base.OpenSubKey('SOFTWARE\Microsoft\Windows\CurrentVersion\SharedDLLs', -not $WhatIf)
        if ($k) {
            foreach ($d in $sysDirs) { foreach ($f in $systemDlls) {
                $n = "$d\$f"; $v = $k.GetValue($n, $null)
                if ($null -ne $v -and ((([int64]$v) -band 0x80000000) -ne 0)) {
                    if ($WhatIf) { Write-Host "   would clear [$vw] $n (=$v)" }
                    else { $k.DeleteValue($n, $false); Write-Host "   cleared [$vw] $n" -ForegroundColor Green; $removed++ }
                }
            } }
            $k.Close()
        }
        $base.Close()
    } catch { Write-Host "   [$vw] $($_.Exception.Message)" -ForegroundColor Red }
}

Write-Host ""
if ($WhatIf) { Write-Host "WhatIf: no changes made. Re-run without -WhatIf to apply." -ForegroundColor Yellow }
else { Write-Host "Done. Removed/cleared $removed item(s). Reinstall the redistributable to restore if needed." -ForegroundColor Cyan }
Write-Host ""
