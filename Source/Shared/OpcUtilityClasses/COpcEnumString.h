/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcEnumStrings_H_
#define _COpcEnumStrings_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcComObject.h"
#include "COpcList.h"
#include "COpcString.h"

//==============================================================================
// CLASS:   COpcEnumString
// PURPOSE: A class to implement the IEnumString interface.
// NOTES:

class OPCUTILS_API COpcEnumString 
:
    public COpcComObject,
    public IEnumString
{     
    OPC_BEGIN_INTERFACE_TABLE(COpcEnumString)
        OPC_INTERFACE_ENTRY(IEnumString)
    OPC_END_INTERFACE_TABLE()

    OPC_CLASS_NEW_DELETE()

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcEnumString();

    // Constructor
    COpcEnumString(UINT uCount, LPWSTR*& pStrings);

    // Destructor 
    ~COpcEnumString();

    //==========================================================================
    // IEnumConnectionPoints
       
    // Next
    STDMETHODIMP Next(
        ULONG     celt,
        LPOLESTR* rgelt,
        ULONG*    pceltFetched);

    // Skip
    STDMETHODIMP Skip(ULONG celt);

    // Reset
    STDMETHODIMP Reset();

    // Clone
    STDMETHODIMP Clone(IEnumString** ppEnum);

private:

    //==========================================================================
    // Private Members

    UINT    m_uIndex;
    UINT    m_uCount;
    LPWSTR* m_pStrings;
};

#endif // _COpcEnumStrings_H_
