;// ========================================================================
;// Copyright (c) 2002-2026 OPC Foundation. All rights reserved.
;//
;// OPC Foundation MIT License 1.00
;//
;// Permission is hereby granted, free of charge, to any person
;// obtaining a copy of this software and associated documentation
;// files (the "Software"), to deal in the Software without
;// restriction, including without limitation the rights to use,
;// copy, modify, merge, publish, distribute, sublicense, and/or sell
;// copies of the Software, and to permit persons to whom the
;// Software is furnished to do so, subject to the following
;// conditions:
;//
;// The above copyright notice and this permission notice shall be
;// included in all copies or substantial portions of the Software.
;// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
;// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
;// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
;// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
;// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
;// OTHER DEALINGS IN THE SOFTWARE.
;//
;// The complete license agreement can be found here:
;// http://opcfoundation.org/License/MIT/1.00/
;// ========================================================================

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
