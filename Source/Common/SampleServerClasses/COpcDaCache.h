//============================================================================
// TITLE: COpcDaCache.h
//
// CONTENTS:
// 
// The global item cache for an OPC server.
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
// 2002/11/16 RSA   Second release.
// 2003/03/22 RSA   Added support for complex data.
// 2003/04/08 RSA   Added check to prevent item ids from being returned for basic properties.
// 2003/06/25 RSA   Fixed memory problems and fetched server info from resource block.

#ifndef _COpcDaCache_H_
#define _COpcDaCache_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "opcda.h"

#include "COpcDaBrowseElement.h"
#include "COpcDaCacheItem.h"
#include "COpcDaWriteThread.h"
#include "COpcDaUpdateThread.h"
#include "OpcDaHelpers.h"
#include "COpcDaTypeDictionary.h"
#include "IOpcDaDevice.h"

//============================================================================
// CLASS:   COpcDaCache
// PURPOSE: Maintains an in memory cache of DA items.

class COpcDaCache : public IOpcDaDevice, public COpcSynchObject
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcDaCache();

    // Destructor
    ~COpcDaCache();
    
	//=========================================================================
    // Configuration Functions

	// Start
	virtual bool Start();
	
	// Stop
	virtual void Stop();

	// Load
	virtual bool Load(const COpcString& cFileName);

	// Save
	virtual bool Save(const COpcString& cFileName, const COpcString& cDefaultNamespace);

	// Load
	virtual bool Load(COpcXmlElement& cRoot);

	// Save
	virtual bool Save(COpcXmlElement& cRoot);
	
	// GetState
	OPCSERVERSTATE GetState() const;

	// SetState
	void SetState(OPCSERVERSTATE eState);

	// GetVersionInfo
	void GetVersionInfo(OpcDaVersionInfo& cInfo);

	//=========================================================================
    // Browsing Functions

    // BrowseUp
    bool BrowseUp(
        const COpcString& cOldPath, 
        COpcString&       cNewPath
    );

    // BrowseDown
    bool BrowseDown(
        const COpcString& cOldPath, 
        const COpcString& cName, 
        COpcString&       cNewPath
    );

    // BrowseTo
    bool BrowseTo(
        const COpcString& cItemID, 
        COpcString&       cNewPath
    );

    // GetItemID
    bool GetItemID(
        const COpcString& cPath, 
        const COpcString& cName, 
        COpcString&       cItemID
    );

    // Browse
    bool Browse(
        const COpcString& cFullPath, 
        OPCBROWSETYPE     eType, 
        const COpcString& cFilterCriteria,  
        VARTYPE           vtDataTypeFilter,     
        DWORD             dwAccessRightsFilter,
        COpcStringList&   cNodes
    );

    // Browse
    HRESULT Browse(
	    const COpcString&  cPath,
	    DWORD              dwMaxElements,
		OPCBROWSEFILTER    dwFilter,
	    const COpcString&  cNameFilter,
	    const COpcString&  cVendorFilter,
        DWORD*             pdwStartIndex,
	    DWORD*	           pdwCount,
	    OPCBROWSEELEMENT** ppBrowseElements
    );
       
    //=========================================================================
    // Item Access
    
    // AddItem
    bool AddItem(const COpcString& cItemID);

    // RemoveItem
    bool RemoveItem(const COpcString& cItemID);

    // AddItemAndLink
    bool AddItemAndLink(const COpcString& cBrowsePath);
    
    // RemoveItemAndLink
    bool RemoveItemAndLink(const COpcString& cBrowsePath);

    // AddLink
    bool AddLink(const COpcString& cBrowsePath);

    // AddLink
    bool AddLink(const COpcString& cBrowsePath, const COpcString& cItemID);
    
    // RemoveLink
    bool RemoveLink(const COpcString& cBrowsePath);
    
    // RemoveEmptyLink
    bool RemoveEmptyLink(const COpcString& cBrowsePath);

    // ItemExists
    bool ItemExists(const COpcString& cItemID);

    // LinkExists
    bool LinkExists(const COpcString& cBrowsePath);

	// GetItemResult
	HRESULT GetItemResult(
		const COpcString& cItemID, 
		OPCITEMRESULT&    cResult
	);

	// GetItemAttributes
	HRESULT GetItemAttributes(
		const COpcString&  cItemID, 
		OPCITEMATTRIBUTES& cAttributes
	);

	// GetItemProperty
    HRESULT GetItemProperty(
		const COpcString&  cItemID, 
        DWORD              dwProperty,
        VARIANT&           cValue
    );

	// GetItemProperties
    HRESULT GetItemProperties(
        const COpcString&   cItemID, 
		bool                bReturnValues,
		COpcDaPropertyList& cProperties
	);

	// GetItemProperties
    HRESULT GetItemProperties(
        const COpcString&      cItemID,
		const COpcList<DWORD>& cIDs,
		bool                   bReturnValues,
		COpcDaPropertyList&    cProperties
	);

	// Read
	HRESULT Read(
		const COpcString& cItemID, 
		LCID              lcid,
		VARTYPE           vtReqType,
        DWORD             dwMaxAge,
        VARIANT&          cValue,
        FILETIME&         ftTimestamp,
        WORD&             wQuality
	);

	// Write
	HRESULT Write(
		const COpcString& cItemID,
		LCID              lcid,
        VARIANT&          cValue,
        FILETIME*         pftTimestamp = NULL,
        WORD*             pwQuality    = NULL
	);

    //========================================================================
    // Message Queue

    // QueueMessage
	void QueueMessage(COpcMessage* pMsg);

    //========================================================================
    // Cache Updates

	// RegisterForUpdates
	void RegisterForUpdates(COpcDaGroup* pGroup)
	{
		m_cUpdateThread.Register(pGroup);
	}

	// UnregisterForUpdates
	void UnregisterForUpdates(COpcDaGroup* pGroup)
	{
		m_cUpdateThread.Unregister(pGroup);
	}

	//========================================================================
    // Complex Data
	
	// creates a type dictionary (if it does not already exist) from the specified file.
	COpcDaTypeDictionary* CreateTypeDictionary(const COpcString& cFileName);

	// gets the type dictionary referenced by the item id.
	COpcDaTypeDictionary* GetTypeDictionary(const COpcString& cItemID);	
	
	// creates an XML schema mapping (if it does not already exist) from the specified file.
	COpcDaTypeDictionary* CreateXmlSchemaMapping(const COpcString& cFileName);
    
	//========================================================================
    // IOpcDaDevice

	// BuildAddressSpace
	virtual bool BuildAddressSpace();

	// ClearAddressSpace
	virtual void ClearAddressSpace();

	// IsKnownItem
	virtual bool IsKnownItem(const COpcString& cItemID);

	// GetAvailableProperties
    virtual HRESULT GetAvailableProperties(
        const COpcString&   cItemID, 
		bool                bReturnValues,
		COpcDaPropertyList& cProperties);

	// GetAvailableProperties
    virtual HRESULT GetAvailableProperties(
        const COpcString&      cItemID,
		const COpcList<DWORD>& cIDs,
		bool                   bReturnValues,
		COpcDaPropertyList&    cProperties);

	// Read
	virtual HRESULT Read(
        const COpcString& cItemID, 
        DWORD             dwPropertyID,
        VARIANT&          cValue, 
        FILETIME*         pftTimestamp = NULL,
        WORD*             pwQuality = NULL);

	// Write
	virtual HRESULT Write(
        const COpcString& cItemID, 
        DWORD             dwPropertyID,
        const VARIANT&    cValue, 
        FILETIME*         pftTimestamp = NULL,
        WORD*             pwQuality = NULL);

	// returns the device object for the specified item.
	virtual IOpcDaDevice* GetDevice(const COpcString& cItemID);

private:

    //========================================================================
    // Private Members

	OpcDaVersionInfo        m_cVersionInfo;
    
    COpcDaBrowseElement*    m_pAddressSpace;
	COpcDaCacheItemMap      m_cItems;
    OPCSERVERSTATE          m_eState;
	
	COpcThreadPool        m_cQueue;
	COpcDaWriteThread       m_cWriteThread;
	COpcDaUpdateThread      m_cUpdateThread;

	IXMLDOMElement*         m_ipSelfRegInfo;

	COpcDaTypeDictionaryMap m_cDictionaries;
};

//============================================================================
// FUNCTION: GetCache
// PURPOSE:  Returns a reference (must never be null) to the global cache.

extern COpcDaCache& GetCache();

#endif // _COpcDaCache_H_