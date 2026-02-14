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
