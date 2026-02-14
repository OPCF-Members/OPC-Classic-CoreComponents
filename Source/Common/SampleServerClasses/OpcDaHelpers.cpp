//==============================================================================
// TITLE: OpcDaHelpers.cpp
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

#include "StdAfx.h"
#include "OpcDaHelpers.h"

#define TAG_APPID      _T("AppID")
#define TAG_CLSID      _T("CLSID")
#define TAG_CLASS_NAME _T("ClassName")

// OpcDaGetModuleName
COpcString OpcDaGetModuleName()
{
    COpcString cConfigPath;

    // get executable path.
    TCHAR tsPath[MAX_PATH+1];
    memset(tsPath, 0, sizeof(tsPath));

	DWORD dwResult = GetModuleFileName(NULL, tsPath, MAX_PATH);

    if (dwResult == 0)
    {
        return cConfigPath;
    }

    cConfigPath = tsPath;

    // remove directory.
    int iIndex = cConfigPath.ReverseFind(_T("\\"));
    
    if (iIndex != -1)
    {
        cConfigPath = cConfigPath.SubStr(iIndex+1);
    }

    // remove extension.
    iIndex = cConfigPath.ReverseFind(_T("."));
    
    if (iIndex != -1)
    {
        cConfigPath = cConfigPath.SubStr(0, iIndex);
    }

    return cConfigPath;
}

// OpcDaGetModulePath
COpcString OpcDaGetModulePath()
{
    COpcString cConfigPath;

    // get executable path.
    TCHAR tsPath[MAX_PATH+1];
    memset(tsPath, 0, sizeof(tsPath));

	DWORD dwResult = GetModuleFileName(NULL, tsPath, MAX_PATH);

    if (dwResult == 0)
    {
        return cConfigPath;
    }

    cConfigPath = tsPath;

    // remove file name.
    int iIndex = cConfigPath.ReverseFind(_T("\\"));
    
    if (iIndex != -1)
    {
        cConfigPath = cConfigPath.SubStr(0, iIndex);
    }

    return cConfigPath;
}

// OpcDaGetModuleVersion
bool OpcDaGetModuleVersion(OpcDaVersionInfo& cInfo)
{
	// initialize output parameters.
	cInfo.cFileDescription.Empty();
	cInfo.wMajorVersion = 0;
	cInfo.wMinorVersion = 0;
	cInfo.wBuildNumber = 0;
	cInfo.wRevisionNumber = 0;

	// get module path.
	TCHAR tsModuleName[MAX_PATH+1];
	memset(tsModuleName, 0, sizeof(tsModuleName));

	DWORD dwResult = GetModuleFileName(NULL, tsModuleName, sizeof(tsModuleName));

	if (dwResult == 0)
	{
		return false;
	}

	// get the size of the version info blob.
	DWORD dwSize = GetFileVersionInfoSize(tsModuleName, NULL);

	if (dwSize == 0)
	{
		return false;
	}

	// allocate space for the version info blob.
	BYTE* pBuffer = OpcArrayAlloc(BYTE, dwSize);
	memset(pBuffer, 0, dwSize);

	// get the version info blob.
	if (GetFileVersionInfo(tsModuleName, NULL, dwSize, pBuffer))
	{
		// get the base version info.
		UINT uInfoSize = 0;
		VS_FIXEDFILEINFO* pInfo = NULL;

		if (VerQueryValue(pBuffer, _T("\\"), (void**)&pInfo, &uInfoSize))
		{
			cInfo.wMajorVersion   = (WORD)((pInfo->dwFileVersionMS>>16)&0x00FF);
			cInfo.wMinorVersion   = (WORD)(pInfo->dwFileVersionMS&0x00FF);
			cInfo.wBuildNumber    = (WORD)((pInfo->dwFileVersionLS>>16)&0x00FF);
			cInfo.wRevisionNumber = (WORD)(pInfo->dwFileVersionLS&0x00FF);
		}

		UINT uTranslateSize = 0;
		struct LANGANDCODEPAGE { WORD wLanguage; WORD wCodePage; }* pTranslate = NULL;

		// read the list of languages and code pages.
		if (VerQueryValue(pBuffer, 
              _T("\\VarFileInfo\\Translation"),
              (void**)&pTranslate,
              &uTranslateSize))
		{
			TCHAR tsSubBlock[MAX_PATH+1];
			memset(tsSubBlock, 0, sizeof(tsSubBlock));

			_stprintf(
				tsSubBlock, 
				_T("\\StringFileInfo\\%04x%04x\\FileDescription"),
				pTranslate->wLanguage,
				pTranslate->wCodePage
			);

			TCHAR* pBlock = NULL;
			UINT   uBlockSize = 0;

			if (VerQueryValue(pBuffer, tsSubBlock, (void**)&pBlock, &uTranslateSize))
			{
				cInfo.cFileDescription = pBlock;
			}
		}
	}

	// free the buffer.
	OpcFree(pBuffer);
		
	return true;
}
