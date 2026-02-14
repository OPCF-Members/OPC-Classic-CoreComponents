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
