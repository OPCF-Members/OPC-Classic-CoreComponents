//==============================================================================
// TITLE: COpcEnumUnknown.h
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