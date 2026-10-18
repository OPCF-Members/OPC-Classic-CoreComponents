/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _CCategoryManager_H_
#define _CCategoryManager_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcComObject.h"
#include "CategoryManager.h"
#include "opccomn.h"

//============================================================================
// CLASS: CCategoryManager

class CCategoryManager :
    public COpcComObject,
	public IOPCServerList
{
    OPC_CLASS_NEW_DELETE()

    OPC_BEGIN_INTERFACE_TABLE(CCategoryManager)
        OPC_INTERFACE_ENTRY(IOPCServerList)
    OPC_END_INTERFACE_TABLE()

public:

    //==========================================================================
    // Operators

    // Constructor
    CCategoryManager() {}

    // Destructor 
    ~CCategoryManager() {}

    //=========================================================================
    // Public Methods

    // FinalConstruct
    virtual HRESULT FinalConstruct();

    // FinalRelease
    virtual bool FinalRelease();

    //=========================================================================
    // IOPCServerList Methods

    virtual HRESULT STDMETHODCALLTYPE EnumClassesOfCategories( 
        ULONG cImplemented,
        CATID rgcatidImpl[],
        ULONG cRequired,
        CATID rgcatidReq[],
        IEnumGUID **ppenumClsid);
    
    virtual HRESULT STDMETHODCALLTYPE GetClassDetails( 
        REFCLSID clsid,
        LPOLESTR* ppszProgID,
        LPOLESTR* ppszUserType);
    
    virtual HRESULT STDMETHODCALLTYPE CLSIDFromProgID( 
        LPCOLESTR szProgId,
        LPCLSID clsid);
};

#endif // _CCategoryManager_H_
