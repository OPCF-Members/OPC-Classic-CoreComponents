//==============================================================================
// TITLE: OpcTestServer.cpp
//
// CONTENTS:
//
// Implements the required COM server functions.
//
// (c) Copyright 2002-2003 The OPC Foundation
// ALL RIGHTS RESERVED.
//
// DISCLAIMER:
//  This code is provided by the OPC Foundation solely to assist in
//  understanding and use of the appropriate OPC Specification(s) and may be
//  used as set forth in the License Grant section of the OPC Specification.
//  This code is provided as-is and without warranty or support of any sort
//  and is subject to the Warranty and Liability Disclaimers which appear
//  in the printed OPC Specification.
//
// MODIFICATION LOG:
//
// Date       By    Notes
// ---------- ---   -----
// 2002/11/16 RSA   First release.
//

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
