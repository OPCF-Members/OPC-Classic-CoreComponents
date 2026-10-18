/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcDaBrowseElement_H_
#define _COpcDaBrowseElement_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//============================================================================
// CLASS:   COpcDaBrowseElement
// PURPOSE: Describes an element in the server namespace.

class COpcDaBrowseElement : public COpcBrowseElement
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcDaBrowseElement(COpcDaBrowseElement* pParent);

    // Destructor
    ~COpcDaBrowseElement() { Clear(); }

    //========================================================================
    // Public Methods

    // Browse
    void Browse(
        OPCBROWSETYPE     eType, 
        const COpcString& cPath,
        COpcStringList&   cNodes
    );

protected:

	//========================================================================
    // Protected Methods

    // CreateInstance
    virtual COpcBrowseElement* CreateInstance();
};

#endif // _COpcDaBrowseElement_H_
