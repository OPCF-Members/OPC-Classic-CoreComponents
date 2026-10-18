/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcTestGroup_H_
#define _COpcTestGroup_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaGroup.h"

//============================================================================
// CLASS:   COpcTestGroup
// PURPOSE: A class that implements the IOPCServer interface.
// NOTES:

class COpcTestGroup : public COpcDaGroup
{
    OPC_CLASS_NEW_DELETE()

    OPC_BEGIN_INTERFACE_TABLE(COpcTestGroup)
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
	COpcTestGroup() {}
	COpcTestGroup(COpcDaServer& cServer, const COpcString& cName) : COpcDaGroup(cServer, cName) {}

    // Destructor
    ~COpcTestGroup() { Clear(); }
};

#endif // _COpcTestGroup_H_
