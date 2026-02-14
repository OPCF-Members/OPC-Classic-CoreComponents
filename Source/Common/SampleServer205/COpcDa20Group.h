//============================================================================
// TITLE: COpcDa20Group.h
//
// CONTENTS:
// 
// A group object for an OPC Data Access server.
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
// 2003/05/13 RSA   First release.
//

#ifndef _COpcDa20Group_H_
#define _COpcDa20Group_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaGroup.h"

//============================================================================
// CLASS:   COpcDa20Group
// PURPOSE: A class that implements the IOPCServer interface.
// NOTES:

class COpcDa20Group : public COpcDaGroup
{
    OPC_CLASS_NEW_DELETE()

    OPC_BEGIN_INTERFACE_TABLE(COpcDa20Group)
        OPC_INTERFACE_ENTRY(IConnectionPointContainer)
        OPC_INTERFACE_ENTRY(IOPCItemMgt)
        OPC_INTERFACE_ENTRY(IOPCSyncIO)
        OPC_INTERFACE_ENTRY(IOPCAsyncIO2)
        OPC_INTERFACE_ENTRY(IOPCGroupStateMgt)
    OPC_END_INTERFACE_TABLE()

public:

    //=========================================================================
    // Operators

    // Constructor
	COpcDa20Group() {}
	COpcDa20Group(COpcDaServer& cServer, const COpcString& cName) : COpcDaGroup(cServer, cName) {}

    // Destructor 
    ~COpcDa20Group() { Clear(); }
};

#endif // _COpcDa20Group_H_