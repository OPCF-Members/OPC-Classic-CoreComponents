//==============================================================================
// TITLE: OpcRegistry.h
//
// CONTENTS:
// 
// Declarations for registry access functions.
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
//

#ifndef _OpcRegistry_H_
#define _OpcRegistry_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"

//==============================================================================
// FUNCTION: OpcRegGetValue
// PURPOSE:  Gets a string value from the registry.

bool OPCUTILS_API OpcRegGetValue(
	HKEY    hBaseKey,
	LPCTSTR tsSubKey,
	LPCTSTR tsValueName,
	LPTSTR* ptsValue
);

//==============================================================================
// FUNCTION: OpcRegGetValue
// PURPOSE:  Gets a DWORD value from the registry.

bool OPCUTILS_API OpcRegGetValue(
	HKEY    hBaseKey,
	LPCTSTR tsSubKey,
	LPCTSTR tsValueName,
	DWORD*  pdwValue
);

//==============================================================================
// FUNCTION: OpcRegGetValue
// PURPOSE:  Gets a DWORD value from the registry.

bool OPCUTILS_API OpcRegGetValue(
	HKEY    hBaseKey,
	LPCTSTR tsSubKey,
	LPCTSTR tsValueName,
	BYTE**  ppValue,
	DWORD*  pdwLength
);

//==============================================================================
// FUNCTION: OpcRegSetValue
// PURPOSE:  Sets a string value in the registry.

bool OPCUTILS_API OpcRegSetValue(
	HKEY    hBaseKey,
	LPCTSTR tsSubKey,
	LPCTSTR tsValueName,
	LPCTSTR tsValue
);

//==============================================================================
// FUNCTION: OpcRegSetValue
// PURPOSE:  Gets a DWORD value from the registry.

bool OPCUTILS_API OpcRegSetValue(
	HKEY    hBaseKey,
	LPCTSTR tsSubKey,
	LPCTSTR tsValueName,
	DWORD   dwValue
);

//==============================================================================
// FUNCTION: OpcRegSetValue
// PURPOSE:  Sets a string value in the registry.

bool OPCUTILS_API OpcRegSetValue(
	HKEY    hBaseKey,
	LPCTSTR tsSubKey,
	LPCTSTR tsValueName,
	BYTE*   pValue,
	DWORD   dwLength
);

//==============================================================================
// FUNCTION: OpcRegDeleteKey
// PURPOSE:  Recursively deletes a key and all sub keys.
// NOTES:

bool OPCUTILS_API OpcRegDeleteKey(
	HKEY    hBaseKey,
	LPCTSTR tsSubKey
);

#endif //ndef _OpcRegistry_H_
