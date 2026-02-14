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

#ifndef _COpcDaGroup_H_
#define _COpcDaGroup_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class COpcDaServer;
class COpcDaTransaction;

#include "COpcDaGroupItem.h"

//============================================================================
// CLASS:   COpcDaGroup
// PURPOSE: A class that implements the IOPCServer interface.
// NOTES:

class COpcDaGroup :
    public COpcComObject,
    public COpcCPContainer,
    public IOPCItemMgt,
    public IOPCItemDeadbandMgt,
    public IOPCItemSamplingMgt,
    public IOPCSyncIO2,
    public IOPCAsyncIO3,
    public IOPCGroupStateMgt2,
	public IOpcMessageCallback,
    public COpcSynchObject

{
    OPC_CLASS_NEW_DELETE()

    OPC_BEGIN_INTERFACE_TABLE(COpcDaGroup)
        OPC_INTERFACE_ENTRY(IConnectionPointContainer)
        OPC_INTERFACE_ENTRY(IOPCItemMgt)
        OPC_INTERFACE_ENTRY(IOPCItemDeadbandMgt)
        OPC_INTERFACE_ENTRY(IOPCItemSamplingMgt)
        OPC_INTERFACE_ENTRY(IOPCSyncIO)
        OPC_INTERFACE_ENTRY(IOPCSyncIO2)
        OPC_INTERFACE_ENTRY(IOPCAsyncIO2)
        OPC_INTERFACE_ENTRY(IOPCAsyncIO3)
        OPC_INTERFACE_ENTRY(IOPCGroupStateMgt)
        OPC_INTERFACE_ENTRY(IOPCGroupStateMgt2)
    OPC_END_INTERFACE_TABLE()

public:

    //=========================================================================
    // Operators

    // Constructor
	COpcDaGroup();
    COpcDaGroup(COpcDaServer& cServer, const COpcString& cName);

    // Destructor 
    ~COpcDaGroup() { Clear(); }

    //=========================================================================
    // Public Methods

	// Init
	void Init();

	// Clear
	void Clear();

	// Initialize
	HRESULT Initialize(COpcDaGroup& cGroup);

    // ProcessTransaction
    void ProcessTransaction(COpcDaTransaction& cTransaction);

    // Update
    void Update(LONGLONG uCount, UINT uInterval);

    // Delete
    bool Delete();

    // GetHandle
    OPCHANDLE GetHandle() const { return m_hServer; }
	
	//=========================================================================
    // IOpcMessageCallback

	// ProcessMessage
	virtual void ProcessMessage(COpcMessage& cMsg);

    //=========================================================================
    // IOPCGroupStateMgt

    // GetState
    STDMETHODIMP  GetState(
        DWORD     * pUpdateRate, 
        BOOL      * pActive, 
        LPWSTR    * ppName,
        LONG      * pTimeBias,
        FLOAT     * pPercentDeadband,
        DWORD     * pLCID,
        OPCHANDLE * phClientGroup,
        OPCHANDLE * phServerGroup
    );

    // SetState
    STDMETHODIMP  SetState( 
        DWORD     * pRequestedUpdateRate, 
        DWORD     * pRevisedUpdateRate, 
        BOOL      * pActive, 
        LONG      * pTimeBias,
        FLOAT     * pPercentDeadband,
        DWORD     * pLCID,
        OPCHANDLE * phClientGroup
    );

    // SetName
    STDMETHODIMP  SetName( 
        LPCWSTR szName
    );

    // CloneGroup
    STDMETHODIMP  CloneGroup(
        LPCWSTR     szName,
        REFIID      riid,
        LPUNKNOWN * ppUnk
    );
    
	//=========================================================================
    // IOPCGroupStateMgt2

	// SetKeepAlive
    STDMETHODIMP SetKeepAlive( 
        DWORD   dwKeepAliveTime,
        DWORD * pdwRevisedKeepAliveTime 
    );

	// GetKeepAlive
    STDMETHODIMP GetKeepAlive( 
        DWORD * pdwKeepAliveTime 
    );

    //=========================================================================
    // IOPCItemMgt

    // AddItems
    STDMETHODIMP  AddItems( 
        DWORD            dwCount,
        OPCITEMDEF     * pItemArray,
        OPCITEMRESULT ** ppAddResults,
        HRESULT       ** ppErrors
    );

    // ValidateItems
    STDMETHODIMP  ValidateItems( 
        DWORD             dwCount,
        OPCITEMDEF      * pItemArray,
        BOOL              bBlobUpdate,
        OPCITEMRESULT  ** ppValidationResults,
        HRESULT        ** ppErrors
    );

    // RemoveItems
    STDMETHODIMP  RemoveItems( 
        DWORD        dwCount,
        OPCHANDLE  * phServer,
        HRESULT   ** ppErrors
    );

    // SetActiveState
    STDMETHODIMP  SetActiveState(
        DWORD        dwCount,
        OPCHANDLE  * phServer,
        BOOL         bActive, 
        HRESULT   ** ppErrors
    );

    // SetClientHandles
    STDMETHODIMP  SetClientHandles(
        DWORD        dwCount,
        OPCHANDLE  * phServer,
        OPCHANDLE  * phClient,
        HRESULT   ** ppErrors
    );

    // SetDatatypes
    STDMETHODIMP  SetDatatypes(
        DWORD        dwCount,
        OPCHANDLE  * phServer,
        VARTYPE    * pRequestedDatatypes,
        HRESULT   ** ppErrors
    );

    // CreateEnumerator
    STDMETHODIMP  CreateEnumerator(
        REFIID      riid,
        LPUNKNOWN * ppUnk
    );

    //=========================================================================
    // IOPCAsyncIO2

    // Read
    STDMETHODIMP  Read(
        DWORD           dwCount,
        OPCHANDLE     * phServer,
        DWORD           dwTransactionID,
        DWORD         * pdwCancelID,
        HRESULT      ** ppErrors
    );

    // Write
    STDMETHODIMP  Write(
        DWORD           dwCount, 
        OPCHANDLE     * phServer,
        VARIANT       * pItemValues, 
        DWORD           dwTransactionID,
        DWORD         * pdwCancelID,
        HRESULT      ** ppErrors
    );

    // Refresh2
    STDMETHODIMP  Refresh2(
        OPCDATASOURCE   dwSource,
        DWORD           dwTransactionID,
        DWORD         * pdwCancelID
    );

    // Cancel2
    STDMETHODIMP  Cancel2(DWORD dwCancelID);

    // SetEnable
    STDMETHODIMP  SetEnable(BOOL bEnable);

    // GetEnable
    STDMETHODIMP  GetEnable(BOOL* pbEnable);

    //=========================================================================
    // IOPCItemDeadbandMgt

    // SetItemDeadband
    STDMETHODIMP SetItemDeadband( 
        DWORD 	    dwCount,
        OPCHANDLE * phServer,
        FLOAT     * pPercentDeadband,
        HRESULT  ** ppErrors
    );

    // GetItemDeadband
    STDMETHODIMP GetItemDeadband( 
        DWORD 	    dwCount,
        OPCHANDLE * phServer,
        FLOAT    ** ppPercentDeadband,
        HRESULT  ** ppErrors
    );

    // ClearItemDeadband
    STDMETHODIMP ClearItemDeadband(
        DWORD       dwCount,
        OPCHANDLE * phServer,
        HRESULT  ** ppErrors
    );

    //=========================================================================
    // IOPCItemSamplingMgt

    // SetItemSamplingRate
    STDMETHODIMP SetItemSamplingRate(
        DWORD 	    dwCount,
        OPCHANDLE * phServer,
        DWORD     * pdwRequestedSamplingRate,
        DWORD    ** ppdwRevisedSamplingRate,
        HRESULT  ** ppErrors
    );

    // GetItemSamplingRate
    STDMETHODIMP GetItemSamplingRate(
        DWORD 	    dwCount,
        OPCHANDLE * phServer,
        DWORD    ** ppdwSamplingRate,
        HRESULT  ** ppErrors
    );

    // ClearItemSamplingRate
    STDMETHODIMP ClearItemSamplingRate(
        DWORD 	    dwCount,
        OPCHANDLE * phServer,
        HRESULT  ** ppErrors
    );

    // SetItemBufferEnable
    STDMETHODIMP SetItemBufferEnable(
        DWORD       dwCount, 
        OPCHANDLE * phServer, 
        BOOL      * pbEnable,
        HRESULT  ** ppErrors
    );

    // GetItemBufferEnable
    STDMETHODIMP GetItemBufferEnable(
        DWORD       dwCount, 
        OPCHANDLE * phServer, 
        BOOL     ** ppbEnable,
        HRESULT  ** ppErrors
    );

    //=========================================================================
    // IOPCSyncIO2

    // Read
    STDMETHODIMP  Read(
        OPCDATASOURCE   dwSource,
        DWORD           dwCount, 
        OPCHANDLE     * phServer, 
        OPCITEMSTATE ** ppItemValues,
        HRESULT      ** ppErrors
    );

    // Write
    STDMETHODIMP  Write(
        DWORD        dwCount, 
        OPCHANDLE  * phServer, 
        VARIANT    * pItemValues, 
        HRESULT   ** ppErrors
    );

    // ReadMaxAge
    STDMETHODIMP ReadMaxAge(
        DWORD       dwCount, 
        OPCHANDLE * phServer, 
        DWORD     * pdwMaxAge,
        VARIANT  ** ppvValues,
        WORD     ** ppwQualities,
        FILETIME ** ppftTimeStamps,
        HRESULT  ** ppErrors
    );

    // WriteVQT
    STDMETHODIMP WriteVQT(
        DWORD         dwCount, 
        OPCHANDLE  *  phServer, 
        OPCITEMVQT *  pItemVQT,
        HRESULT    ** ppErrors
    );
    
	//=========================================================================
    // IOPCAsyncIO3

    STDMETHODIMP ReadMaxAge(
        DWORD       dwCount, 
        OPCHANDLE * phServer,
        DWORD     * pdwMaxAge,
        DWORD       dwTransactionID,
        DWORD     * pdwCancelID,
        HRESULT  ** ppErrors
    );

    STDMETHODIMP WriteVQT(
        DWORD        dwCount, 
        OPCHANDLE  * phServer,
        OPCITEMVQT * pItemVQT,
        DWORD        dwTransactionID,
        DWORD      * pdwCancelID,
        HRESULT   ** ppErrors
    );

    STDMETHODIMP RefreshMaxAge(
        DWORD   dwMaxAge,
        DWORD   dwTransactionID,
        DWORD * pdwCancelID
    );

private:

    //=========================================================================
    // Private Methods
    
    // Read
    HRESULT Read(
        DWORD        dwCount, 
        OPCHANDLE *  phServer, 
        DWORD     *  pdwMaxAge,
        OPCHANDLE ** pphClient,
        VARIANT   ** ppvValues,
        WORD      ** ppwQualities,
        FILETIME  ** ppftTimeStamps,
        HRESULT   ** ppErrors
    );

    // Write
    HRESULT Write(
        DWORD       dwCount, 
        OPCHANDLE*  phServer, 
        VARIANT*    pItemValues, 
        OPCHANDLE** phClient,
        HRESULT**   ppErrors
    );

    // WriteVQT
    STDMETHODIMP WriteVQT(
        DWORD         dwCount, 
        OPCHANDLE  *  phServer, 
        OPCITEMVQT *  pItemVQT,
        OPCHANDLE  ** phClient,
        HRESULT    ** ppErrors
    );

    // FindItem
    COpcDaGroupItem* FindItem(OPCHANDLE hServer);

    // RemoveItem
    bool RemoveItem(OPCHANDLE hServer);

    //=========================================================================
    // Private Members

	COpcDaServer&          m_cServer;
	COpcString             m_cName;
    OPCHANDLE              m_hServer;
    OPCHANDLE              m_hClient;

    BOOL                   m_bActive;
	BOOL				   m_bEnabled;
    BOOL                   m_bDeleted;
    DWORD                  m_dwUpdateRate;
    LONG                   m_lTimeBias;
    FLOAT                  m_fltDeadband;
    DWORD                  m_dwLCID;

    DWORD                  m_dwKeepAlive;
    FILETIME               m_ftLastUpdate;
	LONGLONG               m_uTickOffset;
 
    COpcDaGroupItemMap     m_cItems;
	COpcMap<DWORD,bool>    m_cTransactions;
};

//============================================================================
// TYPE:    COpcDaGroupMap
// PURPOSE: A table of groups indexed by group name.

typedef COpcMap<COpcString,COpcDaGroup*> COpcDaGroupMap;

#endif // _COpcDaGroup_H_
