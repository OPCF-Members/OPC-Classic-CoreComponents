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
