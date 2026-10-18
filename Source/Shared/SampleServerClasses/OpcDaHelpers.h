/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

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
