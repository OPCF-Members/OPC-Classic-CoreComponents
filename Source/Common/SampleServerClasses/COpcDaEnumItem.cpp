//============================================================================
// TITLE: COpcDaEnumItem.cpp
//
// CONTENTS:
// 
// An implementation of the IEnumOPCItemAttributes interface.
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

#include "StdAfx.h"
#include "COpcDaEnumItem.h"

//============================================================================
// COpcDaEnumItem

COpcDaEnumItem::COpcDaEnumItem()
{
    m_uIndex = 0;
    m_uCount = 0;
    m_pItems = NULL;
	
}

// Constructor
COpcDaEnumItem::COpcDaEnumItem(UINT uCount, OPCITEMATTRIBUTES* pItems)
{
    m_uIndex = 0;
    m_uCount = uCount;
    m_pItems = pItems;
}

// Destructor 
COpcDaEnumItem::~COpcDaEnumItem()
{
    for (UINT ii = 0; ii < m_uCount; ii++)
    {
        Clear(m_pItems[ii]);
    }

    OpcFree(m_pItems);
}

// Init
void COpcDaEnumItem::Init(OPCITEMATTRIBUTES& cAttributes)
{
	memset(&cAttributes, 0, sizeof(OPCITEMATTRIBUTES));
}

// Clear
void COpcDaEnumItem::Clear(OPCITEMATTRIBUTES& cAttributes)
{
	OpcFree(cAttributes.szAccessPath);
	OpcFree(cAttributes.szItemID);
	OpcFree(cAttributes.pBlob);
	OpcVariantClear(&cAttributes.vEUInfo);
}

// Copy
void COpcDaEnumItem::Copy(OPCITEMATTRIBUTES& cDst, OPCITEMATTRIBUTES& cSrc)
{
	cDst.szAccessPath   = OpcStrDup(cSrc.szAccessPath);
	cDst.szItemID       = OpcStrDup(cSrc.szItemID);
	cDst.bActive        = cSrc.bActive;
	cDst.hClient        = cSrc.hClient;
	cDst.hServer        = cSrc.hServer;
	cDst.dwAccessRights = cSrc.dwAccessRights;
	cDst.dwBlobSize     = cSrc.dwBlobSize;
  
	if (cSrc.dwBlobSize > 0)
	{
		cDst.pBlob = (BYTE*)OpcAlloc(cSrc.dwBlobSize);
		memcpy(cDst.pBlob, cSrc.pBlob, cSrc.dwBlobSize);
	}

	
	cDst.vtRequestedDataType = cSrc.vtRequestedDataType;
	cDst.vtCanonicalDataType = cSrc.vtCanonicalDataType;
	cDst.dwEUType            = cSrc.dwEUType;

	OpcVariantCopy(&cDst.vEUInfo, &cSrc.vEUInfo);
}

//============================================================================
// IEnumOPCItemAttributes
   
// Next
HRESULT COpcDaEnumItem::Next(
	ULONG               celt,
	OPCITEMATTRIBUTES** ppItemArray,
	ULONG*              pceltFetched
)
{
    // check for invalid arguments.
    if (ppItemArray == NULL || pceltFetched == NULL)
    {
        return E_INVALIDARG;
    }

    *pceltFetched = 0;

    // all items already returned.
    if (m_uIndex >= m_uCount)
    {
        return S_FALSE;
    }

    // copy items.
    *ppItemArray = (OPCITEMATTRIBUTES*)OpcArrayAlloc(OPCITEMATTRIBUTES, celt);
    memset(*ppItemArray, 0, celt*sizeof(OPCITEMATTRIBUTES));

    UINT ii;
    for (ii = m_uIndex; ii < m_uCount && *pceltFetched < celt; ii++)
    {
        Copy((*ppItemArray)[*pceltFetched], m_pItems[ii]);
        (*pceltFetched)++;
    }

    // no enough strings left.
    if (*pceltFetched < celt)
    {
        m_uIndex = m_uCount;
        return S_FALSE;
    }

    m_uIndex = ii;
    return S_OK;
}

// Skip
HRESULT COpcDaEnumItem::Skip(ULONG celt)
{
    if (m_uIndex + celt > m_uCount)
    {
        m_uIndex = m_uCount;
        return S_FALSE;
    }

    m_uIndex += celt;
    return S_OK;
}

// Reset
HRESULT COpcDaEnumItem::Reset()
{
    m_uIndex = 0;
    return S_OK;
}

// Clone
HRESULT COpcDaEnumItem::Clone(IEnumOPCItemAttributes** ppEnum)
{
    // check for invalid arguments.
    if (ppEnum == NULL)
    {
        return E_INVALIDARG;
    }

    // allocate enumerator.
    COpcDaEnumItem* pEnum = new COpcDaEnumItem();

	if (m_uCount > 0)
	{
		// copy items.
		OPCITEMATTRIBUTES* pItems = OpcArrayAlloc(OPCITEMATTRIBUTES, m_uCount);

		for (UINT ii = 0; ii < m_uCount; ii++)
		{
			Copy(pItems[ii], m_pItems[ii]);
		}

		// set new enumerator state.
		pEnum->m_pItems = pItems;
		pEnum->m_uCount = m_uCount;
		pEnum->m_uIndex = m_uIndex;
	}

	// query for interface.
    HRESULT hResult = pEnum->QueryInterface(IID_IEnumOPCItemAttributes, (void**)ppEnum);

    // release local reference.
    pEnum->Release();

    return hResult;
}
