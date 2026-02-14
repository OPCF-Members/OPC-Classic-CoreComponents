//==============================================================================
// TITLE: COpcConnectionPoint.h
//
// CONTENTS:
// 
//  A class that implements the IConnectionPoint interface.
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

#ifndef _COpcConnectionPoint_H_
#define _COpcConnectionPoint_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ocidl.h"

#include "OpcDefs.h"
#include "COpcComObject.h"
#include "COpcCriticalSection.h"

class COpcCPContainer;

//==============================================================================
// CLASS:   COpcConnectionPoint
// PURPOSE: Implements the IConnectionPoint interface.
// NOTES:

class OPCUTILS_API COpcConnectionPoint
: 
    public COpcComObject,
    public COpcSynchObject,
    public IConnectionPoint
{
    OPC_BEGIN_INTERFACE_TABLE(COpcConnectionPoint)
        OPC_INTERFACE_ENTRY(IConnectionPoint)
    OPC_END_INTERFACE_TABLE()

    OPC_CLASS_NEW_DELETE()

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcConnectionPoint();

    // Constructor
    COpcConnectionPoint(const IID& tIid, COpcCPContainer* pContainer);

    // Destructor 
    ~COpcConnectionPoint();

    //==========================================================================
    // IConnectionPoint

    // GetConnectionInterface
    STDMETHODIMP GetConnectionInterface(IID* pIID);

    // GetConnectionPointContainer
    STDMETHODIMP GetConnectionPointContainer(IConnectionPointContainer** ppCPC);

    // Advise
    STDMETHODIMP Advise(IUnknown* pUnkSink, DWORD* pdwCookie);

    // Unadvise
    STDMETHODIMP Unadvise(DWORD dwCookie);

    // EnumConnections
    STDMETHODIMP EnumConnections(IEnumConnections** ppEnum);

    //==========================================================================
    // Public Methods

    // GetCallback
    IUnknown* GetCallback() { return m_ipCallback; }

    // GetInterface
    const IID& GetInterface() { return m_tInterface; }

    // Delete
    bool Delete();

    // IsConnected
    bool IsConnected() { return (m_dwCookie != NULL); }
    
private:

    //==========================================================================
    // Private Members

    IID              m_tInterface;
    COpcCPContainer* m_pContainer;
    IUnknown*        m_ipCallback;
    DWORD            m_dwCookie;
    bool             m_bFetched;
};

//==============================================================================
// FUNCTION: OpcConnect
// PURPOSE:  Establishes a connection to the server.

OPCUTILS_API HRESULT OpcConnect(
    IUnknown* ipSource, 
    IUnknown* ipSink, 
    REFIID    riid, 
    DWORD*    pdwConnection);

//==============================================================================
// FUNCTION: OpcDisconnect
// PURPOSE:  Closes a connection to the server.

OPCUTILS_API HRESULT OpcDisconnect(
    IUnknown* ipSource, 
    REFIID    riid, 
    DWORD     dwConnection);

#endif // _COpcConnectionPoint_H_