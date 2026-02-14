//==============================================================================
// TITLE: COpcClassFactory.h
//
// CONTENTS:
// 
// Declares a standard COM class factory class.
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

#ifndef _COpcClassFactory_H
#define _COpcClassFactory_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "OpcCategory.h"
#include "COpcCriticalSection.h"

//==============================================================================
// TYPE:    OpcCreateInstanceProc
// PURPOSE: Declares a function to create instances of COM servers.
// NOTES:

typedef HRESULT (__stdcall PfnOpcCreateInstance)(IUnknown**, const CLSID*);

//==============================================================================
// STRUCT:  TOpcClassTableEntry
// PURPOSE: An element in the module class table.
// NOTES:

struct TOpcClassTableEntry 
{ 
    const CLSID*          pClsid; 
    LPCTSTR               tsClassName; 
	LPCTSTR               tsClassVersion;
	LPCTSTR               tsClassDescription;
    PfnOpcCreateInstance* pfnCreateInstance;
}; 


//==============================================================================
// CLASS:   COpcClassFactory
// PURPOSE: Implements the IClassFactory interface.
// NOTES:

class OPCUTILS_API COpcClassFactory : public IClassFactory
{
    OPC_CLASS_NEW_DELETE();

public:

    //==========================================================================
    // Operators

    // Constructor
    inline COpcClassFactory(const TOpcClassTableEntry* pClassInfo)
    {
        m_ulRefs  = 1;
        m_ulLocks = 0;
        m_pClassInfo = pClassInfo;
    }

    // Destructor
    inline ~COpcClassFactory()
    {
        OPC_ASSERT(m_ulRefs == 0 && m_ulLocks == 0);
    }

    //==========================================================================
    // IUnknown

    // QueryInterface
    STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppInterface) 
    { 
       if (ppInterface == NULL) return E_POINTER;

       if (iid == IID_IClassFactory || iid == IID_IUnknown) 
       {
          AddRef();
          *ppInterface = dynamic_cast<IClassFactory*>(this);
          return S_OK;
       }

       return E_NOINTERFACE;
    } 

    // AddRef
    STDMETHODIMP_(ULONG) AddRef()
    { 
        COpcLock cLock(m_cLock);
        ULONG ulRefs = ++m_ulRefs;       
        return ulRefs; 
    } 

    // Release
    STDMETHODIMP_(ULONG) Release()
    { 
        COpcLock cLock(m_cLock);

        OPC_ASSERT(m_ulRefs > 0);

        ULONG ulRefs = --m_ulRefs;

        if (ulRefs == 0 && m_ulLocks == 0)
        {
            cLock.Unlock();
            delete this;
        }

        return ulRefs; 
    } 
 
    //==========================================================================
    // IClassFactory

    // CreateInstance
    STDMETHODIMP CreateInstance(
        IUnknown* pUnkOuter,
        REFIID    riid,
        void**    ppvObject)
    {
        *ppvObject = NULL;

        // aggregation is not supported.
        if (pUnkOuter != NULL)
        {
            return CLASS_E_NOAGGREGATION;
        }

        OPC_ASSERT(m_pClassInfo != NULL && m_pClassInfo->pfnCreateInstance != NULL);

        // create instance - adds one reference.
        IUnknown* ipUnknown = NULL;

		HRESULT hResult = m_pClassInfo->pfnCreateInstance(&ipUnknown, m_pClassInfo->pClsid);
        
        if (FAILED(hResult))
        {
            return hResult;
        }

        // query desired interface - adds another reference.
        hResult = ipUnknown->QueryInterface(riid, ppvObject);

        if (FAILED(hResult))
        {
            ipUnknown->Release();
            return hResult;
        }

        // release one reference.
        ipUnknown->Release();
        return S_OK;
    }

    // LockServer
    STDMETHODIMP LockServer(BOOL fLock)
    {
        COpcLock cLock(m_cLock);

        if (fLock)
        {
            m_ulLocks++;
            return  S_OK; 
        }

        OPC_ASSERT(m_ulLocks > 0);

        ULONG ulLocks = --m_ulLocks;

        if (ulLocks == 0 && m_ulRefs == 0)
        {
            cLock.Unlock();
            delete this;
        }

        return S_OK;
    }

private:

    COpcCriticalSection        m_cLock;
    ULONG                      m_ulRefs;
    ULONG                      m_ulLocks;
    const TOpcClassTableEntry* m_pClassInfo;
};

//==============================================================================
// FUNCTION: OpcGetClassObject
// PURPOSE:  Gets the class factory for the specified COM server.
// NOTES:

OPCUTILS_API HRESULT OpcGetClassObject(
    REFCLSID                   rclsid, 
    REFIID                     riid, 
    LPVOID*                    ppv, 
    const TOpcClassTableEntry* pClasses);

//==============================================================================
// FUNCTION: OpcRegisterClassObject
// PURPOSE:  Registers COM servers for use as an EXE server.
// NOTES:

OPCUTILS_API HRESULT OpcRegisterClassObjects(
    const TOpcClassTableEntry* pClasses,
    DWORD                      dwContext, 
    DWORD                      dwFlags,
    DWORD*                     pdwRegister);

//==============================================================================
// FUNCTION: OpcRevokeClassObject
// PURPOSE:  Unregisters COM servers for use as an EXE server.
// NOTES:

OPCUTILS_API HRESULT OpcRevokeClassObjects(
    const TOpcClassTableEntry* pClasses,
    DWORD*                     pdwRegister);

//==============================================================================
// FUNCTION: OpcRegisterServer
// PURPOSE:  Registers COM servers for the module.
// NOTES:

OPCUTILS_API HRESULT OpcRegisterServer(
    HINSTANCE                  hModule, 
	LPCTSTR                    tsVendorName,
	LPCTSTR                    tsApplicationName,
	LPCTSTR                    tsApplicationDescription,
	const GUID&                cAppID,
	bool                       bService,
    const TOpcClassTableEntry* pClasses,
    const TClassCategories*    pCategories,
	bool                       bEveryone);

//==============================================================================
// FUNCTION: OpcUnregisterServer
// PURPOSE:  Unregisters COM servers for the module.
// NOTES:

OPCUTILS_API HRESULT OpcUnregisterServer(
    HINSTANCE                  hModule, 
	LPCTSTR                    tsVendorName,
	LPCTSTR                    tsApplicationName,
	const GUID&                cAppID,
    const TOpcClassTableEntry* pClasses,
    const TClassCategories*    pCategories);

//==============================================================================
// MACRO:   OPC_BEGIN_CLASS_MAP
// PURPOSE: Starts the COM class table.
// NOTES:

#define OPC_BEGIN_CLASS_TABLE() \
\
static HINSTANCE g_hModule = NULL; \
static const TOpcClassTableEntry g_pClassTable[] = {

//==============================================================================
// MACRO:   OPC_CLASS_ENTRY
// PURPOSE: Adds a class to the COM class table.
// NOTES:

#define OPC_CLASS_TABLE_ENTRY(xClass, xClassName, xClassVersion, xClassDescription) \
{ \
    &(__uuidof(xClassName)), \
	_T(#xClassName), \
	_T(#xClassVersion), \
	_T(xClassDescription), \
    (PfnOpcCreateInstance*)xClass::CreateInstance \
},

//==============================================================================
// MACRO:   OPC_END_CLASS_MAP
// PURPOSE: Completes the COM class table.
// NOTES:

#define OPC_END_CLASS_TABLE() {&CLSID_NULL, NULL, NULL, NULL, NULL}}; 

#endif // _COpcClassFactory_H