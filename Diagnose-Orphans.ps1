<#
.SYNOPSIS
    Diagnose why files are left behind after uninstalling, specifically when
    upgrading from the broken 3.1.1.89 release.

.DESCRIPTION
    Run this ELEVATED on the VM in its current (orphaned) state, i.e. after:
        install 3.1.1  ->  install 3.1.2  ->  uninstall 3.1.2
    It reports:
      1. Whether the OLD 3.1.1 component GUIDs (A1B2C3D4-... / B2C3D4E5-...) are
         still registered in the Windows Installer component database, and what
         key-path each still claims. A lingering registration is what keeps the
         files on disk after our product is removed.
      2. SharedDLLs reference counts for the runtime DLLs.
      3. Which OPC files are actually present on disk, and their versions.

    Nothing is modified - this is read only.

.EXAMPLE
    powershell -NoProfile -ExecutionPolicy Bypass -File "C:\Work\Diagnose-Orphans.ps1"
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Continue'

# --- MSI packed/squished GUID -------------------------------------------------
function Get-Squished([string]$g) {
    $h = ($g -replace '[{}-]','').ToUpper()
    $rev  = { param($s) -join ($s.ToCharArray()[($s.Length-1)..0]) }
    $swap = { param($s) $o=''; for ($i=0; $i -lt $s.Length; $i+=2) { $o += $s[$i+1] + $s[$i] }; $o }
    (& $rev $h.Substring(0,8)) + (& $rev $h.Substring(8,4)) + (& $rev $h.Substring(12,4)) +
    (& $swap $h.Substring(16,4)) + (& $swap $h.Substring(20,12))
}

$ComponentsRoot = 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Installer\UserData\S-1-5-18\Components'

function Get-Reg([string]$guid) {
    $key = Join-Path $ComponentsRoot (Get-Squished $guid)
    if (-not (Test-Path -LiteralPath $key)) { return @{ Present=$false; Clients=0; KeyPaths=@() } }
    try { $p = Get-ItemProperty -LiteralPath $key -ErrorAction Stop } catch { return @{ Present=$true; Clients=0; KeyPaths=@() } }
    $vals = @($p.PSObject.Properties | Where-Object { $_.Name -notlike 'PS*' })
    @{ Present=$true; Clients=$vals.Count; KeyPaths=@($vals | ForEach-Object { $_.Value }) }
}

# --- OLD 3.1.1 (broken) component GUIDs --------------------------------------
# Proxy/stub module - x86 AND x64 shared this SAME set (that was the ICE08 bug).
$old_proxystub = [ordered]@{
    'opccomn_ps'         = 'A1B2C3D4-0001-4000-8000-000000000001'
    'opcproxy'           = 'A1B2C3D4-0002-4000-8000-000000000002'
    'opc_aeps'           = 'A1B2C3D4-0003-4000-8000-000000000003'
    'opcbc_ps'           = 'A1B2C3D4-0004-4000-8000-000000000004'
    'OpcCmdPs'           = 'A1B2C3D4-0005-4000-8000-000000000005'
    'OpcDxPs'            = 'A1B2C3D4-0006-4000-8000-000000000006'
    'opchda_ps'          = 'A1B2C3D4-0007-4000-8000-000000000007'
    'opcsec_ps'          = 'A1B2C3D4-0008-4000-8000-000000000008'
    'OpcEnum'            = 'A1B2C3D4-0009-4000-8000-000000000009'
    'OpcCategoryManager' = 'A1B2C3D4-000A-4000-8000-000000000010'
}
# SDK module - shared set too. Pattern: B2C3D4E5-XXXX-4000-9000-00000000XXXX
$old_sdk = [ordered]@{}
foreach ($i in (@(1..0x25) + @(0x30..0x37))) {
    $hx = '{0:X4}' -f $i
    $old_sdk["sdk_$hx"] = "B2C3D4E5-$hx-4000-9000-00000000$hx"
}

# --- runtime file inventory ---------------------------------------------------
$cf64 = $env:CommonProgramFiles
$cf86 = ${env:CommonProgramFiles(x86)}; if (-not $cf86) { $cf86 = $cf64 }
$dirs = [ordered]@{
    'System32'              = (Join-Path $env:windir 'System32')
    'SysWOW64'              = (Join-Path $env:windir 'SysWOW64')
    'CF64\OPCF\Bin'         = (Join-Path $cf64 'OPC Foundation\Bin')
    'CF86\OPCF\Bin'         = (Join-Path $cf86 'OPC Foundation\Bin')
    'CF64\OPCF\Include'     = (Join-Path $cf64 'OPC Foundation\Include')
    'CF86\OPCF\Include'     = (Join-Path $cf86 'OPC Foundation\Include')
}
$runtimeFiles = @('opccomn_ps.dll','opcproxy.dll','opc_aeps.dll','opcbc_ps.dll','opchda_ps.dll',
                  'opcsec_ps.dll','OpcCmdPs.dll','OpcDxPs.dll','OpcEnum.exe','OpcCategoryManager.exe')

# =============================================================================
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) { Write-Warning "Not elevated - HKLM reads may be incomplete. Re-run as Administrator." }

Write-Host "`n=== 1. Lingering 3.1.1 PROXY/STUB component registrations ===" -ForegroundColor Cyan
$lingerPS = 0
foreach ($k in $old_proxystub.Keys) {
    $r = Get-Reg $old_proxystub[$k]
    if ($r.Present) {
        $lingerPS++
        Write-Host ("  [LINGERS] {0,-20} clients={1}  keypath={2}" -f $k, $r.Clients, ($r.KeyPaths -join ' | ')) -ForegroundColor Red
    } else {
        Write-Host ("  [clean]   {0}" -f $k) -ForegroundColor DarkGray
    }
}

Write-Host "`n=== 2. Lingering 3.1.1 SDK component registrations ===" -ForegroundColor Cyan
$lingerSdk = 0
foreach ($k in $old_sdk.Keys) {
    $r = Get-Reg $old_sdk[$k]
    if ($r.Present) {
        $lingerSdk++
        Write-Host ("  [LINGERS] {0,-12} {1}  clients={2}  keypath={3}" -f $k, $old_sdk[$k], $r.Clients, ($r.KeyPaths -join ' | ')) -ForegroundColor Red
    }
}
if ($lingerSdk -eq 0) { Write-Host "  (none of the 45 SDK GUIDs are registered)" -ForegroundColor DarkGray }

Write-Host "`n=== 3. SharedDLLs reference counts ===" -ForegroundColor Cyan
$sd = Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\SharedDLLs' -ErrorAction SilentlyContinue
$sharedHits = 0
foreach ($d in $dirs.GetEnumerator()) {
    foreach ($f in $runtimeFiles) {
        $p = Join-Path $d.Value $f
        $rc = if ($sd) { $sd.$p } else { $null }
        if ($null -ne $rc) {
            $sharedHits++
            Write-Host ("  refcount={0}  {1}" -f $rc, $p) -ForegroundColor Yellow
        }
    }
}
if ($sharedHits -eq 0) { Write-Host "  (no SharedDLLs entries for any OPC runtime file)" -ForegroundColor DarkGray }

Write-Host "`n=== 4. OPC files actually present on disk ===" -ForegroundColor Cyan
$fileHits = 0
foreach ($d in $dirs.GetEnumerator()) {
    foreach ($f in $runtimeFiles) {
        $p = Join-Path $d.Value $f
        if (Test-Path -LiteralPath $p) {
            $fileHits++
            $ver = (Get-Item -LiteralPath $p -ErrorAction SilentlyContinue).VersionInfo.FileVersion
            Write-Host ("  [{0,-18}] {1,-24} ver={2}" -f $d.Key, $f, $ver)
        }
    }
}
if ($fileHits -eq 0) { Write-Host "  (no OPC runtime files present - disk is clean)" -ForegroundColor Green }

Write-Host "`n=== VERDICT ===" -ForegroundColor Cyan
Write-Host ("  Lingering 3.1.1 proxy/stub registrations: {0}" -f $lingerPS)
Write-Host ("  Lingering 3.1.1 SDK registrations:        {0}" -f $lingerSdk)
Write-Host ("  SharedDLLs entries for OPC files:         {0}" -f $sharedHits)
Write-Host ("  OPC files left on disk:                   {0}" -f $fileHits)
Write-Host ""
if ($lingerPS -gt 0 -or $lingerSdk -gt 0) {
    Write-Host "=> A lingering 3.1.1 component registration is holding the files." -ForegroundColor Yellow
    Write-Host "   Fix: installer cleanup of these specific old GUIDs + their files." -ForegroundColor Yellow
} elseif ($sharedHits -gt 0) {
    Write-Host "=> SharedDLLs refcount is keeping the files (no MSI owner)." -ForegroundColor Yellow
    Write-Host "   Fix: clear/decrement those refcounts + RemoveFile on install." -ForegroundColor Yellow
} elseif ($fileHits -gt 0) {
    Write-Host "=> Plain orphan files, no MSI owner and no refcount." -ForegroundColor Yellow
    Write-Host "   Fix: RemoveFile-on-install in the x86 components is safe + sufficient." -ForegroundColor Yellow
} else {
    Write-Host "=> Disk is clean - nothing to do." -ForegroundColor Green
}
Write-Host ""
