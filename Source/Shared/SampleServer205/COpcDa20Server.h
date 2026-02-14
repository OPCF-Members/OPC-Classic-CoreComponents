/* ========================================================================
 * Copyright (c) 2002-2026 OPC Foundation. All rights reserved.
 *
 * OPC Foundation MIT License 1.00
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * The complete license agreement can be found here:
 * http://opcfoundation.org/License/MIT/1.00/
 * ======================================================================*/

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
