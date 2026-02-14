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

#ifndef _COpcDaCacheItem_H_
#define _COpcDaCacheItem_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class COpcDaCacheItem;
interface IOpcDaDevice;

#include "COpcDaProperty.h"

//============================================================================
// MACROS:  OPC_READ_ONLY/OPC_WRITE_ONLY
// PURPOSE: Define contants that combine access rights masks.

#define OPC_READ_ONLY  (DWORD)OPC_READABLE
#define OPC_WRITE_ONLY (DWORD)OPC_WRITEABLE
#define OPC_READ_WRITE (DWORD)(OPC_READABLE | OPC_WRITEABLE)

//============================================================================
// CLASS:   COpcDaCacheItem
// PURPOSE: Describes the current state of an item.

class COpcDaCacheItem
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcDaCacheItem(const COpcString& cItemID, IOpcDaDevice* pDevice);
            
    // Destructor
    ~COpcDaCacheItem() { Clear();}

    //========================================================================
    // Public Methods

	// Init
	void Init();

	// Clear
	void Clear();
	
    // GetItemID
	const COpcString& GetItemID() const { return m_cItemID; }

    //========================================================================
    // Property Access

    // GetDataType
    VARTYPE GetDataType();

    // GetAccessRights
    DWORD GetAccessRights();

    // GetEUType
    OPCEUTYPE GetEUType();

    // GetItemResult
    void GetItemResult(OPCITEMRESULT& cResult);

    // GetItemAttributes
    void GetItemAttributes(OPCITEMATTRIBUTES& cAttributes);

	// GetProperties
    HRESULT GetProperties(
		bool                bReturnValues,
		COpcDaPropertyList& cProperties);

	// GetProperties
    HRESULT GetProperties(
		const COpcList<DWORD>& cIDs,
		bool                   bReturnValues,
		COpcDaPropertyList&    cProperties);

	// GetProperty
	HRESULT GetProperty(DWORD dwPropertyID, VARIANT& cValue);

    //========================================================================
    // Data Access

	// Refresh
	HRESULT Refresh();

	// Read
	HRESULT Read(
		LCID      lcid,
		VARTYPE   vtReqType,
        DWORD     dwMaxAge,
        DWORD     dwPropertyID,
		VARIANT&  cValue,
		FILETIME& ftTimestamp,
		WORD&     wQuality
	);

	// Write
	HRESULT Write(
		LCID      lcid,
        DWORD     dwPropertyID,
	    VARIANT&  cValue,
		FILETIME* pftTimestamp = NULL,
		WORD*     pwQuality    = NULL
	);

protected:

	//========================================================================
    // Protected Methods

    // GetLimits
    virtual bool GetLimits(double& dblMinValue, double& dblMaxValue);

    // GetEnumValues
    virtual bool GetEnumValues(COpcStringArray& cEnumValues);

    // FindEnumIndex
    virtual int COpcDaCacheItem::FindEnumIndex(
        COpcStringArray& cEnumValues, 
        LPCWSTR          szEnumValue
    );

    // FindEnumValue
    virtual BSTR COpcDaCacheItem::FindEnumValue(
        COpcStringArray& cEnumValues, 
        int              iEnumIndex
    );

    // ToEnumValue
    virtual HRESULT ToEnumValue(	
	    VARIANT&       cDst,
        const VARIANT& cSrc
    );

    // FromEnumValue
    virtual HRESULT FromEnumValue(
	    VARIANT&       cDst,
        const VARIANT& cSrc
    );

private:

    //========================================================================
    // Private Members

    IOpcDaDevice*      m_pDevice;
	COpcString         m_cItemID;

	COpcVariant        m_cValue;
	VARTYPE            m_vtDataType;
	FILETIME           m_ftTimestamp;
	WORD               m_wQuality;
};

//============================================================================
// TYPE:    COpcDaCacheItemMap
// PURPOSE: A table of items indexed by item id.

typedef COpcMap<COpcString,COpcDaCacheItem*> COpcDaCacheItemMap;

#endif // _COpcDaCacheItem_H_
