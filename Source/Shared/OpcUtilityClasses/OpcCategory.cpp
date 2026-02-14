//==============================================================================
// TITLE: COpcEnumServers.cpp
//
// CONTENTS:
// 
// A class that allows browsing of COM servers by category.
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
// 2003/05/07 RSA   Added support for the OPC subkey used by DA1.0 and DA2.0 servers.
//

#include "StdAfx.h"
#include "OpcCategory.h"
#include "OpcRegistry.h"

//==============================================================================
// Define category ids that require special handling.

#define LOCAL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

LOCAL_DEFINE_GUID(CLSID, CATID_OPCDAServer10, 0x63D5F430, 0xCFE4, 0x11d1, 0xB2,0xC8,0x00,0x60,0x08,0x3B,0xA1,0xFB);
LOCAL_DEFINE_GUID(CLSID, CATID_OPCDAServer20, 0x63D5F432, 0xCFE4, 0x11d1, 0xB2,0xC8,0x00,0x60,0x08,0x3B,0xA1,0xFB);

// OpcEnumServersInCategory
HRESULT OpcEnumServersInCategory(
    LPCTSTR          tsHostName,
    const CATID&     tCategory,
    COpcList<CLSID>* pServers 
)
{
    HRESULT hResult = S_OK;
  
    IOPCServerList* ipInfo = NULL; 
    IEnumGUID*      ipEnumClsid = NULL;

    COSERVERINFO tInfo;
    memset(&tInfo, 0, sizeof(tInfo));

    MULTI_QI tInterfaces;
    memset(&tInterfaces, 0, sizeof(tInterfaces));

    TRY
    {
        // invalid arguments - generate error.
        if (pServers == NULL)
        {
            hResult = E_INVALIDARG;
            THROW();
        }

        // lookup clsid for OpcEnum server.
        CLSID cClsid = GUID_NULL;
    
        hResult = CLSIDFromProgID(L"OPC.ServerList.1", &cClsid);

	    if (FAILED(hResult)) 
        {
            THROW();
        }
        
        tInfo.pwszName   = OpcStrDup((LPCWSTR)(COpcString)tsHostName);
        tInterfaces.pIID = &__uuidof(IOPCServerList);

        // create the remote component category manager. 
        hResult = CoCreateInstanceEx(
            cClsid,
            NULL, 
            CLSCTX_SERVER,
            &tInfo,
            1,
            &tInterfaces
        ); 

	    if (FAILED(hResult) || FAILED(tInterfaces.hr)) 
        {
            THROW();
        }

        ipInfo = (IOPCServerList*)tInterfaces.pItf;

        // enumerate
        hResult = ipInfo->EnumClassesOfCategories(
            1,
            (CATID*)&tCategory,
            0,
            NULL,
            &ipEnumClsid
        );

	    if (FAILED(hResult)) 
        {
            THROW();
        }

        // copy clsids into return parameter.
        pServers->RemoveAll();

        do
        {
            ULONG ulFetched = 0;
            CLSID tClsid    = GUID_NULL;

            hResult = ipEnumClsid->Next(1, &tClsid, &ulFetched);

            if (ulFetched == 1)
            {
                pServers->AddTail(tClsid);   
            }
        }
        while (hResult == S_OK);

        // error while enumerating clsids.
	    if (FAILED(hResult)) 
        {
            THROW();
        }
    }
    CATCH 
    {
        if (pServers != NULL) pServers->RemoveAll();
    }
    FINALLY
    {
        if (ipEnumClsid != NULL) ipEnumClsid->Release();
        if (ipInfo != NULL) ipInfo->Release();
    }   

    return hResult;
}

// RegisterClsidInCategory
HRESULT RegisterClsidInCategory(REFCLSID clsid, CATID catid, LPCWSTR szDescription) 
{
    HRESULT hResult = S_OK;

    ICatRegister*    ipRegister = NULL; 
    ICatInformation* ipInfo     = NULL; 
   
    CoInitialize(NULL);

    TRY
    {
        // invalid arguments - generate error.
        if (catid == GUID_NULL)
        {
            hResult = E_INVALIDARG;
            THROW();
        }

        // create component category manager. 
        hResult = CoCreateInstance(
             CLSID_StdComponentCategoriesMgr,  
             NULL, 
             CLSCTX_INPROC_SERVER, 
             IID_ICatRegister, 
             (void**)&ipRegister
        ); 

	    if (FAILED(hResult)) 
        {
            THROW();
        }
        
        // create category
        CATEGORYINFO pInfo[1];

        pInfo[0].catid            = catid;
        pInfo[0].lcid             = LOCALE_USER_DEFAULT;
        pInfo[0].szDescription[0] = L'\0';

		if (szDescription != NULL)
		{
			_tcsncpy(pInfo[0].szDescription, szDescription, 127);
		}

        hResult = ipRegister->RegisterCategories(1, pInfo);

        if (FAILED(hResult)) 
        {
            THROW();
        }

        // create register class in category.
        hResult = ipRegister->RegisterClassImplCategories(clsid, 1, &catid);

        if (FAILED(hResult)) 
        {
            THROW();
        }

		// create OPC subkey for DA1.0 and DA2.0 servers.
		if (catid == CATID_OPCDAServer10 || catid == CATID_OPCDAServer20)
		{
			LPWSTR szProgID = NULL;

			if (SUCCEEDED(ProgIDFromCLSID(clsid, &szProgID)))
			{
				COpcString cSubKey;
				cSubKey += szProgID;
				cSubKey += _T("\\OPC");

				OpcRegSetValue(HKEY_CLASSES_ROOT, cSubKey, _T(""),	_T(""));
				OpcFree(szProgID);
			}
		}
    }
    CATCH 
    {
    }
    FINALLY
    {
        if (ipRegister != NULL) ipRegister->Release();
    }   

    CoUninitialize();

    return hResult;
}

// UnregisterClsidInCategory
HRESULT UnregisterClsidInCategory(REFCLSID clsid, CATID catid)
{
    HRESULT hResult = S_OK;
  
    ICatRegister* ipRegister = NULL; 

    CoInitialize(NULL);

    TRY
    {
        // invalid arguments - generate error.
        if (catid == GUID_NULL)
        {
            hResult = E_INVALIDARG;
            THROW();
        }
		
		// remove OPC subkey for DA1.0 and DA2.0 servers.
		if (catid == CATID_OPCDAServer10 || catid == CATID_OPCDAServer20)
		{
			LPWSTR szProgID = NULL;

			if (SUCCEEDED(ProgIDFromCLSID(clsid, &szProgID)))
			{
				COpcString cSubKey;
				cSubKey += szProgID;
				cSubKey += _T("\\OPC");

				OpcRegDeleteKey(HKEY_CLASSES_ROOT, cSubKey);
				OpcFree(szProgID);
			}
		}

        // create component category manager. 
        hResult = CoCreateInstance(
             CLSID_StdComponentCategoriesMgr,  
             NULL, 
             CLSCTX_INPROC_SERVER, 
             IID_ICatRegister, 
             (void**)&ipRegister
        ); 

	    if (FAILED(hResult)) 
        {
            THROW();
        }

        // create category.
        hResult = ipRegister->UnRegisterClassImplCategories(clsid, 1, &catid);

        if (FAILED(hResult)) 
        {
            THROW();
        }
    }
    CATCH 
    {
    }
    FINALLY
    {
        if (ipRegister != NULL) ipRegister->Release();
    }   

    CoUninitialize();

    return hResult;
}

