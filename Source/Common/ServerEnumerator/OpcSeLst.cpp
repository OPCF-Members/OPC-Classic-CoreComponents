//============================================================================
// TITLE: OpcSeLst.cpp
//
// CONTENTS:
// 
// Implementation of the EnumClassesOfCategories methods.
//
// (c) Copyright 1998-2002 The OPC Foundation
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
// 2002/06/12 mar   Added the EnumClassesOfCategories method that uses 
//                  the new IOPCEnumGUID interface.
//
//                  Added AddOldstyleServers function to get all of the "old-style"
//                  DA 1.0 servers that did not register themselves with the 
//                  ccomponent ategory ID of CATID_OPCDAServer10.
//
// 2002/06/24 mar	Added the VerIndProgID parameter to
//		            IOPCServerList2::GetClassDetails. This returns the
//		            VersionIndependentProgID if it is in the registry.
//

#include "stdafx.h"
#include "OpcSvLst.h"

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
MIDL_DEFINE_GUID(CLSID, CLSID_OpcCategoryManager, 0xE4DCB3DB,0x343C,0x4037,0xA1,0x26,0x9A,0x02,0xA2,0x84,0xD6,0xCE);

//============================================================================
// COpcServerList

// EnumClassesOfCategories
STDMETHODIMP COpcServerList::EnumClassesOfCategories(
			/*[in]*/ ULONG cImplemented,
			/*[in,size_is(cImplemented)]*/ CATID rgcatidImpl[],
			/*[in]*/ ULONG cRequired,
			/*[in,size_is(cRequired)]*/ CATID rgcatidReq[],
			/*[out]*/ IEnumCLSID** ppenumClsid)
{
	/*
	ICatInformationPtr pCatInfo;
	HRESULT hr = pCatInfo.CreateInstance(CLSID_StdComponentCategoriesMgr);
	if (FAILED(hr))
		return hr;

	if (!cImplemented)
	{
		cImplemented = -1;
		rgcatidImpl = NULL;
	}
	if (!cRequired)
	{
		cRequired = -1;
		rgcatidReq = NULL;
	}
	return pCatInfo->EnumClassesOfCategories(cImplemented, rgcatidImpl, 
		cRequired, rgcatidReq, ppenumClsid);
	*/

	IOPCEnumGUID* ipEnum = NULL;
	HRESULT hr = EnumClassesOfCategories2(cImplemented, rgcatidImpl, cRequired, rgcatidReq, &ipEnum);

	if (FAILED(hr))
	{
		return hr;
	}

	// need to create a master list.
	std::vector<GUID>* guids = new std::vector<GUID>;

	// update list.
	GUID guid;
	unsigned long count;

	while ((hr = ipEnum->Next(1, &guid, &count)) == S_OK)
	{
		guids->push_back(guid);
	}

	ipEnum->Release();

	if (FAILED(hr)) 
	{
		delete guids;
		return hr;
	}

	// construct enumerator to return.
	EnumCLSID* newEnum = new EnumCLSID;

	if (guids->size() > 0)
	{
		newEnum->Init(&(guids->front()), &(guids->back())+1, NULL, AtlFlagTakeOwnership);
	}
	else
	{
		newEnum->Init(NULL, NULL, NULL, AtlFlagTakeOwnership);
	}

	hr = newEnum->QueryInterface(ppenumClsid);

	if (FAILED(hr))
	{
		delete newEnum;
	}

	return hr;
}

// EnumClassesOfCategories
STDMETHODIMP COpcServerList::EnumClassesOfCategories(
			/*[in]*/ ULONG cImplemented,
			/*[in,size_is(cImplemented)]*/ CATID rgcatidImpl[],
			/*[in]*/ ULONG cRequired,
			/*[in,size_is(cRequired)]*/ CATID rgcatidReq[],
			/*[out]*/ IOPCEnumGUID** ppenumClsid)
{
	return EnumClassesOfCategories2(cImplemented, rgcatidImpl, cRequired, rgcatidReq, ppenumClsid);

	/*
	*ppenumClsid = 0;

	// Get the enum object from comcat.
	CComPtr<IEnumGUID> enumGuid;

	HRESULT hr = EnumClassesOfCategories(cImplemented, rgcatidImpl,
		cRequired, rgcatidReq, &enumGuid);

	if (FAILED(hr))
		return hr;

	std::vector<GUID>* guids = new std::vector<GUID>;

	// Add all of the returned GUIDs to the vector.
	unsigned long count;	// Count
	GUID guid;
	while ((hr = enumGuid->Next(1, &guid, &count)) == S_OK)
	{
		guids->push_back(guid);
	}

	if (FAILED(hr)) {
		delete guids;
		return hr;
	}

	// Do we need to enumerate the old-style DA 1.0 servers?
	if (cImplemented != -1) {
		for (ULONG i = 0; i < cImplemented; i++) {
			if ( IsEqualGUID(rgcatidImpl[i], CATID_OPCDAServer10) ) {
				AddOldstyleServers(guids);
				break;
			}
		}
	}

	EnumGUID* newEnum = new EnumGUID;

	if (guids->size() > 0)
	{
		newEnum->Init(&(guids->front()), &(guids->back())+1, NULL, AtlFlagTakeOwnership);
	}
	else
	{
		newEnum->Init(NULL, NULL, NULL, AtlFlagTakeOwnership);
	}

	//
	//newEnum->Init(guids->begin(), guids->end(), NULL, AtlFlagTakeOwnership);
    //

	hr = newEnum->QueryInterface(ppenumClsid);
	if (FAILED(hr))
		delete newEnum;

	return hr;
	*/
}

// EnumClassesOfCategories
STDMETHODIMP COpcServerList::EnumClassesOfCategories2(
			/*[in]*/ ULONG cImplemented,
			/*[in,size_is(cImplemented)]*/ CATID rgcatidImpl[],
			/*[in]*/ ULONG cRequired,
			/*[in,size_is(cRequired)]*/ CATID rgcatidReq[],
			/*[out]*/ IEnumCLSID** ppenumClsid)
{
	ICatInformationPtr pCatInfo;
	HRESULT hr = pCatInfo.CreateInstance(CLSID_StdComponentCategoriesMgr);
	if (FAILED(hr))
		return hr;

	if (!cImplemented)
	{
		cImplemented = -1;
		rgcatidImpl = NULL;
	}
	if (!cRequired)
	{
		cRequired = -1;
		rgcatidReq = NULL;
	}
	return pCatInfo->EnumClassesOfCategories(cImplemented, rgcatidImpl, 
		cRequired, rgcatidReq, ppenumClsid);
}

// EnumClassesOfCategories2
STDMETHODIMP COpcServerList::EnumClassesOfCategories2(
	ULONG cImplemented,
	CATID rgcatidImpl[],
	ULONG cRequired,
	CATID rgcatidReq[],
	IOPCEnumGUID** ppenumClsid)
{
	*ppenumClsid = 0;

	// need to create a master list.
	std::vector<GUID>* guids = new std::vector<GUID>;

	// enumerate that COM categories.
	CComPtr<IEnumGUID> enumGuid;

	HRESULT hr = EnumClassesOfCategories2(
		cImplemented, rgcatidImpl,
		cRequired, 
		rgcatidReq, 
		&enumGuid);

	if (FAILED(hr))
	{
		return hr;
	}

	// update list.
	GUID guid;
	unsigned long count;
	while ((hr = enumGuid->Next(1, &guid, &count)) == S_OK)
	{
		guids->push_back(guid);
	}

	enumGuid.Release();

	if (FAILED(hr)) 
	{
		delete guids;
		return hr;
	}

	// need to fetch x64 only servers from the OPC Category Manager (call should fail on x86 systems).
	CComQIPtr<IOPCServerList> pList;

	hr = pList.CoCreateInstance(CLSID_OpcCategoryManager);

	if (SUCCEEDED(hr))
	{
		CComPtr<IEnumGUID> enumGuid2;

		hr = pList->EnumClassesOfCategories(cImplemented, rgcatidImpl, cRequired, rgcatidReq, &enumGuid2);
		
		if (SUCCEEDED(hr))
		{
			while ((hr = enumGuid2->Next(1, &guid, &count)) == S_OK)
			{
				bool found = false;

				for (UINT ii = 0; ii < guids->size(); ii++) 
				{
					if (IsEqualGUID((*guids)[ii], guid))
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					guids->push_back(guid);
				}
			}

			enumGuid2.Release();
		}

		pList.Release();
	}

	// Do we need to enumerate the old-style DA 1.0 servers?
	if (cImplemented != -1) 
	{
		for (ULONG i = 0; i < cImplemented; i++) 
		{
			if (IsEqualGUID(rgcatidImpl[i], CATID_OPCDAServer10)) 
			{
				AddOldstyleServers(guids);
				break;
			}
		}
	}

	// construct enumerator to return.
	EnumGUID* newEnum = new EnumGUID;

	if (guids->size() > 0)
	{
		newEnum->Init(&(guids->front()), &(guids->back())+1, NULL, AtlFlagTakeOwnership);
	}
	else
	{
		newEnum->Init(NULL, NULL, NULL, AtlFlagTakeOwnership);
	}

	hr = newEnum->QueryInterface(ppenumClsid);

	if (FAILED(hr))
	{
		delete newEnum;
	}

	return hr;
}

// AddOldstyleServers
void COpcServerList::AddOldstyleServers(std::vector<GUID>* guids)
{
	USES_CONVERSION;

	_TCHAR szKey[200];
	for (DWORD dwIndex = 0 ;
		RegEnumKey(HKEY_CLASSES_ROOT, dwIndex, szKey, 200) == ERROR_SUCCESS ;
		++dwIndex )	{

		// Open each subkey (the ProgIDs)
		HKEY hProgID;
		_TCHAR szDummy[2048];

		if (RegOpenKeyEx( HKEY_CLASSES_ROOT, szKey, 0, KEY_READ, &hProgID) == ERROR_SUCCESS) {

			// and for that subkey/ProgID - try to read the OPC Subkey
			long lVsize = 2048;
			if(RegQueryValue( hProgID, _T("OPC"), szDummy, &lVsize) == ERROR_SUCCESS) {

				// Check to see if this is already in the list.
				CLSID clsid;
				CLSIDFromString( T2OLE(szKey), &clsid);

				unsigned int i = 0;

				for (i = 0; i < guids->size(); i++) {

					if ( IsEqualGUID((*guids)[i], clsid) )
						break;
				}

				// If we did not find the GUID in the list, i will equal
				// guids->size().
				if (i == guids->size())
					guids->push_back(clsid);
			}

			RegCloseKey( hProgID ) ;
		}
		// Enum the next one...
	}
}

// GetClassDetails
STDMETHODIMP COpcServerList::GetClassDetails(
			/*[in]*/ REFCLSID clsid, 
			/*[out]*/ LPOLESTR* ppszProgID, 
			/*[out]*/ LPOLESTR* ppszUserType,
			/*[out]*/ LPOLESTR* ppszVerIndProgID)
{
	*ppszVerIndProgID = 0;

	// Get everything but the version independent prog ID.
	HRESULT hr = GetClassDetails(clsid, ppszProgID, ppszUserType);

	if (FAILED(hr))
		return hr;

	// Get the version independent prog ID.
	hr = VersionIndependentProgIDFromCLSID(clsid, ppszVerIndProgID);

	// If we could not get the version independent prog ID then just copy
	// the stanadard prog ID.
	if (FAILED(hr)) {

		// Allocate enough space for the unicode string with
		// the NULL terminator.
		long size = (long)wcslen(*ppszProgID)*2 + 2;
		*ppszVerIndProgID = (LPOLESTR)CoTaskMemAlloc(size);
		CopyMemory (*ppszVerIndProgID, *ppszProgID, size);
	}

	return S_OK;
}

// VersionIndependentProgIDFromCLSID
HRESULT COpcServerList::VersionIndependentProgIDFromCLSID(REFCLSID clsid, LPOLESTR * lplpszProgID)
{
	USES_CONVERSION;

	LPOLESTR id;
	StringFromCLSID (clsid, &id);

	_TCHAR verId[2048] = _T("CLSID\\");
	_tcscat( verId, W2T(id) );

	CoTaskMemFree (id);

	_tcscat( verId, _T("\\VersionIndependentProgID") );

	CRegKey key;
	if (key.Open (HKEY_CLASSES_ROOT, verId, KEY_READ) != ERROR_SUCCESS)
		return REGDB_E_CLASSNOTREG;

	_TCHAR buf[1024];
	DWORD bufSize = sizeof(buf);
	key.QueryStringValue(NULL, buf, &bufSize);

	if (bufSize > 0) {
		// bufSize contains the size of the data string including the NULL
		// terminator. Multiply by two if we are not compiled for UNICODE.
#ifndef _UNICODE
		bufSize *= 2;
#endif
		*lplpszProgID = (LPOLESTR)CoTaskMemAlloc (bufSize);
		CopyMemory (*lplpszProgID, T2W (buf), bufSize);
		return S_OK;
	}
	else
		return E_FAIL;
}
