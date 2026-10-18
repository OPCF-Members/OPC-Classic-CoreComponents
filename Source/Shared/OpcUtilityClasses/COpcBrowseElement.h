/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcBrowseElement_H_
#define _COpcBrowseElement_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcString.h"
#include "COpcList.h"

//============================================================================
// TYPE:    COpcBrowseElementList
// PURPOSE: A ordered list of server namespace elements.

class COpcBrowseElement;
typedef COpcList<COpcBrowseElement*> COpcBrowseElementList;

//============================================================================
// CLASS:   COpcBrowseElement
// PURPOSE: Describes an element in the server namespace.

class COpcBrowseElement
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcBrowseElement(COpcBrowseElement* pParent);

    // Destructor
    ~COpcBrowseElement() { Clear(); }

    //========================================================================
    // Public Methods
    
    // Init
    void Init();

    // Clear
    void Clear();

    // GetName
    COpcString GetName() const;

    // GetItemID
    COpcString GetItemID() const;

    // GetBrowsePath
    COpcString GetBrowsePath() const;

    // GetSeparator
    COpcString GetSeparator() const;

    // GetParent
    COpcBrowseElement* GetParent() const { return m_pParent; }

    // GetChild
    COpcBrowseElement* GetChild(UINT uIndex) const;

	// Browse
	void Browse(
		const COpcString& cPath,
		bool              bFlat, 
		COpcStringList&   cNodes
	);

    // Find
    COpcBrowseElement* Find(const COpcString& cPath);
    
    // Insert
    COpcBrowseElement* Insert(const COpcString& cPath);

    // Insert
    COpcBrowseElement* Insert(
        const COpcString& cPath,
        const COpcString& cItemID
    );

    // Remove
    void Remove();

    // Remove
    bool Remove(const COpcString& cName);

protected:
    
	//========================================================================
    // Protected Methods

    // CreateInstance
    virtual COpcBrowseElement* CreateInstance();

    //========================================================================
    // Protected Members

    COpcBrowseElement* m_pParent;
    COpcString         m_cItemID;
    COpcString         m_cName;
    COpcString         m_cSeparator;

    COpcBrowseElementList m_cChildren;
};

#endif // _COpcBrowseElement_H_
