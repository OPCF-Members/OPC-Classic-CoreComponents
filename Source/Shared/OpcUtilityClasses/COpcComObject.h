//==============================================================================
// TITLE: COpcComObject.h
//
// CONTENTS:
// 
// A base class for COM servers
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

#ifndef _COpcComObject_H
#define _COpcComObject_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"

//==============================================================================
// TITLE:   COpcComObject.h
// PURPOSE: Defines the base class for COM servers
// NOTES:

class OPCUTILS_API COpcComObject
{
    OPC_CLASS_NEW_DELETE_ARRAY();

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcComObject()
    {
        // set the reference count to one - calling function must release.
        m_ulRefs = 1;
    }

    // Destructor
    virtual ~COpcComObject()
    {
        OPC_ASSERT(m_ulRefs == 0);
    }

    //==========================================================================
    // Public Methods

    // InternalAddRef
    // 
    // Description
    //
    // Adds a reference to the COM server.
    //
    // Return Codes
    //
    // The current number of references.

    ULONG InternalAddRef() 
    { 
        return InterlockedIncrement((LONG*)&m_ulRefs); 
    } 

    // InternalRelease
    // 
    // Description
    //
    // Removes a reference to the COM server. If the reference reaches zero
    // it calls FinalRelease() and deletes the instance.
    //
    // Return Codes
    //
    // The current number of references.

    ULONG InternalRelease() 
    { 
        ULONG ulRefs = InterlockedDecrement((LONG*)&m_ulRefs); 

        if (ulRefs == 0) 
        { 
            if (FinalRelease())
			{
				delete this;
			}

			return 0; 
        } 

        return ulRefs; 
    } 

    // InternalQueryInterface
    // 
    // Description
    //
    // An pure virtual method that the COM server's interface map implements.
    // The inteface map macro's add a stub QueryInterface implmentation
    // that call this function. It calls AddRef() so the client must call
    // Release() on the returned interface.
    //
    // Parameters;
    //
    // iid The desired interface IID.
    // ppInterface The returned interface.
    //
    // Return Codes
    //
    // S_OK if the interface is supported
    // E_NOINTERFACE if not.

    virtual HRESULT InternalQueryInterface(REFIID iid, LPVOID* ppInterface) = 0;

    // FinalConstruct
    // 
    // Description
    //
    // A function called by the class factory after creating the object.
    // The COM server does any server specific initialization.
    //
    // Return Codes
    //
    // S_OK if intialization succeeded.

    virtual HRESULT FinalConstruct() 
    { 
        return S_OK;
    } 

    // FinalRelease
    // 
    // Description
    //
    // A function called by release after the reference count drops to zero
    // before deleting the object. The COM server does any server specific 
    // uninitialization.
    //

    virtual bool FinalRelease() 
    { 
		// returning false would stop the caller from explicitly deleting the object.
		return true;
    } 

private: 

    //==========================================================================
    // Private Members

    ULONG m_ulRefs;
};

//==============================================================================
// MACRO:   OPC_BEGIN_INTERFACE_TABLE
// PURPOSE: Starts the COM server's interface table.
// NOTES:

#define OPC_BEGIN_INTERFACE_TABLE(xClass) \
private: \
\
const CLSID* m_pClsid; \
\
protected: \
\
REFCLSID GetCLSID() { return *m_pClsid; } \
\
public: \
\
static HRESULT __stdcall CreateInstance(IUnknown** ippUnknown, const CLSID* pClsid) \
{ \
    if (ippUnknown == NULL) return E_POINTER; \
    *ippUnknown = NULL; \
\
    xClass* pObject = new xClass(); \
\
	pObject->m_pClsid = pClsid; \
\
    HRESULT hResult = pObject->FinalConstruct(); \
\
    if (FAILED(hResult)) \
    { \
       pObject->Release(); \
       return hResult; \
    } \
\
    hResult = pObject->QueryInterface(IID_IUnknown, (void**)ippUnknown); \
    pObject->Release(); \
    return hResult; \
} \
\
virtual HRESULT InternalQueryInterface(REFIID iid, LPVOID* ppInterface) \
{ \
    if (ppInterface == NULL) return E_POINTER; \
    *ppInterface = NULL; 

//==============================================================================
// MACRO:   OPC_INTERFACE_ENTRY
// PURPOSE: Adds an interface to the COM server's interface table.
// NOTES:

#define OPC_INTERFACE_ENTRY(xInterface) \
\
if (iid == __uuidof(xInterface) || iid == IID_IUnknown) \
{ \
    *ppInterface = (dynamic_cast<xInterface*>(this)); \
    AddRef(); \
    return S_OK; \
} 

//==============================================================================
// MACRO:   OPC_AGGREGATE_OBJECT
// PURPOSE: Adds an interface to the COM server's interface table.
// NOTES:

#define OPC_AGGREGATE_OBJECT(xObject) \
\
if (xObject != NULL) \
{ \
    return xObject->QueryInterface(iid, ppInterface); \
}

//==============================================================================
// MACRO:   OPC_END_INTERFACE_TABLE
// PURPOSE: Completes the COM server's interface table.
// NOTES:

#define OPC_END_INTERFACE_TABLE() \
return E_NOINTERFACE; } \
\
STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppInterface) {return InternalQueryInterface(iid, ppInterface);} \
STDMETHODIMP_(ULONG) AddRef() {return InternalAddRef();} \
STDMETHODIMP_(ULONG) Release() {return InternalRelease();} 

#endif // _COpcComObject_H