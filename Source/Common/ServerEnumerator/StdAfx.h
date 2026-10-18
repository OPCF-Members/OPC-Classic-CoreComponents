/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#if !defined(AFX_STDAFX_H__13486D47_4821_11D2_A494_3CB306C10000__INCLUDED_)
#define AFX_STDAFX_H__13486D47_4821_11D2_A494_3CB306C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STRICT

#ifndef WINVER
#define WINVER 0x0601
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x06010100
#endif
#define _ATL_APARTMENT_THREADED

#include <stdio.h>
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

class CServiceModule : public CComModule
{
public:
	HRESULT RegisterServer(BOOL bRegTypeLib, BOOL bService);
	HRESULT UnregisterServer();
	void Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID);
    void Start();
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    void Handler(DWORD dwOpcode);
    void Run();
    BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();
	LONG Unlock();
	void LogEvent(LPCTSTR pszFormat, ...);
    void SetServiceStatus(DWORD dwState);
    void SetupAsLocalServer();

//Implementation
private:
	static void WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI _Handler(DWORD dwOpcode);

// data members
public:
    TCHAR m_szServiceName[256];
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS m_status;
	DWORD dwThreadID;
	BOOL m_bService;
	BOOL m_bStartedByDCOM;
};

extern CServiceModule _Module;
#include <atlcom.h>

#include "comdef.h"
#include "comcat.h"
#include "opccomn.h"

EXTERN_C const IID IID_CATID_OPCDAServer10;
#define CATID_OPCDAServer10 IID_CATID_OPCDAServer10

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__13486D47_4821_11D2_A494_3CB306C10000__INCLUDED)
