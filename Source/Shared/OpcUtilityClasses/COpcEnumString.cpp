//==============================================================================
// TITLE: COpcEnumString.cpp
//
// CONTENTS:
// 
// A class that implements the IEnumConnectionPoints interface.
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
//

#include "StdAfx.h"
#include "COpcEnumString.h"

//==============================================================================
// COpcEnumString

COpcEnumString::COpcEnumString()
{
    m_uIndex   = 0;
    m_uCount   = 0;
    m_pStrings = NULL;
}

// Constructor
COpcEnumString::COpcEnumString(UINT uCount, LPWSTR*& pStrings)
{
    m_uIndex   = 0;
    m_uCount   = uCount;
    m_pStrings = pStrings;

    // take ownership of memory.
    pStrings = NULL;
}

// Destructor 
COpcEnumString::~COpcEnumString()
{
    for (UINT ii = 0; ii < m_uCount; ii++)
    {
        OpcFree(m_pStrings[ii]);
    }

    OpcFree(m_pStrings);
}

//==============================================================================
// IEnumString
   
// Next
HRESULT COpcEnumString::Next(
    ULONG     celt,
    LPOLESTR* rgelt,
    ULONG*    pceltFetched)
{
    // check for invalid arguments.
    if (rgelt == NULL || pceltFetched == NULL)
    {
        return E_INVALIDARG;
    }

    *pceltFetched = NULL;

    // all strings already returned.
    if (m_uIndex >= m_uCount)
    {
        return S_FALSE;
    }

    // copy strings.
    UINT ii;
    for (ii = m_uIndex; ii < m_uCount && *pceltFetched < celt; ii++)
    {
        rgelt[*pceltFetched] = (LPWSTR)CoTaskMemAlloc((wcslen(m_pStrings[ii])+1)*sizeof(WCHAR));

        if (m_pStrings[ii] == NULL)
        {
            rgelt[*pceltFetched] = NULL;
        }
        else
        {
            wcscpy(rgelt[*pceltFetched], m_pStrings[ii]);
        }

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
HRESULT COpcEnumString::Skip(ULONG celt)
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
HRESULT COpcEnumString::Reset()
{
    m_uIndex = 0;
    return S_OK;
}

// Clone
HRESULT COpcEnumString::Clone(IEnumString** ppEnum)
{
    // check for invalid arguments.
    if (ppEnum == NULL)
    {
        return E_INVALIDARG;
    }

    // allocate enumerator.
    COpcEnumString* pEnum = new COpcEnumString();
 
    // copy strings.
    pEnum->m_pStrings = OpcArrayAlloc(LPWSTR, m_uCount);

    for (UINT ii = 0; ii < m_uCount; ii++)
    {
        pEnum->m_pStrings[ii] = OpcArrayAlloc(WCHAR, wcslen(m_pStrings[ii])+1);
        wcscpy(pEnum->m_pStrings[ii], m_pStrings[ii]);
    }

    // set index.
    pEnum->m_uIndex = m_uIndex;
    pEnum->m_uCount = m_uCount;

    HRESULT hResult = pEnum->QueryInterface(IID_IEnumString, (void**)ppEnum);

    // release local reference.
    pEnum->Release();

    return hResult;
}
