//============================================================================
// TITLE: COpcDaProperty.h
//
// CONTENTS:
// 
// Constants that describe all well known item properties.
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

#ifndef _COpcDaProperty_H_
#define _COpcDaProperty_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcMap.h"
#include "COpcXmlElement.h"

class COpcDaProperty;

//============================================================================
// TYPE:    COpcDaPropertyList
// PURPOSE: An list of item properties.

typedef COpcArray<COpcDaProperty*> COpcDaPropertyList;
template class COpcArray<COpcDaProperty*>;

//============================================================================
// CLASS:   COpcDaProperty
// PURPOSE: A class that describes a property.

class COpcDaProperty
{
    OPC_CLASS_NEW_DELETE_ARRAY()

public:

	//==========================================================================
	// Public Operators

	// Constructor
	COpcDaProperty(DWORD dwID);

	// Copy Constructor
	COpcDaProperty(const COpcDaProperty& cProperty);

	// Destructor
	~COpcDaProperty();

	// Assignment.
	COpcDaProperty& operator=(const COpcDaProperty& cProperty);

	//==========================================================================
	// Public Methods

	// ID
	DWORD GetID() const { return m_dwID; }
	void SetID(DWORD dwID) { m_dwID = dwID; }

	// ItemID
	const COpcString& GetItemID() const { return m_cItemID; }
	void SetItemID(const COpcString& cItemID) { m_cItemID = cItemID; }

	// Description
	const COpcString& GetDescription() const { return m_cDescription; }
	void SetDescription(const COpcString& cDescription) { m_cDescription = cDescription; }

	// DataType
	VARTYPE GetDataType() const { return m_vtDataType; }
	void SetDataType(VARTYPE vtDataType) { m_vtDataType = vtDataType; }

	// Value
	VARIANT& GetValue() { return m_cValue.GetRef(); }
	void SetValue(const VARIANT& cValue) { m_cValue = cValue; }

	// Error
	HRESULT GetError() const { return m_hError; }
	void SetError(HRESULT HRESULT) { m_hError = HRESULT; }
	
	//==========================================================================
	// Static Methods

	// initializes a list of properties from a list of ids.
	static void Create(
		const COpcList<DWORD>& cIDs, 
		COpcDaPropertyList&    cProperties
	);

	// copies the property information into separate arrays.
	static void Copy(
		const COpcDaPropertyList& cProperties,
        DWORD&                    dwCount,
        DWORD*&                   pPropertyIDs,
        LPWSTR*&                  pDescriptions,
        VARTYPE*&                 pvtDataTypes
	);

	// copies the property values into a separate array.
	static void Copy(
		const COpcDaPropertyList& cProperties,
        DWORD&                    dwCount,
        VARIANT*&                 pValues,
		HRESULT*&                 pErrors
	);
	
	// copies the property item ids into separate arrays.
	static void Copy(
		const COpcDaPropertyList& cProperties,
        DWORD&                    dwCount,
        LPWSTR*&                  pItemIDs,
		HRESULT*&                 pErrors
	);

	// copies a list of properties into an array of OPCITEMPROPERTY structures.
	static void Copy(
		const COpcDaPropertyList& cProperties,
		bool                      bReturnValues,
		DWORD&                    dwCount,
		OPCITEMPROPERTY*&         pProperties
	);

	// frees a property list.
	static void Free(COpcDaPropertyList& cProperties);

	// looks up the localized text for a string.
	static LPWSTR LocalizeText(LPCWSTR szString, DWORD dwLocaleId, LPCWSTR  szUserName);

	// looks up the original text for a localized text.
	static LPWSTR DeLocalizeText(LPCWSTR szString, DWORD dwLocaleId, LPCWSTR  szUserName);

	// LocalizeVARIANT
	static void COpcDaProperty::LocalizeVARIANT( 
		DWORD    dwLocaleID,
		LPCWSTR  szUserName,
		VARIANT* pvData);

	// LocalizeVARIANTs
	static void COpcDaProperty::LocalizeVARIANTs( 
		DWORD    dwLocaleID,
		LPCWSTR  szUserName,
		DWORD    dwCount,
		VARIANT* pvData);

	// LocalizeOPCBROWSEELEMENT
	static void COpcDaProperty::LocalizeOPCBROWSEELEMENT(
		DWORD			  dwLocaleID,
		LPCWSTR			  szUserName,
		DWORD	          dwCount,
		OPCBROWSEELEMENT* pBrowseElements);

	// LocalizeOPCITEMPROPERTIES
	static void COpcDaProperty::LocalizeOPCITEMPROPERTIES( 
		DWORD			   dwLocaleID,
		LPCWSTR			   szUserName,
		DWORD		       dwPropertyCount,
		OPCITEMPROPERTIES* pItemProperties);

	// LocalizeOPCITEMSTATE
	static void COpcDaProperty::LocalizeOPCITEMSTATE( 
		DWORD         dwLocaleID,
		LPCWSTR       szUserName,
		DWORD         dwCount,
		OPCITEMSTATE* pValues);

private:

	DWORD       m_dwID;
	COpcString  m_cItemID;
	COpcString  m_cDescription;
	VARTYPE     m_vtDataType;
	COpcVariant m_cValue;
	HRESULT     m_hError;
};

//============================================================================
// FUNCTION: OpcGetPropertyType
// PURPOSE:  Returns the canonical data type for the property.

VARTYPE OpcGetPropertyType(DWORD dwID);

//============================================================================
// FUNCTION: OpcParseItemID
// PURPOSE:  Extracts the root item id and property id from an item id.

bool OpcParseItemID(COpcString& cItemID, DWORD& dwPropertyID);

//============================================================================
// FUNCTION: OpcParseItemID
// PURPOSE:  Constructs a composite item id from a root item id an a property id.

COpcString OpcConstructItemID(LPCWSTR szItemID, DWORD dwPropertyID);

//============================================================================
// MACRO:   Vendor Defined Property Id
// PURPOSE: Defines some vendor specific DA property ids.

#define OPC_PROPERTY_SIMULATION_NAME      5000
#define OPC_PROPERTY_SIMULATION_COUNT     5001
#define OPC_PROPERTY_SIMULATION_TIMESTAMP 5002

#endif // _COpcDaProperty_H_