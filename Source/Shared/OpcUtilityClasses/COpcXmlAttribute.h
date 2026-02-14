//==============================================================================
// TITLE: COpcXmlAttribute.h
//
// CONTENTS:
// 
// A class that represents an XML attribute.
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
//

#ifndef _COpcXmlAttribute_H_
#define _COpcXmlAttribute_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcString.h"
#include "COpcArray.h"
#include "OpcXmlType.h"

//==============================================================================
// CLASS:   COpcXmlAttribute
// PURPOSE  Represents an XML attribute.

class OPCUTILS_API COpcXmlAttribute 
{
    OPC_CLASS_NEW_DELETE_ARRAY();

public:

    //==========================================================================
    // Public Operators

    // Constructor
    COpcXmlAttribute(IUnknown* ipUnknown = NULL);

    // Copy Constructor
    COpcXmlAttribute(const COpcXmlAttribute& cAttribute);
            
    // Destructor
    ~COpcXmlAttribute();

    // Assignment
    COpcXmlAttribute& operator=(IUnknown* ipUnknown);
    COpcXmlAttribute& operator=(const COpcXmlAttribute& cAttribute);

    // Accessor
    operator IXMLDOMAttribute*() const { return m_ipAttribute; }

    //==========================================================================
    // Public Methods
    
    // GetName
    COpcString GetName();
    	
	// Prefix
    COpcString GetPrefix();   
        
	// Namespace
	COpcString GetNamespace();

	// GetQualifiedName
	OpcXml::QName GetQualifiedName();

    // GetValue
    COpcString GetValue();
   
protected:

    //==========================================================================
    // Private Members

    IXMLDOMAttribute* m_ipAttribute;
};

//==============================================================================
// TYPE:    COpcXmlAttributeList
// PURPOSE: A list of elements.

typedef COpcArray<COpcXmlAttribute> COpcXmlAttributeList;

#endif // _COpcXmlAttribute_H_ 
