<#
.SYNOPSIS
    Verify OPC Core Components install state on a clean VM.

.DESCRIPTION
    Two modes:
      -Mode Clean      Assert NONE of the expected artifacts are present
                       (run on a pristine VM, or after uninstall, to prove a
                       clean machine / clean removal).
      -Mode Installed  Assert ALL expected artifacts ARE present
                       (run after installing the MSI).

    Artifacts checked:
      1. Windows Installer component registrations (squished component GUIDs under
         HKLM\...\Installer\UserData\S-1-5-18\Components). This is the
         authoritative source of truth - the component GUID list below is the
         exact set baked into the 3.1.2 merge modules.
      2. The key-path file recorded by each component actually exists on disk
         (Installed mode).
      3. The OPC Foundation\Bin and \Include folders (Common Files / Common
         Files (x86)).
      4. The x86 proxy/stub DLLs in System32 / SysWOW64.
      5. The OpcEnum Windows service.
      6. The product UpgradeCode registration.

    -Product selects which package's artifacts to expect:
      x86   standalone x86 MSI  -> x86 runtime (+ x86 SDK with -IncludeSdk)
      x64   combined x64 MSI    -> x64 runtime + x86 runtime (the combined MSI
                                   bundles the x86 merge module)
                                   (+ x64 SDK + x86 SDK with -IncludeSdk)
      both  both products installed (same artifact set as x64; component refcounts
            will be higher - see the reported "clients" count)

    NOTE: the SDK (DevEnvironment) feature is Level 2 and is NOT installed by a
    default silent install. Only pass -IncludeSdk if the install selected it
    (e.g. ADDLOCAL=... or INSTALLLEVEL>=2).

.EXAMPLE
    # On a pristine VM, before installing anything:
    .\Check-Installation.ps1 -Mode Clean -Product x64

.EXAMPLE
    # After: msiexec /i opc-core-...-x86-x64.msi /qn /l*v inst.log
    .\Check-Installation.ps1 -Mode Installed -Product x64

.EXAMPLE
    # Full install incl. SDK:
    .\Check-Installation.ps1 -Mode Installed -Product x64 -IncludeSdk

.NOTES
    Run elevated (reads HKLM UserData / writes nothing). Exit code 0 = all
    assertions passed, 1 = one or more failed.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidateSet('Clean','Installed')] [string]$Mode,
    [ValidateSet('x86','x64','both')] [string]$Product = 'x64',
    [switch]$IncludeSdk
)

# No Set-StrictMode: this tool deliberately tolerates $null/absent registry
# reads (a key not existing is a normal, expected result), and continues past
# individual read errors so it can report a full picture.
$ErrorActionPreference = 'Continue'

# ---------------------------------------------------------------------------
# Component GUIDs baked into the 3.1.2 merge modules (keep in sync with
# WiX\MergeModule.wxs and WiX\MergeModuleSdk.wxs).
# ---------------------------------------------------------------------------
$X86_RUNTIME = [ordered]@{
    'opccomn_ps.dll (System)' = '1902BC04-D8CF-67CE-FC28-9C8635C5C2D0'
    'opcproxy.dll (System)'   = '3C257474-772A-6CE8-282E-7F4413643C2C'
    'opc_aeps.dll (System)'   = 'CD1FB489-1F99-28C2-058C-D7D3E7740F3B'
    'opcbc_ps.dll (System)'   = '531CABFE-03FC-439E-EB24-DEBB3AD7792B'
    'opchda_ps.dll (System)'  = '69CC9D6E-4B05-F7A4-3D9B-5D771F36DB4E'
    'opcsec_ps.dll (System)'  = '2F6FBEF4-D9E4-74B8-2B97-CC0C401D052A'
    'OpcEnum.exe (System)'    = '4695E177-FFA9-6706-9772-290B37AD1438'
    'OpcCmdPs.dll (Bin)'      = 'B9011D62-73A6-23AF-A2D1-F979014F853B'
    'OpcDxPs.dll (Bin)'       = '8D21BDBF-0135-5954-A9BF-4911EAD28B2B'
}
$X64_RUNTIME = [ordered]@{
    'opccomn_ps.dll (Bin64)'        = '3B994FAF-50B0-664A-7327-84C53151DC3A'
    'opcproxy.dll (Bin64)'          = 'A930A8B0-CD04-98D6-D8CC-28B2B86C6FEA'
    'opc_aeps.dll (Bin64)'          = 'BF39502E-19E8-0AF9-6A17-33FCB8556882'
    'opcbc_ps.dll (Bin64)'          = '8EF6E68D-0D6E-E29C-8521-885632687BCD'
    'OpcCmdPs.dll (Bin64)'          = '2A707F0E-D9FE-9DAE-01F0-C7598CB77D14'
    'OpcDxPs.dll (Bin64)'           = '8B1F066C-7B26-4193-526A-96570BF904EE'
    'opchda_ps.dll (Bin64)'         = '2084B775-59CC-B88F-99BF-B82FEFD263F4'
    'opcsec_ps.dll (Bin64)'         = 'F59E37D7-AA01-9651-122D-AD3ECBCFEBDB'
    'OpcCategoryManager.exe (Bin64)'= '39F22912-1A3C-C44E-EF19-778291394A31'
}
$X86_SDK = [ordered]@{
    'opccomn.h'='7507D67A-6018-E8EB-1CD6-E1CE564C978F'; 'opccomn_i.c'='FC5DDE9B-1B69-3FA1-9A05-5819297499A0'; 'opccomn.idl'='437098B9-1FF5-5045-171F-54E7F014C05E'
    'opcda.h'='98650C5E-A78A-8E3B-E9BF-74425A2523CB'; 'opcda_i.c'='DE3E9DCA-8685-2711-A3E0-D6992905EFC2'; 'opcda.idl'='78A0AB98-5D59-88FA-A938-6724426AF59E'
    'opcerror.h'='2BA5E564-3C7A-68BA-ECA3-F4AF64327600'
    'opc_ae.h'='7AAD8B0C-B0BC-A152-4052-9235ECC79768'; 'opc_ae_i.c'='FE1680B9-B277-C59F-67A5-A2D2D2350FEB'; 'opc_ae.idl'='5609747B-3EA7-2CDD-F9A2-29BEFC6D600C'
    'opcae_er.h'='EE644669-F463-6291-2F61-2E11A4CEDB32'; 'opcaedef.h'='9E442E7A-B4A3-8A95-BCA8-B520E9BA19F9'
    'opcbc.h'='B8F3EBA7-FA7A-180D-23BD-709BA81D1E53'; 'opcbc_i.c'='8879318F-EB22-4837-CA35-2BA3A3BBA852'; 'opcbc.idl'='71B2FAB5-D384-ADA9-8FB4-6B893E699E97'
    'OpcBatchError.h'='B3F6AFF3-BA17-DF76-714A-C42AB633BB7C'; 'OpcBatchProps.h'='58B36504-82CA-8949-98FD-CE736D3CAA7D'; 'OpcBatchDef.h'='FB029D0B-3A45-BA8F-6167-666FC00AFA3F'
    'OpcCmd.h'='B0E2A135-56D2-89EE-42BD-9279654CAAA5'; 'OpcCmd_i.c'='45CAF99A-BB9C-FEDB-0E24-92EC47AC3050'; 'OpcCmd.idl'='19F675EE-EA1D-CDF4-8BE2-35C964EE6DEF'; 'OpcCmdError.h'='82A63252-867B-063D-2EF7-7976827F5F63'
    'OpcDx.h'='88DB562F-9255-0368-BCF9-A31BFAF52776'; 'OpcDx_i.c'='2D39136E-E47A-EDDA-9D3B-EFFB977A83F3'; 'OpcDx.idl'='C05DEF7B-BB81-73EB-6C55-CED99F6285EE'; 'OpcDxError.h'='7129755B-A3D1-684C-8467-0C48AACB2151'
    'opchda.h'='E60D79A5-8932-6400-E1FD-CDF498082F21'; 'opchda_i.c'='91B572AE-CCBF-F76F-EF7F-415188C252CE'; 'opchda.idl'='CFA68802-F066-718B-1F67-971680525F1E'; 'OpcHda_Error.h'='CDFE06C4-DDB8-92CC-50ED-7CC0A9566865'
    'opcSec.h'='FE683149-4C48-983C-F073-F07A8F0F3FD9'; 'opcSec_i.c'='02AEA27C-96A4-15D5-9FCF-F0D49E156F33'; 'opcSec.idl'='4ECECD82-0B14-B53F-6E0A-CF617759A6ED'; 'OpcErrSec.h'='B616BD4B-5664-795C-0F75-BBEDAD514EAB'
    'OpcEnum.h'='855E275C-7A78-11F9-9B02-7D40F9EDBCC0'; 'OpcEnum_i.c'='4EDBA834-ADD4-353A-B0D7-04FFC8746D20'; 'OpcEnum.idl'='C7D97234-6F09-1E7B-F97E-A2B399FA96C6'
    'opccomn_ps.dll (Inc)'='0E523F66-985E-DCE5-EA01-4554BB5C6BD5'; 'opcproxy.dll (Inc)'='EDDB2CE7-1D53-2CFA-B4F4-340E3C28DB43'; 'opc_aeps.dll (Inc)'='8D0A046B-91CC-0EE3-3ED1-373ED6F43383'; 'opcbc_ps.dll (Inc)'='7F7DF0C9-F3E1-39A9-3A2D-6914B3DA2B2F'
    'OpcCmdPs.dll (Inc)'='C3D484BD-B570-D103-36B6-3805E92D66EC'; 'OpcDxPs.dll (Inc)'='F440E3B6-1079-206D-29F9-BE3870968F79'; 'opchda_ps.dll (Inc)'='4BAE66D4-DF99-EA23-2B34-5439F587C62E'; 'opcsec_ps.dll (Inc)'='01F4C181-D968-B1FA-5495-FA9B2CAF54F4'
}
$X64_SDK = [ordered]@{
    'opccomn.h'='97A81043-A06A-40F6-9437-2BE8E47F71A9'; 'opccomn_i.c'='902D1E91-F112-4782-9016-DBA8E3AABD88'; 'opccomn.idl'='11664897-BB09-4CA2-BFE5-5C43095D4D70'
    'opcda.h'='3A782046-CC8F-47C1-B1E1-F8AFC681FF27'; 'opcda_i.c'='1F2D6705-D90A-4459-A231-0FE486BD3F96'; 'opcda.idl'='FA14F33C-8AAD-48E0-B0A0-74671AD8C10C'
    'opcerror.h'='62C1E837-B73B-4E95-9FE3-3EDB1D69F841'
    'opc_ae.h'='3E1CFE69-08DC-4914-9C68-0FCA9BB5904B'; 'opc_ae_i.c'='08CBA1CE-6EB2-4388-8A90-EF63160EC8CA'; 'opc_ae.idl'='6DC5C505-2AB8-41D6-AC5E-B9575EB29C30'
    'opcae_er.h'='BB221E76-586A-4525-9167-3504B2A3B09A'; 'opcaedef.h'='0D2B8535-3990-4E7F-83E0-92E982F73DEC'
    'opcbc.h'='DDE9F3E3-D95F-4B87-B20E-11FF80F47FCE'; 'opcbc_i.c'='D69596A4-F49B-4F06-992E-E0540FE141C0'; 'opcbc.idl'='82AB88D9-21B7-4198-8D2B-181AE65F8A66'
    'OpcBatchError.h'='758D4F8F-D173-4911-8ABC-0851AD26B436'; 'OpcBatchProps.h'='A27EAAFA-5F81-43C7-BD55-B4241FA48B19'; 'OpcBatchDef.h'='D9A6F681-AEF7-44B3-9F3B-5DE658BB64C3'
    'OpcCmd.h'='102D644C-FD0A-468E-A2F0-F111C2911A57'; 'OpcCmd_i.c'='12B56932-65C7-4AD1-9177-F8E207B68BFF'; 'OpcCmd.idl'='FA50913B-4330-4A13-BA58-C1D3A078459A'; 'OpcCmdError.h'='4BDAB62A-B272-4D5A-AD17-03B76B10DDE2'
    'OpcDx.h'='244ECED5-0F38-44F4-B44A-A2CB6CCF283E'; 'OpcDx_i.c'='FE9D9F3D-6AD0-43F7-B7A8-087E06F5CFE1'; 'OpcDx.idl'='09D8B011-4AC4-469F-938A-8B5C0A4BFA20'; 'OpcDxError.h'='1DD93891-8ED9-439E-A696-49DD0E081C1C'
    'opchda.h'='F8FFF911-59CD-420F-B887-B3B559F4DB23'; 'opchda_i.c'='9CFC011C-2C8D-42D6-8BED-E03FB0B960C9'; 'opchda.idl'='DF4582E7-7CE0-4915-A1BD-48DE28138B67'; 'OpcHda_Error.h'='E68C2986-DAF1-43F2-AC99-98B7E620C0C9'
    'opcSec.h'='BB6C49E9-E311-4DE8-A082-53DF1F5639E2'; 'opcSec_i.c'='3BB7968B-9E95-4740-94F9-01B58A11B9BF'; 'opcSec.idl'='8F44D991-9FBE-4060-92D8-217E37CE3778'; 'OpcErrSec.h'='07D6FC5B-6872-4161-BA20-B7BA6668138C'
    'opccomn_ps.dll (Inc)'='5B4FE3FD-AC66-4F3C-AA55-C0C042276AD1'; 'opcproxy.dll (Inc)'='C0433505-3B6C-430A-8024-994726AADF16'; 'opc_aeps.dll (Inc)'='15412DF1-EFB0-4F93-9C9F-F330388341A2'; 'opcbc_ps.dll (Inc)'='B888A532-5C0B-4A61-A615-23BD344D30D2'
    'OpcCmdPs.dll (Inc)'='ED371067-129A-43FE-A2A7-F813C64A20C2'; 'OpcDxPs.dll (Inc)'='980A853D-5F4E-4868-B080-55BFFEC215B5'; 'opchda_ps.dll (Inc)'='AFA92F67-2A37-4849-B5C9-B10328B65D47'; 'opcsec_ps.dll (Inc)'='9D2F8E7A-08F3-450A-A6AA-AD1CAA6255BD'
}

$UPGRADE_X86 = 'ABE618F1-0CCE-4F84-9124-65DB0EF16E00'
$UPGRADE_X64 = '282C4D0F-722D-4D30-B09C-61F6D4149DE0'
$OPCENUM_CLSID = '13486D44-4821-11D2-A494-3CB306C10000'   # well-known OpcEnum CLSID (informational)
$X86_SYSTEM_DLLS = @('opccomn_ps.dll','opcproxy.dll','opc_aeps.dll','opcbc_ps.dll','opchda_ps.dll','opcsec_ps.dll','OpcEnum.exe')

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

# MSI "squished"/packed GUID: reverse first 3 groups, byte-swap the last two.
function Get-SquishedGuid([string]$guid) {
    $h = ($guid -replace '[{}-]','').ToUpper()
    if ($h.Length -ne 32) { throw "Bad GUID: $guid" }
    $rev = { param($s) -join ($s.ToCharArray()[($s.Length-1)..0]) }
    $swap = { param($s) $o=''; for($i=0;$i -lt $s.Length;$i+=2){ $o += $s[$i+1] + $s[$i] }; $o }
    (& $rev $h.Substring(0,8)) + (& $rev $h.Substring(8,4)) + (& $rev $h.Substring(12,4)) +
    (& $swap $h.Substring(16,4)) + (& $swap $h.Substring(20,12))
}

$ComponentsRoot = 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Installer\UserData\S-1-5-18\Components'
$UpgradeRoot    = 'HKLM:\SOFTWARE\Classes\Installer\UpgradeCodes'

# Robustly confirm a component's key-path file exists. The keypath string read
# from the Components registry can contain non-printable/encoded characters
# (e.g. "C:\?WINDOWS\SysWOW64\..."), so we try the raw value, a sanitized value
# (invalid path chars stripped), and finally the bare filename in the standard
# install dirs ($CandidateDirs is set in the resolve section below).
function Test-KeyPathFile([string[]]$paths) {
    foreach ($raw in $paths) {
        if (Test-Path -LiteralPath $raw) { return $true }
        $clean = (($raw.ToCharArray() | Where-Object { [int]$_ -ge 32 -and ('?','*','"','<','>','|') -notcontains $_ }) -join '')
        if ($clean -and (Test-Path -LiteralPath $clean)) { return $true }
        if ($clean) {
            $leaf = Split-Path $clean -Leaf
            foreach ($d in $script:CandidateDirs) { if ($d -and (Test-Path -LiteralPath (Join-Path $d $leaf))) { return $true } }
        }
    }
    return $false
}

# Returns @{ Present=bool; Clients=int; KeyPaths=@() }
function Get-ComponentState([string]$guid) {
    $sq = Get-SquishedGuid $guid
    $key = Join-Path $ComponentsRoot $sq
    if (-not (Test-Path -LiteralPath $key)) { return @{ Present=$false; Clients=0; KeyPaths=@() } }
    try { $p = Get-ItemProperty -LiteralPath $key -ErrorAction Stop }
    catch { return @{ Present=$true; Clients=0; KeyPaths=@() } }   # key exists but unreadable (ACL)
    $vals = @($p.PSObject.Properties | Where-Object { $_.Name -notlike 'PS*' })
    @{ Present=$true; Clients=$vals.Count; KeyPaths=@($vals | ForEach-Object { $_.Value }) }
}

$script:Results = New-Object System.Collections.Generic.List[object]
function Add-Result($category,$item,$detail,$pass) {
    $script:Results.Add([pscustomobject]@{ Category=$category; Item=$item; Detail=$detail; Pass=$pass })
}

# Assert a component GUID registration matches the mode.
function Check-Component($category,$name,$guid) {
    $s = Get-ComponentState $guid
    if ($Mode -eq 'Installed') {
        if (-not $s.Present) { Add-Result $category $name "NOT registered ($guid)" $false; return }
        # key-path file should exist
        $kp = @($s.KeyPaths | Where-Object { $_ -match '[A-Za-z]:\\' })
        $fileOk = ($kp.Count -eq 0) -or (Test-KeyPathFile $kp)
        $detail = "registered, clients=$($s.Clients)" + ($(if(-not $fileOk){ "; KEYPATH FILE MISSING: $($kp -join ';')" } else { '' }))
        Add-Result $category $name $detail $fileOk
    } else {
        Add-Result $category $name $(if($s.Present){"STILL registered, clients=$($s.Clients)"}else{'absent'}) (-not $s.Present)
    }
}

function Check-PathPresence($category,$item,$path,[switch]$Directory) {
    $exists = Test-Path -LiteralPath $path
    if ($Mode -eq 'Installed') { Add-Result $category $item $(if($exists){$path}else{"MISSING: $path"}) $exists }
    else                       { Add-Result $category $item $(if($exists){"STILL present: $path"}else{"absent: $path"}) (-not $exists) }
}

# ---------------------------------------------------------------------------
# Resolve standard folders
# ---------------------------------------------------------------------------
$cf64 = $env:CommonProgramFiles
$cf86 = ${env:CommonProgramFiles(x86)}; if (-not $cf86) { $cf86 = $cf64 }
$sys32 = Join-Path $env:windir 'System32'
$syswow = Join-Path $env:windir 'SysWOW64'

# Candidate dirs for the key-path filename fallback (see Test-KeyPathFile)
$script:CandidateDirs = @(
    $sys32, $syswow,
    (Join-Path $cf64 'OPC Foundation\Bin'),     (Join-Path $cf64 'OPC Foundation\Include'),
    (Join-Path $cf86 'OPC Foundation\Bin'),     (Join-Path $cf86 'OPC Foundation\Include')
)

$wantX86 = ($Product -in 'x86','x64','both')   # x64 combined MSI bundles the x86 merge module
$wantX64 = ($Product -in 'x64','both')

# elevation note
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) { Write-Warning "Not elevated - HKLM component reads may fail. Re-run as Administrator." }

Write-Host ""
Write-Host "OPC Core Components - install check"  -ForegroundColor Cyan
Write-Host "  Mode=$Mode  Product=$Product  IncludeSdk=$IncludeSdk" -ForegroundColor Cyan
Write-Host "  CommonFiles64=$cf64"
Write-Host "  CommonFiles86=$cf86"
Write-Host ""

# ---------------------------------------------------------------------------
# 1. Component registrations
# ---------------------------------------------------------------------------
if ($wantX86) { foreach ($k in $X86_RUNTIME.Keys) { Check-Component 'Component x86 runtime' $k $X86_RUNTIME[$k] } }
if ($wantX64) { foreach ($k in $X64_RUNTIME.Keys) { Check-Component 'Component x64 runtime' $k $X64_RUNTIME[$k] } }
if ($IncludeSdk) {
    if ($wantX86) { foreach ($k in $X86_SDK.Keys) { Check-Component 'Component x86 SDK' $k $X86_SDK[$k] } }
    if ($wantX64) { foreach ($k in $X64_SDK.Keys) { Check-Component 'Component x64 SDK' $k $X64_SDK[$k] } }
}

# ---------------------------------------------------------------------------
# 2. Folders
# ---------------------------------------------------------------------------
if ($wantX86) { Check-PathPresence 'Folder' 'CommonFiles(x86)\OPC Foundation\Bin'     (Join-Path $cf86 'OPC Foundation\Bin') -Directory }
if ($wantX64) { Check-PathPresence 'Folder' 'CommonFiles\OPC Foundation\Bin'          (Join-Path $cf64 'OPC Foundation\Bin') -Directory }
if ($IncludeSdk -and $wantX86) { Check-PathPresence 'Folder' 'CommonFiles(x86)\OPC Foundation\Include' (Join-Path $cf86 'OPC Foundation\Include') -Directory }
if ($IncludeSdk -and $wantX64) { Check-PathPresence 'Folder' 'CommonFiles\OPC Foundation\Include'      (Join-Path $cf64 'OPC Foundation\Include') -Directory }
# In Clean mode the whole "OPC Foundation" tree should be gone
if ($Mode -eq 'Clean') {
    Check-PathPresence 'Folder' 'CommonFiles\OPC Foundation (tree)'      (Join-Path $cf64 'OPC Foundation')
    Check-PathPresence 'Folder' 'CommonFiles(x86)\OPC Foundation (tree)' (Join-Path $cf86 'OPC Foundation')
}

# ---------------------------------------------------------------------------
# 3. x86 system DLLs (System32 / SysWOW64) - location depends on package/OS,
#    so for presence we accept either; for clean we require BOTH absent.
# ---------------------------------------------------------------------------
if ($wantX86) {
    foreach ($dll in $X86_SYSTEM_DLLS) {
        $p32 = Join-Path $sys32 $dll; $p64 = Join-Path $syswow $dll
        $in32 = Test-Path -LiteralPath $p32; $in64 = Test-Path -LiteralPath $p64
        if ($Mode -eq 'Installed') {
            $ok = $in32 -or $in64
            $where = @(); if($in32){$where+='System32'}; if($in64){$where+='SysWOW64'}
            Add-Result 'System DLL (x86)' $dll $(if($ok){"present in $($where -join ',')"}else{"MISSING from System32 & SysWOW64"}) $ok
        } else {
            $ok = -not ($in32 -or $in64)
            Add-Result 'System DLL (x86)' $dll $(if($ok){'absent'}else{"STILL present: $(@($p32,$p64 | Where-Object { Test-Path $_ }) -join ';')"}) $ok
        }
    }
}

# ---------------------------------------------------------------------------
# 4. OpcEnum service (installed whenever the x86 OpcEnum.exe payload is present)
# ---------------------------------------------------------------------------
if ($wantX86) {
    $svc = Get-Service -Name 'OpcEnum' -ErrorAction SilentlyContinue
    if ($Mode -eq 'Installed') { Add-Result 'Service' 'OpcEnum' $(if($svc){"present ($($svc.Status))"}else{'NOT installed'}) ([bool]$svc) }
    else                       { Add-Result 'Service' 'OpcEnum' $(if($svc){"STILL present ($($svc.Status))"}else{'absent'}) (-not $svc) }
    # OpcEnum CLSID (informational only)
    $clsidKey = "HKLM:\SOFTWARE\Classes\WOW6432Node\CLSID\{$OPCENUM_CLSID}"
    $clsidKey2 = "HKLM:\SOFTWARE\Classes\CLSID\{$OPCENUM_CLSID}"
    $clsidExists = (Test-Path $clsidKey) -or (Test-Path $clsidKey2)
    Write-Host ("  [info] OpcEnum CLSID {{{0}}} registered: {1}" -f $OPCENUM_CLSID, $clsidExists) -ForegroundColor DarkGray
}

# ---------------------------------------------------------------------------
# 5. UpgradeCode registration
# ---------------------------------------------------------------------------
function Check-Upgrade($name,$uc) {
    $key = Join-Path $UpgradeRoot (Get-SquishedGuid $uc)
    $exists = Test-Path -LiteralPath $key
    if ($Mode -eq 'Installed') { Add-Result 'UpgradeCode' $name $(if($exists){"registered ($uc)"}else{"NOT registered ($uc)"}) $exists }
    else                       { Add-Result 'UpgradeCode' $name $(if($exists){"STILL registered ($uc)"}else{"absent ($uc)"}) (-not $exists) }
}
# The combined x64 MSI registers only its own (x64) UpgradeCode; the standalone
# x86 MSI registers the x86 one. 'both' expects both.
if ($Product -eq 'x86' -or $Product -eq 'both') { Check-Upgrade 'x86 product' $UPGRADE_X86 }
if ($Product -eq 'x64' -or $Product -eq 'both') { Check-Upgrade 'x64 product' $UPGRADE_X64 }

# ---------------------------------------------------------------------------
# Report
# ---------------------------------------------------------------------------
$pass = @($Results | Where-Object Pass).Count
$fail = @($Results | Where-Object { -not $_.Pass }).Count

$Results | Group-Object Category | ForEach-Object {
    $cf = @($_.Group | Where-Object { -not $_.Pass }).Count
    $color = if ($cf -eq 0) { 'Green' } else { 'Red' }
    Write-Host ("[{0}] {1}  ({2}/{3} ok)" -f $(if($cf -eq 0){'PASS'}else{'FAIL'}), $_.Name, @($_.Group | Where-Object Pass).Count, @($_.Group).Count) -ForegroundColor $color
    $_.Group | Where-Object { -not $_.Pass } | ForEach-Object { Write-Host ("      X {0}: {1}" -f $_.Item, $_.Detail) -ForegroundColor Red }
}

Write-Host ""
if ($fail -eq 0) {
    Write-Host ("RESULT: PASS - {0} checks ok ({1} mode)" -f $pass, $Mode) -ForegroundColor Green
    exit 0
} else {
    Write-Host ("RESULT: FAIL - {0} ok, {1} FAILED ({2} mode). Re-run with -Verbose or inspect items above." -f $pass, $fail, $Mode) -ForegroundColor Red
    Write-Host "Tip: pass -IncludeSdk only if the SDK/DevEnvironment feature was installed; omit it for a default (runtime-only) install." -ForegroundColor Yellow
    exit 1
}
