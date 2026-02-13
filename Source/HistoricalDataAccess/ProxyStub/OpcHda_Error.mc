;//==============================================================================
;// TITLE: OpcHda_Error.h
;//
;// CONTENTS:
;// 
;// Defines error codes for the Historial Data Access specifications.
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
;#ifndef __OPCHDAERROR_H
;#define __OPCHDAERROR_H
;
;#if _MSC_VER >= 1000
;#pragma once
;#endif // _MSC_VER >= 1000
;
;// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
;// The 'Code' is set in the range defined OPC Commmon for DA (i.e. 0x1000 to 0x10FF)
;

MessageID=0x1001
Severity=Error
Facility=Interface
SymbolicName=OPC_E_MAXEXCEEDED
Language=English
The maximum number of values requested exceeds the server's limit.
.

MessageID=
Severity=Informational
Facility=Interface
SymbolicName=OPC_S_NODATA
Language=English
There is no data within the specified parameters.
.

MessageID=
Severity=Informational
Facility=Interface
SymbolicName=OPC_S_MOREDATA
Language=English
There is more data satisfying the query than was returned.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDAGGREGATE
Language=English
The aggregate requested is not valid.
.

MessageID=
Severity=Informational
Facility=Interface
SymbolicName=OPC_S_CURRENTVALUE
Language=English
The server only returns current values for the requested item attributes.
.

MessageID=
Severity=Informational
Facility=Interface
SymbolicName=OPC_S_EXTRADATA
Language=English
Additional data satisfying the query was found.
.

MessageID=
Severity=Warning
Facility=Interface
SymbolicName=OPC_W_NOFILTER
Language=English
The server does not support this filter.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_UNKNOWNATTRID
Language=English
The server does not support this attribute.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_NOT_AVAIL
Language=English
The requested aggregate is not available for the specified item.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDDATATYPE
Language=English
The supplied value for the attribute is not a correct data type.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_DATAEXISTS
Language=English
Unable to insert - data already present.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDATTRID
Language=English
The supplied attribute ID is not valid.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_NODATAEXISTS
Language=English
The server has no value for the specified time and item ID.
.

MessageID=
Severity=Informational
Facility=Interface
SymbolicName=OPC_S_INSERTED
Language=English
The requested insert occurred.
.

MessageID=
Severity=Informational
Facility=Interface
SymbolicName=OPC_S_REPLACED
Language=English
The requested replace occurred.
.

;#endif // __OPCHDAERROR_H
