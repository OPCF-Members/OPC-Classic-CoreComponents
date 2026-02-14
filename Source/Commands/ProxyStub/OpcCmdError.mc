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
;#ifndef _OpcCmdError_H_
;#define _OpcCmdError_H_
;
;#if _MSC_VER >= 1000
;#pragma once
;#endif // _MSC_VER >= 1000
;
;
;// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
;// The 'Code' is set in the range defined OPC Commmon for DX (i.e. 0x0700 to 0x07FF)
;// Note that for backward compatibility not all existing codes use this range.
;

MessageID=0x0900
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_INVALIDBRANCH
Language=English
The Target ID specified in the request is not a valid branch.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_INVALIDNAMESPACE
Language=English
The specified namespace is not a valid namespace for this server.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_INVALIDCOMMANDNAME
Language=English
The specified command name is not valid for this server.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_BUSY
Language=English
The server is currently not able to process this command.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_EVENTFILTER_NOTSUPPORTED
Language=English
The server does not support filtering of events.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_NO_SUCH_COMMAND
Language=English
A command with the specified UUID is neither executing nor completed (and still stored in the cache).
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_ALREADY_CONNECTED
Language=English
A client is already connected for the specified Invoke UUID.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_NOT_CONNECTED
Language=English
No Callback ID is currently connected for the specified command.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_CONTROL_NOTSUPPORTED
Language=English
The server does not support the specified control for this command.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCCMD_E_INVALID_CONTROL
Language=English
The specified control is not possible in the current state of execution.
.

;#endif // ifndef _OpcCmdError_H_
