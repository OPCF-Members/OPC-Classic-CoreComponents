/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcTestServer_H_
#define _COpcTestServer_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaServer.h"

#include "OpcTestServer.h"
#include "COpcTestGroup.h"

//============================================================================
// CLASS:   COpcTestServer
// PURPOSE: A class that implements the IOPCServer interface.

class COpcTestServer :
    public COpcComObject,
    public COpcDaServer
{
    OPC_CLASS_NEW_DELETE()

    OPC_BEGIN_INTERFACE_TABLE(COpcTestServer)
        OPC_INTERFACE_ENTRY(IOPCCommon)
        OPC_INTERFACE_ENTRY(IConnectionPointContainer)
        OPC_INTERFACE_ENTRY(IOPCServer)
        OPC_INTERFACE_ENTRY(IOPCBrowseServerAddressSpace)
        OPC_INTERFACE_ENTRY(IOPCItemProperties)
    OPC_END_INTERFACE_TABLE()

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcTestServer() {}

    // Destructor
    ~COpcTestServer() {}

    //=========================================================================
    // Public Methods

    // FinalConstruct
    virtual HRESULT FinalConstruct();

    // FinalRelease
    virtual bool FinalRelease();

	// CreateGroup
	virtual COpcDaGroup* CreateGroup(COpcDaServer& cServer, const COpcString& cName) { return new COpcTestGroup(cServer, cName); }
};

#endif // _COpcTestServer_H_
