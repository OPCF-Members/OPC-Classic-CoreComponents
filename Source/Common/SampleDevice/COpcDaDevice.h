//============================================================================
// TITLE: COpcDaDevice.h
//
// CONTENTS:
// 
// Simulates a simple I/O device.
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
//

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
