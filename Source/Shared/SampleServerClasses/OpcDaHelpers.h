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
