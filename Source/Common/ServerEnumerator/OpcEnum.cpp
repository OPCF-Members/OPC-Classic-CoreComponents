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

#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
#include "ntsecapi.h"
#include "DCOM Config/dcomperm.h"

#include "OpcEnum.h"
#include "OpcEnum_i.c"
#include "OpcSvLst.h"

CServiceModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_OpcServerList, COpcServerList)
END_OBJECT_MAP()

// FindOneOf
LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
	while (*p1 != NULL)
	{
		LPCTSTR p = p2;
		while (*p != NULL)
		{
			if (*p1 == *p++)
				return p1+1;
		}
		p1++;
	}
	return NULL;
}

// IsNT
bool IsNT()
{
	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(info);
	GetVersionEx(&info);
	return info.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

// RegisterServer
inline HRESULT CServiceModule::RegisterServer(BOOL bRegTypeLib, BOOL bService)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
		return hr;

	// Remove any previous service since it may point to
	// the incorrect file
	Uninstall();

	// Add service entries
	UpdateRegistryFromResource(IDR_OpcEnum, TRUE);

	// Adjust the AppID for Local Server or Service
	CRegKey keyAppID;
	LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"));
	if (lRes != ERROR_SUCCESS)
		return lRes;

	CRegKey key;
	lRes = key.Open(keyAppID, _T("{13486D44-4821-11D2-A494-3CB306C10000}"));
	if (lRes != ERROR_SUCCESS)
		return lRes;
	key.DeleteValue(_T("LocalService"));

	CRegKey win9xKey;
	if (!IsNT()) {
		lRes = win9xKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\RunServices"));
		if (lRes == ERROR_SUCCESS)
			win9xKey.DeleteValue(_T("OpcEnum"));
	}

	// check for OSes earlier than XP SP2
	if (IsLegacySecurityModel())
	{
		// Set LaunchPermission and AccessPermission to "Everyone"
		// Can be customized after installation with DCOMcnfg by user
		static BYTE abEveryone[] = {
			0x01,0x00,0x04,0x80,0x34,0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x14,0x00,0x00,0x00,0x02,0x00,0x20,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x18,0x00,
				0x01,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,
				0xa0,0x5f,0x84,0x1f,0x5e,0x2e,0x6b,0x49,0xce,0x12,0x03,0x03,0xf4,0x01,0x00,0x00,
				0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,0xa0,0x5f,0x84,0x1f,
				0x5e,0x2e,0x6b,0x49,0xce,0x12,0x03,0x03,0xf4,0x01,0x00,0x00};

		// If the keys are already defined, then just leave them as they are.
		if ( ::RegQueryValueEx(key, _T("LaunchPermission"), 0, 0, 0, 0) != ERROR_SUCCESS )
			lRes = ::RegSetValueEx(key, _T("LaunchPermission"), 0,REG_BINARY, abEveryone, sizeof(abEveryone));

		if ( ::RegQueryValueEx(key, _T("AccessPermission"), 0, 0, 0, 0) != ERROR_SUCCESS )
			lRes = ::RegSetValueEx(key, _T("AccessPermission"), 0,REG_BINARY, abEveryone, sizeof(abEveryone));
	}

	// use the APIs to set permissions for OSes later than or equal XP S2.
	else
	{
		DWORD dwResult = ChangeAppIDAccessACL(
			_T("{13486D44-4821-11D2-A494-3CB306C10000}"), 
			_T("Everyone"), 
			TRUE, 
			TRUE, 
			COM_RIGHTS_EXECUTE_LOCAL | COM_RIGHTS_EXECUTE_REMOTE | COM_RIGHTS_EXECUTE); 

		if (dwResult != ERROR_SUCCESS)
		{
			// could not set access permissions.
			return E_FAIL;
		}
		
		dwResult = ChangeAppIDLaunchAndActivateACL(
			_T("{13486D44-4821-11D2-A494-3CB306C10000}"), 
			_T("Everyone"), 
			TRUE, 
			TRUE, 
			COM_RIGHTS_ACTIVATE_LOCAL | COM_RIGHTS_EXECUTE_LOCAL | COM_RIGHTS_ACTIVATE_REMOTE | COM_RIGHTS_EXECUTE_REMOTE | COM_RIGHTS_EXECUTE); 

		if (dwResult != ERROR_SUCCESS)
		{
			// could not set launch and activate permissions.
			return E_FAIL;
		}
	}

	// Set the AuthenticationLevel to None
	if ( ::RegQueryValueEx(key, _T("AuthenticationLevel"), 0, 0, 0, 0) != ERROR_SUCCESS )
		key.SetDWORDValue(_T("AuthenticationLevel"), (DWORD)1);

	if (bService)
	{
		key.SetStringValue(_T("LocalService"), _T("OpcEnum"));

		// Only install a "true" service under NT.
		if (IsNT()) {
			key.SetStringValue(_T("ServiceParameters"), _T("-Service"));
			// Create service
			Install();
		}
		else {
			// Under Win9x set the key to make OpcEnum start with the system boot.

			// Get the executable file path
			TCHAR szFilePath[_MAX_PATH];
			::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

			win9xKey.SetStringValue(_T("OpcEnum"), szFilePath);
		}
	}

	// Add object entries
	hr = CComModule::RegisterServer(bRegTypeLib);

	CoUninitialize();
	return hr;
}

// UnregisterServer
inline HRESULT CServiceModule::UnregisterServer()
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
		return hr;

	// Remove service entries
	UpdateRegistryFromResource(IDR_OpcEnum, FALSE);
	// Remove service
	Uninstall();
	// Remove object entries
	CComModule::UnregisterServer();

	CoUninitialize();
	return S_OK;
}

// Init
inline void CServiceModule::Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID)
{
	CComModule::Init(p, h);

	m_bService = TRUE;

	LoadString(h, nServiceNameID, m_szServiceName, sizeof(m_szServiceName) / sizeof(TCHAR));

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;
}

// Unlock
LONG CServiceModule::Unlock()
{
	LONG l = CComModule::Unlock();
	if (l == 0 && !m_bService && m_bStartedByDCOM)
		PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
	return l;
}

// IsInstalled
BOOL CServiceModule::IsInstalled()
{
    BOOL bResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
	{
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
		{
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

// Install
inline BOOL CServiceModule::Install()
{
	if (IsInstalled())
		return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
	{
		MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
        return FALSE;
	}

    // Get the executable file path
    TCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

    SC_HANDLE hService = ::CreateService(
		hSCM, m_szServiceName, m_szServiceName,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

    if (hService == NULL)
	{
        ::CloseServiceHandle(hSCM);
		MessageBox(NULL, _T("Couldn't create service"), m_szServiceName, MB_OK);
		return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
	return TRUE;
}

// Uninstall
inline BOOL CServiceModule::Uninstall()
{
	if (!IsInstalled())
		return TRUE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
	{
		MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		MessageBox(NULL, _T("Couldn't open service"), m_szServiceName, MB_OK);
		return FALSE;
	}
	SERVICE_STATUS status;
	::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	BOOL bDelete = ::DeleteService(hService);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	// Wait until the service is actually removed from the SCM database.
	// Not waiting could cause problems if the service is immediately re-registered.
	// The loop waits a maximumn of 10 seconds in case of a fatal problem.
    for (int ii = 0; IsInstalled() && ii < 100; ii++)
    {
		Sleep(100);
    }

	if (bDelete)
		return TRUE;

	MessageBox(NULL, _T("Service could not be deleted"), m_szServiceName, MB_OK);
	return FALSE;
}

// LogEvent
void CServiceModule::LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
	va_list	pArg;

	va_start(pArg, pFormat);
	_vstprintf(chMsg, pFormat, pArg);
	va_end(pArg);

    lpszStrings[0] = chMsg;

	if (m_bService)
	{
	    /* Get a handle to use with ReportEvent(). */
		hEventSource = RegisterEventSource(NULL, m_szServiceName);
	    if (hEventSource != NULL)
	    {
	        /* Write to event log. */
	        ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
	        DeregisterEventSource(hEventSource);
	    }
	}
	else
	{
		// As we are not running as a service, just write the error to the console.
		_putts(chMsg);
	}
}

// Start
inline void CServiceModule::Start()
{
    SERVICE_TABLE_ENTRY st[] =
	{
        { m_szServiceName, _ServiceMain },
        { NULL, NULL }
    };
    if (m_bService && !::StartServiceCtrlDispatcher(st))
	{
		m_bService = FALSE;
	}
	if (m_bService == FALSE)
		Run();
}

// ServiceMain
inline void CServiceModule::ServiceMain(DWORD /* dwArgc */, LPTSTR* /* lpszArgv */)
{
    // Register the control request handler
    m_status.dwCurrentState = SERVICE_START_PENDING;
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
    if (m_hServiceStatus == NULL)
	{
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(SERVICE_START_PENDING);

    m_status.dwWin32ExitCode = S_OK;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    // When the Run function returns, the service has stopped.
    Run();

    SetServiceStatus(SERVICE_STOPPED);
	LogEvent(_T("Service stopped"));
}

// Handler
inline void CServiceModule::Handler(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		SetServiceStatus(SERVICE_STOP_PENDING);
		PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:
		LogEvent(_T("Bad service request"));
	}
}

// _ServiceMain
void WINAPI CServiceModule::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	_Module.ServiceMain(dwArgc, lpszArgv);
}

// _Handler
void WINAPI CServiceModule::_Handler(DWORD dwOpcode)
{
	_Module.Handler(dwOpcode); 
}

// SetServiceStatus
void CServiceModule::SetServiceStatus(DWORD dwState)
{
	m_status.dwCurrentState = dwState;
	::SetServiceStatus(m_hServiceStatus, &m_status);
}

// Run
void CServiceModule::Run()
{
	HRESULT hr;

	_Module.dwThreadID = GetCurrentThreadId();

	hr = CoInitialize(NULL);

    //  If you are running on NT 4.0 or higher you can use the following call
    //	instead to make the EXE free threaded.
    //  This means that calls come in on a random RPC thread
    //	HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	_ASSERTE(SUCCEEDED(hr));

	// This tells COM to use the security settings for this AppID from the registry.
	struct __declspec(uuid("13486D44-4821-11D2-A494-3CB306C10000")) lazyMansGuidDefinition;
	GUID MyAppID = __uuidof(lazyMansGuidDefinition);
	hr = CoInitializeSecurity(&MyAppID, 0, 0, 0, 0, 0, 0, EOAC_APPID, 0);
	_ASSERTE(SUCCEEDED(hr));
	if (FAILED(hr))
		LogEvent("CoInitializeSecurity failed: Error #0x%x", hr);

	hr = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);
	_ASSERTE(SUCCEEDED(hr));

	LogEvent(_T("Service started"));
    SetServiceStatus(SERVICE_RUNNING);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
		DispatchMessage(&msg);

	_Module.RevokeClassObjects();

	CoUninitialize();
}

// _tWinMain
extern "C" int WINAPI _tWinMain(
    HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
    LPTSTR    lpCmdLine, 
    int       nShowCmd
)
{
	lpCmdLine = GetCommandLine(); // this line necessary for _ATL_MIN_CRT

	_Module.Init(ObjectMap, hInstance, IDS_SERVICENAME);
	_Module.m_bService = TRUE;
	_Module.m_bStartedByDCOM = FALSE;

	TCHAR szTokens[] = _T("-/");

	LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
	while (lpszToken != NULL)
	{
		if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
			return _Module.UnregisterServer();

		// Register as Local Server
		if (lstrcmpi(lpszToken, _T("RegServer"))==0)
			return _Module.RegisterServer(TRUE, FALSE);
		
		// Register as Service
		if (lstrcmpi(lpszToken, _T("Service"))==0)
			return _Module.RegisterServer(TRUE, TRUE);
		
		// Started Win32 application via DCOM
		if (lstrcmpi(lpszToken, _T("Embedding"))==0)
			_Module.m_bStartedByDCOM = TRUE;
		
		lpszToken = FindOneOf(lpszToken, szTokens);
	}

	// Are we Service or Local Server
	CRegKey keyAppID;
	LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_EXECUTE);
	if (lRes != ERROR_SUCCESS)
		return lRes;

	CRegKey key;
	lRes = key.Open(keyAppID, _T("{13486D44-4821-11D2-A494-3CB306C10000}"), KEY_EXECUTE);
	if (lRes != ERROR_SUCCESS)
		return lRes;

	TCHAR szValue[_MAX_PATH];
	DWORD dwLen = _MAX_PATH;
	lRes = key.QueryStringValue(_T("LocalService"), szValue, &dwLen);

	_Module.m_bService = FALSE;
	if (lRes == ERROR_SUCCESS && IsNT())
		_Module.m_bService = TRUE;

	_Module.Start();

    // When we get here, the service has been stopped
    return _Module.m_status.dwWin32ExitCode;
}

