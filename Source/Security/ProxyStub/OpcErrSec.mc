;//==============================================================================
;// TITLE: OpcErrSec.h
;//
;// CONTENTS:
;// 
;// Defines error codes for the Security specifications.
;//
;// (c) Copyright 2000-2003 The OPC Foundation
;// ALL RIGHTS RESERVED.
;//
;// DISCLAIMER:
;//  This code is provided by the OPC Foundation solely to assist in 
;//  understanding and use of the appropriate OPC Specification(s) and may be 
;//  used as set forth in the License Grant section of the OPC Specification.
;//  This code is provided as-is and without warranty or support of any sort
;//  and is subject to the Warranty and Liability Disclaimers which appear
;//  in the printed OPC Specification.
;//
;// MODIFICATION LOG:
;//
;// Date       By    Notes
;// ---------- ---   -----
;// 2000/08/18 OPC   Created.
;// 2003/01/06 RSA   Updated formatting. Added messages to proxy/stub resource block.
;//

MessageIdTypedef=HRESULT

SeverityNames=
(
	Success=0x0
	Informational=0x1
	Warning=0x2
	Error=0x3
)

FacilityNames=
(
	System=0x0FF
    Interface=0x004
)

LanguageNames=
(
	English=0x409:EXP00001
)

OutputBase=16

;
;#ifndef __OPC_ERR_SEC_H
;#define __OPC_ERR_SEC_H
;
;#if _MSC_VER >= 1000
;#pragma once
;#endif // _MSC_VER >= 1000
;
;// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
;// The 'Code' is set in the range defined OPC Commmon for DA (i.e. 0x0600 to 0x06FF)
;// Note that for backward compatibility not all existing codes use this range.
;

MessageID=0x0301
Severity=Error
Facility=Interface
SymbolicName=OPC_E_PRIVATE_ACTIVE
Language=English
A session using private OPC credentials is already active.
.

MessageID=0x0302
Severity=Error
Facility=Interface
SymbolicName=OPC_E_LOW_IMPERS_LEVEL
Language=English
Server requires higher impersonation level to access secured data.
.

MessageID=0x0303
Severity=Error
Facility=Interface
SymbolicName=OPC_S_LOW_AUTHN_LEVEL
Language=English
Server expected higher level of package privacy.
.

;#endif // __OPC_ERR_SEC_H
