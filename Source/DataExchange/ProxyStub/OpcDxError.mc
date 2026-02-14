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
;#ifndef _OpcDxError_H_
;#define _OpcDxError_H_
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

MessageID=0x0700
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_PERSISTING
Language=English
Could not process request because the configuration is currently being persisted.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_NOITEMLIST
Language=English
No item list was passed in the request.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SERVER_STATE
Language=English
The operation failed because the server is in the wrong state.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_VERSION_MISMATCH
Language=English
The current object version does not match the specified version.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_UNKNOWN_ITEM_PATH
Language=English
The specified item path no longer exists in the DX server's address space.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_UNKNOWN_ITEM_NAME
Language=English
The specified item name no longer exists in the DX server's address space.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_ITEM_PATH
Language=English
The specified item path does not conform to the DX server syntax.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_ITEM_NAME
Language=English
The specified item name does not conform to the DX server syntax.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_NAME
Language=English
The source server name or connection name does not conform to the server syntax.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_DUPLICATE_NAME
Language=English
The connection name or source server name is already in use.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_BROWSE_PATH
Language=English
The browse path does not conform to the DX server syntax or it cannot be modified.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_SERVER_URL
Language=English
The syntax of the source server URL is not correct.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_SERVER_TYPE
Language=English
The source server type is not recognized.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_UNSUPPORTED_SERVER_TYPE
Language=English
The source server does not support the specified server type.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_CONNECTIONS_EXIST
Language=English
The source server cannot be deleted because connections exist.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TOO_MANY_CONNECTIONS
Language=English
The total number of connections would exceed the maximum supported by the DX server.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_OVERRIDE_BADTYPE
Language=English
The override value is not valid and overridden flag is set to true.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_OVERRIDE_RANGE
Language=English
The override value is outside of the target item�s range and overridden flag is set to true.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SUBSTITUTE_BADTYPE
Language=English
The substitute  value is not valid and the enable substitute value flag is set to true.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SUBSTITUTE_RANGE
Language=English
The substitute value is outside of the target item�s range the enable substitute value flag is set to true.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_TARGET_ITEM
Language=English
The target item does not conform to the DX server syntax or the item cannot be used as a target.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_UNKNOWN_TARGET_ITEM
Language=English
The target item no longer exists in the DX server�s address space.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TARGET_ALREADY_CONNECTED
Language=English
The target item is already connected or may not be changed in the method.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_UNKNOWN_SERVER_NAME
Language=English
The specified source server does not exist.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_UNKNOWN_SOURCE_ITEM
Language=English
The source item is no longer in the source server�s address space.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_SOURCE_ITEM
Language=English
The source item does not confirm to the source server�s syntax.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_QUEUE_SIZE
Language=English
The update queue size is not valid.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_DEADBAND
Language=English
The deadband is not valid.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_INVALID_CONFIG_FILE
Language=English
The DX server configuration file is not acessable.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_PERSIST_FAILED
Language=English
Could not save the DX server configuration.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TARGET_FAULT
Language=English
Target is online, but cannot service any request due to being in a fault state.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TARGET_NO_ACCESS
Language=English
Target is not online and may not be accessed.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_SERVER_FAULT
Language=English
Source server is online, but cannot service any request due to being in a fault state.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_SERVER_NO_ACCESS
Language=English
Source server is not online and may not be accessed.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SUBSCRIPTION_FAULT
Language=English
Source server is connected, however it could not create a subscription for the connection.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_ITEM_BADRIGHTS
Language=English
The source items access rights ddo not permit the operation.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_ITEM_BAD_QUALITY
Language=English
The source item value could be used because it has bad quality.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_ITEM_BADTYPE
Language=English
The source item cannot be converted to the target�s data type. This error reported by the source server.
.

MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_ITEM_RANGE
Language=English
The source item is out of the  range for the requested type. This error reported by the source server.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_SERVER_NOT_CONNECTED
Language=English
The source server is not connected.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_SOURCE_SERVER_TIMEOUT
Language=English
The source server was disconnected because to failed to respond to pings.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TARGET_ITEM_DISCONNECTED
Language=English
The target item is disconnected.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TARGET_NO_WRITES_ATTEMPTED
Language=English
The DX server has not attempted to write to the target.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TARGET_ITEM_BADTYPE
Language=English
The target item cannot be converted to the target�s data type.
.
	
MessageID=
Severity=Error
Facility=Interface
SymbolicName=OPCDX_E_TARGET_ITEM_RANGE
Language=English
The target item is outside the value range for the item.
.

MessageID=0x0780
Severity=Success
Facility=Interface
SymbolicName=OPCDX_S_TARGET_SUBSTITUTED
Language=English
The substitute value was written to the target.
.

MessageID=
Severity=Success
Facility=Interface
SymbolicName=OPCDX_S_TARGET_OVERRIDEN
Language=English
The override value was written to the target.
.

MessageID=
Severity=Success
Facility=Interface
SymbolicName=OPCDX_S_CLAMP
Language=English
Value written was accepted but the output was clamped.
.

;#endif // ifndef _OpcDxError_H_
