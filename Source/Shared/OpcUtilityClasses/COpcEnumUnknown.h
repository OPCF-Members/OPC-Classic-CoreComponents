/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcEnumUnknowns_H_
#define _COpcEnumUnknowns_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcComObject.h"
#include "COpcList.h"
#include "COpcString.h"

//==============================================================================
// CLASS:   COpcEnumUnknown
// PURPOSE: A class to implement the IEnumUnknown interface.
// NOTES:

class OPCUTILS_API COpcEnumUnknown 
:
    public COpcComObject,
    public IEnumUnknown
{     
    OPC_BEGIN_INTERFACE_TABLE(COpcEnumUnknown)
        OPC_INTERFACE_ENTRY(IEnumUnknown)
    OPC_END_INTERFACE_TABLE()

    OPC_CLASS_NEW_DELETE()

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcEnumUnknown();

    // Constructor
    COpcEnumUnknown(UINT uCount, IUnknown**& pUnknowns);

    // Destructor 
    ~COpcEnumUnknown();

    //==========================================================================
    // IEnumConnectionPoints
       
    // Next
    STDMETHODIMP Next(
        ULONG      celt,          
        IUnknown** rgelt,   
        ULONG*     pceltFetched
    );

    // Skip
    STDMETHODIMP Skip(ULONG celt);

    // Reset
    STDMETHODIMP Reset();

    // Clone
    STDMETHODIMP Clone(IEnumUnknown** ppEnum);

private:

    //==========================================================================
    // Private Members

    UINT       m_uIndex;
    UINT       m_uCount;
    IUnknown** m_pUnknowns;
};

#endif // _COpcEnumUnknowns_H_
