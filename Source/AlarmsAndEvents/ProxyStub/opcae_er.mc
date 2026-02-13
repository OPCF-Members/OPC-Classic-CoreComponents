;//==============================================================================
;// TITLE: opcae_er.h
;//
;// CONTENTS:
;// 
;// Defines error codes for the OPC Alarms & Events specification.
;//
;// (c) Copyright 1997-2003 The OPC Foundation
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
;// 2002/10/02 JL    Updated for Version 1.1.
;// 2003/01/06 RSA   Fixed formatting to comply with coding guidelines.
;//

MessageIdTypedef=HRESULT

SeverityNames=(
			   Success=0x0
			   Informational=0x1
			   Warning=0x2
			   Error=0x3
			  )

FacilityNames=(
			   System=0x0FF
                           Interface=0x004	;// FACILITY_ITF
			  )

LanguageNames=(
			  English=0x409:EXP00001
			 )

OutputBase=16

;
;#ifndef __OPCAE_ER_H
;#define __OPCAE_ER_H
;
;#if _MSC_VER >= 1000
;#pragma once
;#endif // _MSC_VER >= 1000
;
;// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
;// The 'Code' is set in the range defined OPC Commmon for AE (i.e. 0x0200 to 0x02FF)
;

MessageID=0x0200
Severity=Success
Facility=Interface
SymbolicName=OPC_S_ALREADYACKED
Language=English
The condition has already been acknowleged.
.

MessageID=
SymbolicName=OPC_S_INVALIDBUFFERTIME
Language=English
The buffer time parameter was invalid.
.

MessageID=
SymbolicName=OPC_S_INVALIDMAXSIZE
Language=English
The max size parameter was invalid.
.

MessageID=
SymbolicName=OPC_S_INVALIDKEEPALIVETIME
Language=English
The KeepAliveTime parameter was invalid.
.

MessageID=0x0203
Severity=Error
SymbolicName=OPC_E_INVALIDBRANCHNAME
Language=English
The string was not recognized as an area name.
.

MessageID=
SymbolicName=OPC_E_INVALIDTIME
Language=English
The time does not match the latest active time.
.

MessageID=
SymbolicName=OPC_E_BUSY
Language=English
A refresh is currently in progress.
.

MessageID=
SymbolicName=OPC_E_NOINFO
Language=English
Information is not available.
.

;#endif // __OPCAE_ER_H
