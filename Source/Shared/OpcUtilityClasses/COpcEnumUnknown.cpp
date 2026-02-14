//==============================================================================
// TITLE: COpcEnumUnknown.cpp
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
#include "COpcEnumUnknown.h"

//==============================================================================
// COpcEnumUnknown

COpcEnumUnknown::COpcEnumUnknown()
{
    m_uIndex    = 0;
    m_uCount    = 0;
    m_pUnknowns = NULL;
}

// Constructor
COpcEnumUnknown::COpcEnumUnknown(UINT uCount, IUnknown**& pUnknowns)
{
    m_uIndex   = 0;
    m_uCount   = uCount;
    m_pUnknowns = pUnknowns;

    // take ownership of memory.
    pUnknowns = NULL;
}

// Destructor 
COpcEnumUnknown::~COpcEnumUnknown()
{
    for (UINT ii = 0; ii < m_uCount; ii++)
    {
        if (m_pUnknowns[ii] != NULL) m_pUnknowns[ii]->Release();
    }

    OpcFree(m_pUnknowns);
}

//==============================================================================
// IEnumUnknown
   
// Next
HRESULT COpcEnumUnknown::Next(
    ULONG      celt,          
    IUnknown** rgelt,   
    ULONG*     pceltFetched
)
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
        rgelt[*pceltFetched] = m_pUnknowns[ii];
        if (m_pUnknowns[ii] != NULL) m_pUnknowns[ii]->AddRef();
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
HRESULT COpcEnumUnknown::Skip(ULONG celt)
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
HRESULT COpcEnumUnknown::Reset()
{
    m_uIndex = 0;
    return S_OK;
}

// Clone
HRESULT COpcEnumUnknown::Clone(IEnumUnknown** ppEnum)
{
    // check for invalid arguments.
    if (ppEnum == NULL)
    {
        return E_INVALIDARG;
    }

    // allocate enumerator.
    COpcEnumUnknown* pEnum = new COpcEnumUnknown();
 
    // copy strings.
    pEnum->m_pUnknowns = OpcArrayAlloc(IUnknown*, m_uCount);

    for (UINT ii = 0; ii < m_uCount; ii++)
    {
        pEnum->m_pUnknowns[ii] = m_pUnknowns[ii];
        if (m_pUnknowns[ii] != NULL) m_pUnknowns[ii]->AddRef();
    }

    // set index.
    pEnum->m_uIndex = m_uIndex;
    pEnum->m_uCount = m_uCount;

    HRESULT hResult = pEnum->QueryInterface(IID_IEnumUnknown, (void**)ppEnum);

    // release local reference.
    pEnum->Release();

    return hResult;
}
