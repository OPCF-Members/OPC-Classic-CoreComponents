//==============================================================================
// TITLE: COpcCPContainer.h
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

#ifndef _COpcCPContainer_H_
#define _COpcCPContainer_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcList.h"
#include "COpcConnectionPoint.h"

//==============================================================================
// CLASS:   COpcConnectionPointList
// PURPOSE: Stores a list of connection points.

typedef COpcList<COpcConnectionPoint*> COpcConnectionPointList;
template class OPCUTILS_API COpcList<COpcConnectionPoint*>;

//==============================================================================
// CLASS:   COpcCPContainer
// PURPOSE: Implements the IConnectionPointContainer interface.
// NOTES:

class OPCUTILS_API COpcCPContainer : public IConnectionPointContainer
{
public:

    //==========================================================================
    // Operators

    // Constructor
    COpcCPContainer();

    // Destructor 
    ~COpcCPContainer();

    //==========================================================================
    // IConnectionPointContainer

    // EnumConnectionPoints
    STDMETHODIMP EnumConnectionPoints(IEnumConnectionPoints** ppEnum);

    // FindConnectionPoint
    STDMETHODIMP FindConnectionPoint(REFIID riid, IConnectionPoint** ppCP);

    //==========================================================================
    // Public Methods

	// OnAdvise
	virtual void OnAdvise(REFIID riid, DWORD dwCookie) {}

	// OnUnadvise
	virtual void OnUnadvise(REFIID riid, DWORD dwCookie) {}

protected:

    //==========================================================================
    // Protected Methods

    // RegisterInterface
    void RegisterInterface(const IID& tInterface);

    // UnregisterInterface
    void UnregisterInterface(const IID& tInterface);

    // GetCallback
    HRESULT GetCallback(const IID& tInterface, IUnknown** ippCallback);

    // IsConnected
    bool IsConnected(const IID& tInterface);

    //==========================================================================
    // Protected Members

    COpcConnectionPointList m_cCPs;
};

#endif // _COpcCPContainer_H_