//==============================================================================
// TITLE: OpcDa20Server.cpp
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
#include "COpcDa20Server.h"
#pragma warning(disable:4192)

//================================================================================
// COM Module Declarations

OPC_DECLARE_APPLICATION(OPCSample, OpcDa20Server, "OPC Data Access 2.05a Sample Server")

OPC_BEGIN_CLASS_TABLE()
    OPC_CLASS_TABLE_ENTRY(COpcDa20Server, OpcDa20Server, 1, "OPC Data Access 2.05a Source Server")
OPC_END_CLASS_TABLE()

OPC_BEGIN_CATEGORY_TABLE()
    OPC_CATEGORY_TABLE_ENTRY(OpcDa20Server, CATID_OPCDAServer20, OPC_CATEGORY_DESCRIPTION_DA20)
OPC_END_CATEGORY_TABLE()

#ifndef _USRDLL

// {725536FD-452C-44a1-97FE-490AC0F14AD5}
OPC_IMPLEMENT_LOCAL_SERVER(0x725536fd, 0x452c, 0x44a1, 0x97, 0xfe, 0x49, 0xa, 0xc0, 0xf1, 0x4a, 0xd5);

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

#else

OPC_IMPLEMENT_INPROC_SERVER();

//==============================================================================
// DllMain

extern "C"
BOOL WINAPI DllMain( 
    HINSTANCE hModule, 
    DWORD     dwReason, 
    LPVOID    lpReserved)
{
    OPC_START_INPROC_SERVER(hModule, dwReason);
    return TRUE;
}

#endif // _USRDLL
