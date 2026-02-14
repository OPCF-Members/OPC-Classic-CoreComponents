/* ========================================================================
 * Copyright (c) 2005-2010 The OPC Foundation, Inc. All rights reserved.
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

#include "StdAfx.h"
#include "CCategoryManager.h"

//==============================================================================
// Static Data

static COpcCriticalSection g_cLock;
static UINT                g_uRefs   = 0;

//==============================================================================
// Static Functions

// Initialize
static bool Initialize()
{
    COpcLock cLock(g_cLock);
    
    g_uRefs++;
    
	if (g_uRefs > 1)
    {
        return true;
    }

	return true;
}

// Uninitialize
static void Uninitialize()
{
    COpcLock cLock(g_cLock);

    g_uRefs--;
    
    if (g_uRefs == 0)
    {
        cLock.Unlock();
        COpcComModule::ExitProcess(S_OK);
    }
}

//==============================================================================
// CTestServer

// FinalConstruct
HRESULT CCategoryManager::FinalConstruct()
{
    if (!Initialize())
    {
        return E_FAIL;
    }

    return COpcComObject::FinalConstruct();
}

// FinalRelease
bool CCategoryManager::FinalRelease()
{
    COpcComObject::FinalRelease();
    Uninitialize();
	return true;
}

//==============================================================================
// IOPCServerList

HRESULT CCategoryManager::EnumClassesOfCategories( 
    ULONG cImplemented,
    CATID rgcatidImpl[],
    ULONG cRequired,
    CATID rgcatidReq[],
    IEnumGUID **ppenumClsid)
{
	IUnknown* ipUnknown = NULL;
	
	HRESULT hr = CoCreateInstance(
		CLSID_StdComponentCategoriesMgr,
		NULL,
		CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER,
		IID_IUnknown,
		(void**)&ipUnknown);
	
	if (FAILED(hr))
	{
		return hr;
	}

	ICatInformation* pCatInfo = NULL;

	hr = ipUnknown->QueryInterface(IID_ICatInformation, (void**)&pCatInfo);

	if (FAILED(hr))
	{
		ipUnknown->Release();
		return hr;
	}

	ipUnknown->Release();

	if (cImplemented <= 0)
	{
		cImplemented = -1;
		rgcatidImpl = NULL;
	}

	if (cRequired <= 0)
	{
		cRequired = -1;
		rgcatidReq = NULL;
	}

	hr = pCatInfo->EnumClassesOfCategories(
		cImplemented, 
		rgcatidImpl, 
		cRequired, 
		rgcatidReq, 
		ppenumClsid);

	pCatInfo->Release();

	return hr;
}

HRESULT CCategoryManager::GetClassDetails( 
    REFCLSID clsid,
    LPOLESTR* ppszProgID,
    LPOLESTR* ppszUserType)
{
	return E_NOTIMPL;
}

HRESULT CCategoryManager::CLSIDFromProgID( 
    LPCOLESTR szProgId,
    LPCLSID clsid)
{
	return E_NOTIMPL;
}
