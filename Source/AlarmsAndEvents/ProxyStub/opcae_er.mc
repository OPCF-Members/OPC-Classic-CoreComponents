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
