/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcDaEnumItem_H_
#define _COpcDaEnumItem_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//============================================================================
// CLASS:   COpcDaEnumItem
// PURPOSE: A class to implement the IEnumString interface.
// NOTES:

class COpcDaEnumItem 
:
    public COpcComObject,
    public IEnumOPCItemAttributes
{     
    OPC_BEGIN_INTERFACE_TABLE(COpcDaEnumItem)
        OPC_INTERFACE_ENTRY(IEnumOPCItemAttributes)
    OPC_END_INTERFACE_TABLE()

    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Operators

    // Constructor
    COpcDaEnumItem();

    // Constructor
    COpcDaEnumItem(UINT uCount, OPCITEMATTRIBUTES* pItems);

    // Destructor 
    ~COpcDaEnumItem();

    //========================================================================
    // IEnumOPCItemAttributes

    // Next
	STDMETHODIMP Next( 
		ULONG               celt,
		OPCITEMATTRIBUTES** ppItemArray,
		ULONG*              pceltFetched 
	);

    // Skip
	STDMETHODIMP Skip(ULONG celt);

    // Reset
	STDMETHODIMP Reset();

    // Clone
	STDMETHODIMP Clone(IEnumOPCItemAttributes** ppEnumItemAttributes);

private:

	//=========================================================================
    // Private Methods

	// Init
	void Init(OPCITEMATTRIBUTES& cAttributes);

	// Clear
	void Clear(OPCITEMATTRIBUTES& cAttributes);

	// Copy
	void Copy(OPCITEMATTRIBUTES& cDst, OPCITEMATTRIBUTES& cSrc);

    //=========================================================================
    // Private Members

    UINT			   m_uIndex;
    UINT			   m_uCount;
    OPCITEMATTRIBUTES* m_pItems;
};

#endif // _COpcDaEnumItem_H_
