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
