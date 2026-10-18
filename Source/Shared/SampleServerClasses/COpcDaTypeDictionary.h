/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcDaTypeDictionary_H_
#define _COpcDaTypeDictionary_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IOpcDaDevice.h"
#include "COpcBinary.h"

#define CPX_DATABASE_ROOT _T("CPX")
#define CPX_DATA_FILTERS  _T("DataFilters")

//============================================================================
// CLASS:   COpcDaTypeDictionary
// PURPOSE: Manages complex type items and complex type descriptions.

class COpcDaTypeDictionary : 
	public IOpcDaDevice,
    public COpcSynchObject
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
	COpcDaTypeDictionary();
            
    // Destructor
	~COpcDaTypeDictionary();

	//========================================================================
    // Public Methods

	// Start
	bool Start(const COpcString& cFileName, bool bXmlSchemaMapping = false);
		
	// Stop
	void Stop();
	
	// GetFileName
	COpcString GetFileName();

	// GetItemID
	COpcString GetItemID();

	// GetTypeSystemID
	COpcString GetTypeSystemID();

	// GetDictionaryID
	COpcString GetDictionaryID();

	// GetTypeID
	COpcString GetTypeID(const COpcString& cTypeName);

	// GetTypeItemID
	COpcString GetTypeItemID(const COpcString& cTypeName);

	// GetBinaryDictionary
	COpcTypeDictionary* GetBinaryDictionary();

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
		

private:
    
	//========================================================================
    // Private Methods

	// ValidatePropertyID
	HRESULT ValidatePropertyID(
		const COpcString& cItemID, 
		DWORD             dwPropertyID, 
		int               iAccessRequired
	);

	// GetValue
	HRESULT GetValue(
		const COpcString& cItemID, 
		DWORD             dwPropertyID,
		VARIANT&          cValue
	);

	// DetectTypes
	bool DetectTypes();
	
	// LoadBinaryDictionary
	bool LoadBinaryDictionary();

	// CreateXmlSchemaMapping
	bool CreateXmlSchemaMapping();

    //========================================================================
    // Private Members
	
	COpcString         m_cFileName;
	COpcString         m_cItemID;

	COpcString         m_cTypeSystemID;
	COpcString         m_cDictionaryName;
	
	COpcXmlDocument    m_cDictionary;

	COpcString         m_cDictionaryID;
	COpcStringMap      m_cTypeXPaths;

	COpcTypeDictionary m_cBinaryDictionary;
};

//============================================================================
// TYPE:    COpcDaTypeDictionaryMap
// PURPOSE: A table type dictionaries indexed by a string.

typedef COpcMap<COpcString,COpcDaTypeDictionary*> COpcDaTypeDictionaryMap;

#endif // _COpcDaTypeDictionary_H_
