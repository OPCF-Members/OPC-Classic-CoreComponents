//============================================================================
// TITLE: COpcDa20Server.h
//
// CONTENTS:
// 
// A concrete instance of a OPC Data Server object.
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
// 2002/11/16 RSA   First release.
//

#ifndef _COpcDa20Server_H_
#define _COpcDa20Server_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaServer.h"

#include "OpcDa20Server.h"
#include "COpcDa20Group.h"

//============================================================================
// CLASS:   COpcDa20Server
// PURPOSE: A class that implements the IOPCServer interface.

class COpcDa20Server :
    public COpcComObject,
    public COpcDaServer
{
    OPC_CLASS_NEW_DELETE()

    OPC_BEGIN_INTERFACE_TABLE(COpcDa20Server)
        OPC_INTERFACE_ENTRY(IOPCCommon)
        OPC_INTERFACE_ENTRY(IConnectionPointContainer)
        OPC_INTERFACE_ENTRY(IOPCServer)
        OPC_INTERFACE_ENTRY(IOPCBrowseServerAddressSpace)
        OPC_INTERFACE_ENTRY(IOPCItemProperties)
        OPC_INTERFACE_ENTRY(IOPCSecurityNT)
        OPC_INTERFACE_ENTRY(IOPCSecurityPrivate)
    OPC_END_INTERFACE_TABLE()

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcDa20Server() {}

    // Destructor 
    ~COpcDa20Server() {}

    //=========================================================================
    // Public Methods

    // FinalConstruct
    virtual HRESULT FinalConstruct();

    // FinalRelease
    virtual bool FinalRelease();	
	
	// CreateGroup
	virtual COpcDaGroup* CreateGroup(COpcDaServer& cServer, const COpcString& cName) { return new COpcDa20Group(cServer, cName); }
};

#endif // _COpcDa20Server_H_