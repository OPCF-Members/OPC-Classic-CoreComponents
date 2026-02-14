//==============================================================================
// TITLE: OpcDaHelpers.h
//
// CONTENTS:
// 
// XML/Text conversion helper functions.
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
// 2002/09/03 RSA   First release.
// 2002/11/16 RSA   Second release.
// 2003/06/25 RSA   Added OpcDaGetModuleVersion().

#ifndef _OpcDaHelpers_H
#define _OpcDaHelpers_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcUtils.h"
#include "OpcXmlType.h"

// OpcDaGetModuleName
COpcString OpcDaGetModuleName();

// OpcDaGetModulePath
COpcString OpcDaGetModulePath();

// OpcDaVersionInfo
struct OpcDaVersionInfo
{
	COpcString cFileDescription;
	WORD       wMajorVersion;
	WORD       wMinorVersion;
	WORD       wBuildNumber;
	WORD       wRevisionNumber;

	// Constructor
	OpcDaVersionInfo()
	{
		wMajorVersion = 0;
		wMinorVersion = 0;
		wBuildNumber = 0;
		wRevisionNumber = 0;
	}

	// Copy Constructor
	OpcDaVersionInfo(const OpcDaVersionInfo& cInfo)
	{
		cFileDescription = cInfo.cFileDescription;
		wMajorVersion    = cInfo.wMajorVersion;
		wMinorVersion    = cInfo.wMinorVersion;
		wBuildNumber     = cInfo.wBuildNumber;
		wRevisionNumber  = cInfo.wRevisionNumber;
	}
};

// OpcDaGetModuleVersion
bool OpcDaGetModuleVersion(OpcDaVersionInfo& cInfo);

#endif // _OpcDaHelpers_H