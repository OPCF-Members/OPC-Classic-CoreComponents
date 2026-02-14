//============================================================================
// TITLE: COpcDaBrowseElement.cpp
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

#include "StdAfx.h"
#include "COpcDaBrowseElement.h"

//============================================================================
// Local Declarations

#define DEFAULT_SEPARATOR _T("/")

//============================================================================
// COpcDaBrowseElement

// Constructor
COpcDaBrowseElement::COpcDaBrowseElement(COpcDaBrowseElement* pParent)
: 
	COpcBrowseElement(pParent)
{ 
}

// CreateInstance
COpcBrowseElement* COpcDaBrowseElement::CreateInstance()
{
	return new COpcDaBrowseElement(this);
}

// Browse
void COpcDaBrowseElement::Browse(
    OPCBROWSETYPE     eType, 
    const COpcString& cPath,
    COpcStringList&   cNodes
)
{
    if (eType != OPC_FLAT)
    {
        OPC_POS pos = m_cChildren.GetHeadPosition();

        while (pos != NULL)
        {
            COpcBrowseElement* pNode = m_cChildren.GetNext(pos);

            // add child to list.
            cNodes.AddTail(pNode->GetName());
        }
    }

    else
    {
        OPC_POS pos = m_cChildren.GetHeadPosition();

        while (pos != NULL)
        {
            COpcDaBrowseElement* pNode = (COpcDaBrowseElement*)m_cChildren.GetNext(pos);

            // add child to list.
            cNodes.AddTail(cPath + pNode->GetName());

            // recursively browse children.
            if (pNode->m_cChildren.GetCount() > 0)
            {
                ((COpcDaBrowseElement*)pNode)->Browse(eType, cPath + pNode->GetName() + GetSeparator(), cNodes);
            }
        }
    }
}

