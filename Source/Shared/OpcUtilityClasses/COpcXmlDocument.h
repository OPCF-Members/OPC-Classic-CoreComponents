//==============================================================================
// TITLE: COpcXmlDocument.h
//
// CONTENTS:
// 
// A class thatstrings to and from xml data types.
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
// 2003/03/23 RSA   Added XPath based search capabilities.
//

#ifndef _COpcXmlDocument_H_
#define _COpcXmlDocument_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcMap.h"
#include "OpcXmlType.h"
#include "COpcXmlElement.h"

//==============================================================================
// CLASS:   COpcXmlDocument
// PURPOSE  Facilitiates manipulation of XML documents,

class OPCUTILS_API COpcXmlDocument 
{
    OPC_CLASS_NEW_DELETE()

public:

    //==========================================================================
    // Public Operators

    // Constructor
    COpcXmlDocument(IXMLDOMDocument* ipUnknown = NULL);

    // Copy Constructor
    COpcXmlDocument(const COpcXmlDocument& cDocument);
            
    // Destructor
    ~COpcXmlDocument();

    // Assignment
    COpcXmlDocument& operator=(IUnknown* ipUnknown);
    COpcXmlDocument& operator=(const COpcXmlDocument& cDocument);

    // Accessor
    operator IXMLDOMDocument*() const { return m_ipDocument; }

    //==========================================================================
    // Public Methods
    
    // Init
    virtual bool Init();

    // Clear
    virtual void Clear();

	// New
    virtual bool New();

    // New
    virtual bool New(const COpcString& cRoot, const COpcString& cDefaultNamespace);

	// New
    virtual bool New(IXMLDOMElement* ipElement);

    // Init
    virtual bool LoadXml(LPCWSTR szXml);

    // Load
    virtual bool Load(const COpcString& cFilePath = OPC_EMPTY_STRING);

    // Save
    virtual bool Save(const COpcString& cFilePath = OPC_EMPTY_STRING);

    // GetRoot
    COpcXmlElement GetRoot() const;

	// GetXml
	bool GetXml(COpcString& cXml) const;

    // GetDefaultNamespace
    COpcString GetDefaultNamespace();

    // AddNamespace
    bool AddNamespace(const COpcString& cPrefix, const COpcString& cNamespace);

	// GetNamespaces
	void GetNamespaces(COpcStringMap& cNamespaces);

	// GetNamespacePrefix
	COpcString GetNamespacePrefix(const COpcString& cNamespace);

	// FindElement
	COpcXmlElement FindElement(const COpcString& cXPath);
	
	// FindElements
	UINT FindElements(const COpcString& cXPath, COpcXmlElementList& cElements);

protected:
    
    //==========================================================================
    // Protected Methods

    // GetFilePath
    const COpcString& GetFilePath() const { return m_cFilePath; }

    // SetFilePath
    void SetFilePath(const COpcString& cFilePath) { m_cFilePath = cFilePath; }

private:

    //==========================================================================
    // Private Members

    COpcString       m_cFilePath;
    IXMLDOMDocument* m_ipDocument;
};

#endif // _COpcXmlDocument_H_ 
