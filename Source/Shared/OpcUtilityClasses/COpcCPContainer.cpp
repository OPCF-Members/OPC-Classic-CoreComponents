//==============================================================================
// TITLE: COpcCPContainer.cpp
//
// CONTENTS:
// 
//  A class that implements the IConnectionPointContainer interface.
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
#include "COpcCPContainer.h"
#include "COpcEnumCPs.h"

//==============================================================================
// COpcCPContainer

// Constructor
COpcCPContainer::COpcCPContainer()
{
}

// Destructor 
COpcCPContainer::~COpcCPContainer()
{
    // release the connection points.
    OPC_POS pos = m_cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        m_cCPs.GetNext(pos)->Release();
    }
}

// RegisterInterface
void COpcCPContainer::RegisterInterface(const IID& tInterface)
{
    // constructor adds one reference.
    COpcConnectionPoint* pCP = new COpcConnectionPoint(tInterface, this);
    m_cCPs.AddTail(pCP);
}

// UnregisterInterface
void COpcCPContainer::UnregisterInterface(const IID& tInterface)
{
    OPC_POS pos = m_cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        COpcConnectionPoint* pCP = m_cCPs[pos];

        if (pCP->GetInterface() == tInterface)
        {
            m_cCPs.RemoveAt(pos);
            pCP->Delete();
            break;
        }

        m_cCPs.GetNext(pos);
    }
}

// GetCallback
HRESULT COpcCPContainer::GetCallback(const IID& tInterface, IUnknown** ippCallback)
{
    COpcConnectionPoint* pCP = NULL;

    OPC_POS pos = m_cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        pCP = m_cCPs.GetNext(pos);

        if (pCP->GetInterface() == tInterface)
        {
            IUnknown* ipUnknown = pCP->GetCallback();
            
            if (ipUnknown != NULL)
            {
                return ipUnknown->QueryInterface(tInterface, (void**)ippCallback);
            }
        }
    }

    return E_FAIL;
}

// IsConnected
bool COpcCPContainer::IsConnected(const IID& tInterface)
{
    COpcConnectionPoint* pCP = NULL;

    OPC_POS pos = m_cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        pCP = m_cCPs.GetNext(pos);

        if (pCP->GetInterface() == tInterface)
        {
            return pCP->IsConnected();
        }
    }

    return false;
}

//==============================================================================
// IConnectionPointContainer

// EnumConnectionPoints
HRESULT COpcCPContainer::EnumConnectionPoints(IEnumConnectionPoints** ppEnum)
{
    // invalid arguments.
    if (ppEnum == NULL)
    {
        return E_POINTER;
    }

    // create enumeration object.
    COpcEnumCPs* pEnumCPs = new COpcEnumCPs(m_cCPs);

    if (pEnumCPs == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // query for enumeration interface.
    HRESULT hResult = pEnumCPs->QueryInterface(IID_IEnumConnectionPoints, (void**)ppEnum);

    // release local reference.
    pEnumCPs->Release();

    return hResult;
}

// FindConnectionPoint
HRESULT COpcCPContainer::FindConnectionPoint(REFIID riid, IConnectionPoint** ppCP)
{
    // invalid arguments.
    if (ppCP == NULL)
    {
        return E_POINTER;
    }

    // search for connection point.
    OPC_POS pos = m_cCPs.GetHeadPosition();

    while (pos != NULL)
    {
        COpcConnectionPoint* pCP = m_cCPs.GetNext(pos);

        if (pCP->GetInterface() == riid)
        {
            return pCP->QueryInterface(IID_IConnectionPoint, (void**)ppCP);
        }
    }

    // connection point not found.
    return CONNECT_E_NOCONNECTION;
}
