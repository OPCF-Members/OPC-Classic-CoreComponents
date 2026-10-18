/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcEnumCPs_H_
#define _COpcEnumCPs_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcComObject.h"
#include "COpcCPContainer.h"

//==============================================================================
// CLASS:   COpcEnumCPs
// PURPOSE: Implements the IEnumConnectionPoints interface.
// NOTES:

class OPCUTILS_API COpcEnumCPs 
:
    public COpcComObject,
    public IEnumConnectionPoints
{     
    OPC_BEGIN_INTERFACE_TABLE(COpcEnumCPs)
        OPC_INTERFACE_ENTRY(IEnumConnectionPoints)
    OPC_END_INTERFACE_TABLE()

    OPC_CLASS_NEW_DELETE()

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcEnumCPs();
    
    // Constructor
    COpcEnumCPs(const COpcList<COpcConnectionPoint*>& cCPs);

    // Destructor 
    ~COpcEnumCPs();

    //==========================================================================
    // IEnumConnectionPoints

    // Next
    STDMETHODIMP Next(
        ULONG              cConnections,
        LPCONNECTIONPOINT* ppCP,
        ULONG*             pcFetched
    );

    // Skip
    STDMETHODIMP Skip(ULONG cConnections);

    // Reset
    STDMETHODIMP Reset();

    // Clone
    STDMETHODIMP Clone(IEnumConnectionPoints** ppEnum);

private:

    //==========================================================================
    // Private Members

    OPC_POS                 m_pos;
    COpcConnectionPointList m_cCPs;
};

#endif // _COpcEnumCPs_H_
