;//==============================================================================
;// TITLE: OpcCmdError.h
;//
;// CONTENTS:
;// 
;// Defines error codes for the OPC Command Execution specification.
;//
;// (c) Copyright 2004 The OPC Foundation
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
;// 2002/12/12 RSA   Created.
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
