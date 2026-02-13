;//==============================================================================
;// TITLE: OpcBatchError.h
;//
;// CONTENTS:
;// 
;// Defines error codes for the Batch specifications.
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
;// 2000/01/17 OPC   Created.
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
;#ifndef __OPCBATCHERROR_H
;#define __OPCBATCHERROR_H
;
;#if _MSC_VER >= 1000
;#pragma once
;#endif // _MSC_VER >= 1000
;
;// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
;// The 'Code' is set in the range defined OPC Commmon for Batch (i.e. 0x0500 to 0x05FF)
;// Note that for backward compatibility not all existing codes use this range.
;

MessageID=0x0300
Severity=Error
Facility=Interface
SymbolicName=OPCB_E_NOT_MEANINGFUL
Language=English
The data is not meaningful at the present time.
.

;#endif // __OPCBATCHERROR_H
