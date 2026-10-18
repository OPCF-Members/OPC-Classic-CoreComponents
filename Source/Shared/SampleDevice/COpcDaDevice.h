/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcDaDevice_H_
#define _COpcDaDevice_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcThread.h"
#include "COpcDaDeviceItem.h"
#include "COpcDaCache.h"
#include "COpcDaTypeDictionary.h"

//============================================================================
// CLASS:   COpcDaDevice
// PURPOSE: Simulates an I/O device.

class COpcDaDevice : 
	public IOpcXmlSerialize, 
	public IOpcDaDevice,
    public COpcSynchObject
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcDaDevice() { Init(); }

    // Destructor
    ~COpcDaDevice() { Clear(); }

    //========================================================================
    // Public Methods

    // Start
    bool Start();
    bool Start(COpcXmlElement& cElement);

    // Stop
    void Stop();

	// Update
	void Update();

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

    //========================================================================
    // IOpcXmlSerialize
    
    // Init
    virtual void Init();

    // Clear
    virtual void Clear();

    // Read
    virtual bool Read(COpcXmlElement& cElement);

    // Write
    virtual bool Write(COpcXmlElement& cElement);

protected:

    //========================================================================
    // Protected Methods

	// GetDeviceItem
	COpcDaDeviceItem* GetDeviceItem(const COpcString& cItemID);

private:

    //========================================================================
    // Private Methods

	// Read
	bool Read(const COpcString& cElementPath, COpcXmlElement& cElement);
	
	// ParseItemID
	COpcDaDeviceItem* ParseItemID(const COpcString& cItemID, COpcString& cItemPath);

    //========================================================================
    // Private Members

    COpcDaDeviceItemMap                 m_cItems;
	COpcStringList                      m_cItemList;
	COpcMap<COpcString,COpcStringList*> m_cBranches;
	COpcThread                          m_cThread;
	COpcDaTypeDictionaryMap             m_cDictionaries;
};
	
#endif // _COpcDaDevice_H_
