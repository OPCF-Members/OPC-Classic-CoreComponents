/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef __OPCSERVERLIST_H_
#define __OPCSERVERLIST_H_

#include "resource.h"

#include <vector>

#include "OpcEnum.h"

/////////////////////////////////////////////////////////////////////////////
// COpcServerList
class ATL_NO_VTABLE COpcServerList : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COpcServerList, &CLSID_OpcServerList>,
	public IOPCServerList,
	public IOPCServerList2
{
public:
	COpcServerList()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_OPCSERVERLIST)

BEGIN_COM_MAP(COpcServerList)
	COM_INTERFACE_ENTRY(IOPCServerList)
	COM_INTERFACE_ENTRY(IOPCServerList2)
END_COM_MAP()

// IOpcServerList
public:
	STDMETHOD(EnumClassesOfCategories)(
			/*[in]*/ ULONG cImplemented,
			/*[in,size_is(cImplemented)]*/ CATID rgcatidImpl[],
			/*[in]*/ ULONG cRequired,
			/*[in,size_is(cRequired)]*/ CATID rgcatidReq[],
			/*[out]*/ IEnumCLSID** ppenumClsid);

	STDMETHOD(GetClassDetails)(
			/*[in]*/ REFCLSID clsid, 
			/*[out]*/ LPOLESTR* ppszProgID, 
			/*[out]*/ LPOLESTR* ppszUserType)
	{
		HRESULT hr = ::OleRegGetUserType(clsid, USERCLASSTYPE_FULL, ppszUserType);
		if (FAILED(hr))
			return hr;
		return ::ProgIDFromCLSID(clsid, ppszProgID);
	}

	STDMETHOD(CLSIDFromProgID)(
			/*[in]*/ LPCOLESTR szProgId, 
			/*[out]*/ LPCLSID pClsid)
	{
		return ::CLSIDFromProgID(szProgId, pClsid);
	}

// IOpcServerList2
public:
	STDMETHOD(EnumClassesOfCategories)(
			/*[in]*/ ULONG cImplemented,
			/*[in,size_is(cImplemented)]*/ CATID rgcatidImpl[],
			/*[in]*/ ULONG cRequired,
			/*[in,size_is(cRequired)]*/ CATID rgcatidReq[],
			/*[out]*/ IOPCEnumGUID** ppenumClsid);

	STDMETHOD(GetClassDetails)(
			/*[in]*/ REFCLSID clsid, 
			/*[out]*/ LPOLESTR* ppszProgID, 
			/*[out]*/ LPOLESTR* ppszUserType,
			/*[out]*/ LPOLESTR* ppszVerIndProgID);


	typedef CComObject<CComEnum<IOPCEnumGUID, &IID_IOPCEnumGUID, GUID, _Copy<GUID>>> EnumGUID;
	
	typedef CComObject<CComEnum<IEnumGUID, &IID_IEnumCLSID, CLSID,_Copy<GUID>>> EnumCLSID;

private:
	
	// EnumClassesOfCategories2
	STDMETHOD(EnumClassesOfCategories2)(
		ULONG cImplemented,
		CATID rgcatidImpl[],
		ULONG cRequired,
		CATID rgcatidReq[],
		IOPCEnumGUID** ppenumClsid);
	
	// EnumClassesOfCategories2
	STDMETHOD(EnumClassesOfCategories2)(
		ULONG cImplemented,
		CATID rgcatidImpl[],
		ULONG cRequired,
		CATID rgcatidReq[],
		IEnumCLSID** ppenumClsid);

	void AddOldstyleServers(std::vector<GUID>* guids);
	HRESULT VersionIndependentProgIDFromCLSID(REFCLSID clsid, LPOLESTR * lplpszProgID);
};

#endif //__OPCSERVERLIST_H_
