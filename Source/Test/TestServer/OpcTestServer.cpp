/* ========================================================================
 * Copyright (c) 2002-2026 OPC Foundation. All rights reserved.
 *
 * OPC Foundation MIT License 1.00
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * The complete license agreement can be found here:
 * http://opcfoundation.org/License/MIT/1.00/
 * ======================================================================*/

#include "StdAfx.h"
#include "COpcTestServer.h"
#pragma warning(disable:4192)

//================================================================================
// COM Module Declarations

#ifdef OPC_PLATFORM_X64

OPC_DECLARE_APPLICATION(OPC, OpcTestServer_x64, "OPC DA 2.05a Test Server (x64)")

OPC_BEGIN_CLASS_TABLE()
    OPC_CLASS_TABLE_ENTRY(COpcTestServer, OpcTestServer_x64, 1, "OPC DA 2.05a Test Server (x64)")
OPC_END_CLASS_TABLE()

OPC_BEGIN_CATEGORY_TABLE()
    OPC_CATEGORY_TABLE_ENTRY(OpcTestServer_x64, CATID_OPCDAServer20, OPC_CATEGORY_DESCRIPTION_DA20)
OPC_END_CATEGORY_TABLE()

// {F8582CF9-88FB-11DA-A5ED-0060B0692061}
OPC_IMPLEMENT_LOCAL_SERVER(0xf8582cf9, 0x88fb, 0x11da, 0xa5, 0xed, 0x00, 0x60, 0xb0, 0x69, 0x20, 0x61);

#else

OPC_DECLARE_APPLICATION(OPC, OpcTestServer_x86, "OPC DA 2.05a Test Server (x86)")

OPC_BEGIN_CLASS_TABLE()
    OPC_CLASS_TABLE_ENTRY(COpcTestServer, OpcTestServer_x86, 1, "OPC DA 2.05a Test Server (x86)")
OPC_END_CLASS_TABLE()

OPC_BEGIN_CATEGORY_TABLE()
    OPC_CATEGORY_TABLE_ENTRY(OpcTestServer_x86, CATID_OPCDAServer20, OPC_CATEGORY_DESCRIPTION_DA20)
OPC_END_CATEGORY_TABLE()

// {F8582CF4-88FB-11DA-A5ED-0060B0692061}
OPC_IMPLEMENT_LOCAL_SERVER(0xf8582cf4, 0x88fb, 0x11da, 0xa5, 0xed, 0x00, 0x60, 0xb0, 0x69, 0x20, 0x61);

#endif

//================================================================================
// WinMain

extern "C" int WINAPI _tWinMain(
    HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nShowCmd
)
{
    OPC_START_LOCAL_SERVER_EX(hInstance, lpCmdLine);
    return 0;
}
