/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

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
