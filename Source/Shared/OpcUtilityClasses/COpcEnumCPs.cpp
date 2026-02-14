//==============================================================================
// TITLE: COpcEnumCPs.cpp
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
#include "COpcEnumCPs.h"

// Constructor
COpcEnumCPs::COpcEnumCPs()
{
    Reset();
}

// Constructor
COpcEnumCPs::COpcEnumCPs(const COpcConnectionPointList& cCPs)
{
    // copy connection points.
    OPC_POS pos = cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        COpcConnectionPoint* pCP = cCPs.GetNext(pos);

        m_cCPs.AddTail(pCP);
        pCP->AddRef();
    }      

    // set pointer to start of list.
    Reset();
}

// Destructor 
COpcEnumCPs::~COpcEnumCPs()
{
    // release connection points.
    OPC_POS pos = m_cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        m_cCPs.GetNext(pos)->Release();
    }
}

//==============================================================================
// IEnumConnectionPoints

// Next
HRESULT COpcEnumCPs::Next(
    ULONG              cConnections,
    LPCONNECTIONPOINT* ppCP,
    ULONG*             pcFetched
)
{
    // invalid arguments - return error.
    if (pcFetched == NULL)
    {
        return E_INVALIDARG;
    }
        
    *pcFetched = 0;

    // trivial case - return nothing.
    if (cConnections == 0)
    {
        return S_OK;
    }
    
    // read connection points.
    ULONG ii;
    for (ii = 0; ii < cConnections; ii++)
    {
        // end of list reached before count reached.
        if (m_pos == NULL)
        {
            *pcFetched = ii;
            return S_FALSE;
        }

        ppCP[ii] = m_cCPs.GetNext(m_pos);

        // client must release the reference.
        ppCP[ii]->AddRef();
    }

    *pcFetched = ii;
    return S_OK;
}

// Skip
HRESULT COpcEnumCPs::Skip(ULONG cConnections)
{
    // skip connection points.
    OPC_POS pos = m_cCPs.GetHeadPosition();

    for (ULONG ii = 0; ii < cConnections; ii++)
    {
        // end of list reached before count reached.
        if (m_pos == NULL)
        {
            return S_FALSE;
        }

        m_cCPs.GetNext(m_pos);
    } 

    return S_OK;
}

// Reset
HRESULT COpcEnumCPs::Reset()
{
    m_pos = m_cCPs.GetHeadPosition();
    return S_OK;
}

// Clone
HRESULT COpcEnumCPs::Clone(IEnumConnectionPoints** ppEnum)
{
    // create a new enumeration object.
    COpcEnumCPs* ipEnum = new COpcEnumCPs();

    // copy connection points.
    OPC_POS pos = m_cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        COpcConnectionPoint* pCP = m_cCPs[pos];

        ipEnum->m_cCPs.AddTail(pCP);
    
        // clone must release the reference.
        pCP->AddRef();

        // save the current location.
        if (pos == m_pos)
        {
            ipEnum->m_pos = ipEnum->m_cCPs.GetTailPosition();   
        }

        m_cCPs.GetNext(pos);
    }      

    // query interface.
    HRESULT hResult = ipEnum->QueryInterface(IID_IEnumConnectionPoints, (void**)ppEnum);

    // release local reference.
    ipEnum->Release();

    return hResult;
}
