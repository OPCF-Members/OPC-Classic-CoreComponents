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
;#ifndef __OPCERROR_H
;#define __OPCERROR_H
;
;#if _MSC_VER >= 1000
;#pragma once
;#endif // _MSC_VER >= 1000
;
;// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
;// The 'Code' is set in the range defined OPC Commmon for DA (i.e. 0x0400 to 0x04FF)
;// Note that for backward compatibility not all existing codes use this range.
;

MessageID=0x0001
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDHANDLE
Language=English
The value of the handle is invalid.
.

MessageID=0x0004
Severity=Error
Facility=Interface
SymbolicName=OPC_E_BADTYPE
Language=English
The server cannot convert the data between the specified format and/or requested data type and the canonical data type. 
.

MessageID=0x0005
Severity=Error
Facility=Interface
SymbolicName=OPC_E_PUBLIC
Language=English
The requested operation cannot be done on a public group.
.

MessageID=0x0006
Severity=Error
Facility=Interface
SymbolicName=OPC_E_BADRIGHTS
Language=English
The item's access rights do not allow the operation.
.

MessageID=0x0007
Severity=Error
Facility=Interface
SymbolicName=OPC_E_UNKNOWNITEMID
Language=English
The item ID is not defined in the server address space or no longer exists in the server address space.
.

MessageID=0x0008
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDITEMID
Language=English
The item ID does not conform to the server's syntax.
.

MessageID=0x0009
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDFILTER
Language=English
The filter string was not valid.
.

MessageID=0x000A
Severity=Error
Facility=Interface
SymbolicName=OPC_E_UNKNOWNPATH
Language=English
The item's access path is not known to the server.
.

MessageID=0x000B
Severity=Error
Facility=Interface
SymbolicName=OPC_E_RANGE
Language=English
The value was out of range.
.

MessageID=0x000C
Severity=Error
Facility=Interface
SymbolicName=OPC_E_DUPLICATENAME
Language=English
Duplicate name not allowed.
.

MessageID=0x000D
Severity=Success
Facility=Interface
SymbolicName=OPC_S_UNSUPPORTEDRATE
Language=English
The server does not support the requested data rate but will use the closest available rate.
.

MessageID=0x000E
Severity=Success
Facility=Interface
SymbolicName=OPC_S_CLAMP
Language=English
A value passed to write was accepted but the output was clamped.
.

MessageID=0x000F
Severity=Success
Facility=Interface
SymbolicName=OPC_S_INUSE
Language=English
The operation cannot be performed because the object is bering referenced.
.

MessageID=0x0010
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDCONFIGFILE
Language=English
The server's configuration file is an invalid format.
.

MessageID=0x0011
Severity=Error
Facility=Interface
SymbolicName=OPC_E_NOTFOUND
Language=English
The requested object (e.g. a public group) was not found.
.

MessageID=0x0203
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALID_PID
Language=English
The specified property ID is not valid for the item.
.

MessageID=0x0400
Severity=Error
Facility=Interface
SymbolicName=OPC_E_DEADBANDNOTSET
Language=English
The item deadband has not been set for this item.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_DEADBANDNOTSUPPORTED
Language=English
The item does not support deadband.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_NOBUFFERING
Language=English
The server does not support buffering of data items that are collected at a faster rate than the group update rate.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_INVALIDCONTINUATIONPOINT
Language=English
The continuation point is not valid.
.

MessageID=
Severity=Success
Facility=Interface
SymbolicName=OPC_S_DATAQUEUEOVERFLOW
Language=English
Not every detected change has been returned since the server's buffer reached its limit and had to purge out the oldest data.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_RATENOTSET
Language=English
There is no sampling rate set for the specified item.  
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPC_E_NOTSUPPORTED
Language=English
The server does not support writing of quality and/or timestamp.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCPX_E_TYPE_CHANGED
Language=English
The dictionary and/or type description for the item has changed.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCPX_E_FILTER_DUPLICATE
Language=English
A data filter item with the specified name already exists. 
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCPX_E_FILTER_INVALID
Language=English
The data filter value does not conform to the server's syntax.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCPX_E_FILTER_ERROR
Language=English
An error occurred when the filter value was applied to the source data.
.

MessageID=
Severity=Success
Facility=Interface
SymbolicName=OPCCPX_S_FILTER_NO_DATA
Language=English
The item value is empty because the data filter has excluded all fields.
.

;#endif // ifndef __OPCERROR_H
