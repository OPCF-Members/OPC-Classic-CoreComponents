//============================================================================
// TITLE: COpcDaCacheItem.h
//
// CONTENTS:
// 
// A single item in the global cache for an OPC server.
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
//

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