//============================================================================
// TITLE: COpcDaBrowseElement.h
//
// CONTENTS:
// 
// A single element in the OPC server namespace.
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
// 2002/11/16 RSA   Second release.
//

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
