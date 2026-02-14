//============================================================================
// TITLE: COpcDaEnumGroup.h
//
// CONTENTS:
// 
// An implementation of the IEnumConnectionPoints interface.
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

#ifndef _COpcDaEnumGroup_H_
#define _COpcDaEnumGroup_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//============================================================================
// CLASS:   COpcDaEnumGroup
// PURPOSE: A class to implement the IEnumString interface.
// NOTES:

class COpcDaEnumGroup 
:
    public COpcComObject,
    public IEnumOPCItemAttributes
{     
    OPC_BEGIN_INTERFACE_TABLE(COpcDaEnumGroup)
        OPC_INTERFACE_ENTRY(IEnumOPCItemAttributes)
    OPC_END_INTERFACE_TABLE()

    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Operators

    // Constructor
    COpcDaEnumGroup();

    // Constructor
    COpcDaEnumGroup(UINT uCount, OPCITEMATTRIBUTES* pItems);

    // Destructor 
    ~COpcDaEnumGroup();

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
	STDMETHODIMP Clone(IEnumOPCItemAttributes** ppEnumGroupAttributes);

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

#endif // _COpcDaEnumGroup_H_