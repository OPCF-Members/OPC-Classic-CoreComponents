//==============================================================================
// TITLE: OpcTestClient.cpp
//
// CONTENTS:
//
// A minimal OPC DA 2.05a test client that enumerates OPC servers via OpcEnum
// and calls GetStatus on each one found.
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <objbase.h>

// OPC interface declarations (MIDL-generated)
#include "opccomn.h"
#include "opcda.h"

// OPC GUIDs (MIDL-generated)
#include "opccomn_i.c"
#include "opcda_i.c"

// CLSID for OpcEnum (OpcServerList) — not in opccomn.idl, defined in OpcEnum.idl
// {13486D51-4821-11D2-A494-3CB306C10000}
static const CLSID CLSID_OpcServerList =
    {0x13486D51, 0x4821, 0x11D2, {0xA4, 0x94, 0x3C, 0xB3, 0x06, 0xC1, 0x00, 0x00}};

//==============================================================================
// Helper: OPCSERVERSTATE to string

static LPCTSTR ServerStateString(OPCSERVERSTATE state)
{
    switch (state)
    {
    case OPC_STATUS_RUNNING:    return _T("Running");
    case OPC_STATUS_FAILED:     return _T("Failed");
    case OPC_STATUS_NOCONFIG:   return _T("No Config");
    case OPC_STATUS_SUSPENDED:  return _T("Suspended");
    case OPC_STATUS_TEST:       return _T("Test");
    case OPC_STATUS_COMM_FAULT: return _T("Comm Fault");
    default:                    return _T("Unknown");
    }
}

//==============================================================================
// Helper: FILETIME to printable string

static void FormatFileTime(const FILETIME& ft, TCHAR* buf, int bufLen)
{
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    _sntprintf_s(buf, bufLen, _TRUNCATE,
        _T("%04d-%02d-%02d %02d:%02d:%02d"),
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);
}

//==============================================================================
// ConnectAndGetStatus — CoCreateInstance the server, call GetStatus, print, release.

static void ConnectAndGetStatus(REFCLSID clsid)
{
    IOPCServer* pServer = NULL;
    HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER,
                                  IID_IOPCServer, (void**)&pServer);
    if (FAILED(hr))
    {
        _tprintf(_T("  CoCreateInstance failed: 0x%08X\n"), hr);
        return;
    }

    _tprintf(_T("  Connected. Calling GetStatus...\n"));

    OPCSERVERSTATUS* pStatus = NULL;
    hr = pServer->GetStatus(&pStatus);

    if (SUCCEEDED(hr) && pStatus != NULL)
    {
        TCHAR szTime[64];

        FormatFileTime(pStatus->ftStartTime, szTime, 64);
        _tprintf(_T("  Start Time:   %s\n"), szTime);

        FormatFileTime(pStatus->ftCurrentTime, szTime, 64);
        _tprintf(_T("  Current Time: %s\n"), szTime);

        _tprintf(_T("  State:        %s\n"), ServerStateString(pStatus->dwServerState));
        _tprintf(_T("  Version:      %d.%d.%d\n"),
            pStatus->wMajorVersion, pStatus->wMinorVersion, pStatus->wBuildNumber);
        _tprintf(_T("  Vendor:       %ls\n"),
            pStatus->szVendorInfo ? pStatus->szVendorInfo : L"(none)");
        _tprintf(_T("  Groups:       %lu\n"), pStatus->dwGroupCount);
        _tprintf(_T("  Bandwidth:    %lu\n"), pStatus->dwBandWidth);

        if (pStatus->szVendorInfo) CoTaskMemFree(pStatus->szVendorInfo);
        CoTaskMemFree(pStatus);
    }
    else
    {
        _tprintf(_T("  GetStatus failed: 0x%08X\n"), hr);
    }

    pServer->Release();
    _tprintf(_T("  Released.\n"));
}

//==============================================================================
// main

int _tmain(int /* argc */, TCHAR* /* argv */[])
{
    _tprintf(_T("OPC DA 2.05a Test Client\n"));
    _tprintf(_T("=======================\n\n"));

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        _tprintf(_T("CoInitializeEx failed: 0x%08X\n"), hr);
        return 1;
    }

    CoInitializeSecurity(NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL);

    // Connect to OpcEnum via IOPCServerList.
    IOPCServerList* pServerList = NULL;
    hr = CoCreateInstance(CLSID_OpcServerList, NULL, CLSCTX_ALL,
                          IID_IOPCServerList, (void**)&pServerList);
    if (FAILED(hr))
    {
        _tprintf(_T("Could not connect to OpcEnum (IOPCServerList): 0x%08X\n"), hr);
        _tprintf(_T("Make sure the OpcEnum service is running.\n"));
        CoUninitialize();
        return 1;
    }

    _tprintf(_T("Connected to OpcEnum.\n\n"));

    // Enumerate OPC DA 2.0 servers.
    IEnumGUID* pEnum = NULL;
    CATID catid = CATID_OPCDAServer20;
    hr = pServerList->EnumClassesOfCategories(1, &catid, 0, NULL, &pEnum);
    if (FAILED(hr))
    {
        _tprintf(_T("EnumClassesOfCategories failed: 0x%08X\n"), hr);
        pServerList->Release();
        CoUninitialize();
        return 1;
    }

    _tprintf(_T("OPC DA 2.0 Servers\n"));
    _tprintf(_T("------------------\n"));

    CLSID clsid;
    ULONG fetched = 0;
    int count = 0;

    while (pEnum->Next(1, &clsid, &fetched) == S_OK)
    {
        count++;

        // Server info from OpcEnum.
        LPOLESTR wszProgID   = NULL;
        LPOLESTR wszUserType = NULL;
        pServerList->GetClassDetails(clsid, &wszProgID, &wszUserType);

        LPOLESTR wszClsid = NULL;
        StringFromCLSID(clsid, &wszClsid);

        _tprintf(_T("\nServer %d:\n"), count);
        _tprintf(_T("  CLSID:       %ls\n"), wszClsid   ? wszClsid   : L"?");
        _tprintf(_T("  ProgID:      %ls\n"), wszProgID  ? wszProgID  : L"?");
        _tprintf(_T("  Description: %ls\n"), wszUserType? wszUserType: L"?");

        // Connect, GetStatus, release.
        ConnectAndGetStatus(clsid);

        if (wszClsid)    CoTaskMemFree(wszClsid);
        if (wszProgID)   CoTaskMemFree(wszProgID);
        if (wszUserType) CoTaskMemFree(wszUserType);
    }

    if (count == 0)
        _tprintf(_T("\n(none found)\n"));
    else
        _tprintf(_T("\nTotal: %d server(s).\n"), count);

    pEnum->Release();
    pServerList->Release();
    CoUninitialize();
    return 0;
}
