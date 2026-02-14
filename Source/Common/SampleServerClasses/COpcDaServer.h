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

#ifndef _COpcDaServer_H_
#define _COpcDaServer_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaCache.h"
#include "COpcDaGroup.h"
#include "COpcDaTransaction.h"
#include "COpcThread.h"

//============================================================================
// CLASS:   COpcDaServer
// PURPOSE: A class that implements the IOPCServer interface.
// NOTES:

class COpcDaServer :
    public COpcCommon,
    public COpcCPContainer,
    public IOPCBrowseServerAddressSpace,
    public IOPCItemProperties,
    public IOPCServer,
    public IOPCBrowse,
    public IOPCItemIO,
    public COpcSynchObject
{
    OPC_CLASS_NEW_DELETE()

public:

    //=========================================================================
    // Operators

    // Constructor
    COpcDaServer();

    // Destructor 
    ~COpcDaServer();

    //=========================================================================
    // Public Methods

    // FinalConstruct
    virtual HRESULT FinalConstruct();

    // FinalRelease
    virtual bool FinalRelease();
	
	// CreateGroup
	virtual COpcDaGroup* CreateGroup(COpcDaServer& cServer, const COpcString& cName) { return new COpcDaGroup(cServer, cName); }

	// SetLastUpdateTime
	void SetLastUpdateTime();

	// SetGroupName
	HRESULT SetGroupName(
		const COpcString& cOldName, 
		const COpcString& cNewName);

	// CreateGroup
	HRESULT CreateGroup(
		const COpcString& cName, 
		COpcDaGroup**     ppGroup);

	// CloneGroup
	HRESULT CloneGroup(
		const COpcString& cName, 
		const COpcString& cCloneName, 
		COpcDaGroup**     ppClone);

	// DeleteGroup
	HRESULT DeleteGroup(const COpcString& cName);

    //=========================================================================
    // IOPCServer

    // AddGroup
    STDMETHODIMP AddGroup(
        LPCWSTR    szName,
        BOOL       bActive,
        DWORD      dwRequestedUpdateRate,
        OPCHANDLE  hClientGroup,
        LONG*      pTimeBias,
        FLOAT*     pPercentDeadband,
        DWORD      dwLCID,
        OPCHANDLE* phServerGroup,
        DWORD*     pRevisedUpdateRate,
        REFIID     riid,
        LPUNKNOWN* ppUnk
    );

    // GetErrorString
    STDMETHODIMP GetErrorString( 
        HRESULT dwError,
        LCID    dwLocale,
        LPWSTR* ppString
    );

    // GetGroupByName
    STDMETHODIMP GetGroupByName(
        LPCWSTR    szName,
        REFIID     riid,
        LPUNKNOWN* ppUnk
    );

    // GetStatus
    STDMETHODIMP GetStatus( 
        OPCSERVERSTATUS** ppServerStatus
    );

    // RemoveGroup
    STDMETHODIMP RemoveGroup(
        OPCHANDLE hServerGroup,
        BOOL      bForce
    );

    // CreateGroupEnumerator
    STDMETHODIMP CreateGroupEnumerator(
        OPCENUMSCOPE dwScope, 
        REFIID       riid, 
        LPUNKNOWN*   ppUnk
    );

    //=========================================================================
    // IOPCBrowseServerAddressSpace
    
    // QueryOrganization
    STDMETHODIMP QueryOrganization(OPCNAMESPACETYPE* pNameSpaceType);
    
    // ChangeBrowsePosition
    STDMETHODIMP ChangeBrowsePosition(
        OPCBROWSEDIRECTION dwBrowseDirection,  
        LPCWSTR            szString
    );

    // BrowseOPCItemIDs
    STDMETHODIMP BrowseOPCItemIDs(
        OPCBROWSETYPE   dwBrowseFilterType,
        LPCWSTR         szFilterCriteria,  
        VARTYPE         vtDataTypeFilter,     
        DWORD           dwAccessRightsFilter,
        LPENUMSTRING*   ppIEnumString
    );

    // GetItemID
    STDMETHODIMP GetItemID(
        LPWSTR  wszItemName,
        LPWSTR* pszItemID
    );

    // BrowseAccessPaths
    STDMETHODIMP BrowseAccessPaths(
        LPCWSTR       szItemID,  
        LPENUMSTRING* ppIEnumString
    );

    //=========================================================================
    // IOPCItemProperties

    // QueryAvailableProperties
    STDMETHODIMP QueryAvailableProperties( 
        LPWSTR     szItemID,
        DWORD    * pdwCount,
        DWORD   ** ppPropertyIDs,
        LPWSTR  ** ppDescriptions,
        VARTYPE ** ppvtDataTypes
    );

    // GetItemProperties
    STDMETHODIMP GetItemProperties ( 
        LPWSTR     szItemID,
        DWORD      dwCount,
        DWORD    * pdwPropertyIDs,
        VARIANT ** ppvData,
        HRESULT ** ppErrors
    );

    // LookupItemIDs
    STDMETHODIMP LookupItemIDs( 
        LPWSTR     szItemID,
        DWORD      dwCount,
        DWORD    * pdwPropertyIDs,
        LPWSTR  ** ppszNewItemIDs,
        HRESULT ** ppErrors
    );

    //=========================================================================
    // IOPCBrowse

    // GetProperties
    STDMETHODIMP GetProperties( 
        DWORD		        dwItemCount,
        LPWSTR*             pszItemIDs,
        BOOL		        bReturnPropertyValues,
        DWORD		        dwPropertyCount,
        DWORD*              pdwPropertyIDs,
        OPCITEMPROPERTIES** ppItemProperties 
    );

    // Browse
    STDMETHODIMP Browse(
	    LPWSTR	           szItemName,
	    LPWSTR*	           pszContinuationPoint,
	    DWORD              dwMaxElementsReturned,
		OPCBROWSEFILTER    dwFilter,
	    LPWSTR             szElementNameFilter,
	    LPWSTR             szVendorFilter,
	    BOOL               bReturnAllProperties,
	    BOOL               bReturnPropertyValues,
	    DWORD              dwPropertyCount,
	    DWORD*             pdwPropertyIDs,
	    BOOL*              pbMoreElements,
	    DWORD*	           pdwCount,
	    OPCBROWSEELEMENT** ppBrowseElements
    );
    
    //=========================================================================
    // IOPCItemIO

    // Read
    STDMETHODIMP Read(
        DWORD       dwCount, 
        LPCWSTR   * pszItemIDs,
        DWORD     * pdwMaxAge,
        VARIANT  ** ppvValues,
        WORD     ** ppwQualities,
        FILETIME ** ppftTimeStamps,
        HRESULT  ** ppErrors
    );

    // WriteVQT
    STDMETHODIMP WriteVQT(
        DWORD         dwCount, 
        LPCWSTR    *  pszItemIDs,
        OPCITEMVQT *  pItemVQT,
        HRESULT    ** ppErrors
    );

private:
        
    //==========================================================================
    // Private Members

    COpcString      m_cBrowsePath;
    COpcDaGroupMap  m_cGroups;
    OPCSERVERSTATUS m_cStatus;
	UINT            m_uCounter;
};

#endif // _COpcDaServer_H_
