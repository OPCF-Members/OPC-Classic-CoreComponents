//==============================================================================
// TITLE: COpcXmlAttribute.cpp
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

#include "StdAfx.h"
#include "COpcXmlAttribute.h"
#include "COpcVariant.h"

//==============================================================================
// COpcXmlAttribute

// Constructor
COpcXmlAttribute::COpcXmlAttribute(IUnknown* ipUnknown)
{
    m_ipAttribute = NULL;
    *this = ipUnknown;
}

// Copy Constructor
COpcXmlAttribute::COpcXmlAttribute(const COpcXmlAttribute& cAttribute)
{
    m_ipAttribute = NULL;
    *this = cAttribute.m_ipAttribute;
}

// Destructor
COpcXmlAttribute::~COpcXmlAttribute()
{
    if (m_ipAttribute != NULL)
    {
        m_ipAttribute->Release();
        m_ipAttribute = NULL;
    }
}

// Assignment
COpcXmlAttribute& COpcXmlAttribute::operator=(IUnknown* ipUnknown)
{
    if (m_ipAttribute != NULL)
    {
        m_ipAttribute->Release();
        m_ipAttribute = NULL;
    }

    if (ipUnknown != NULL)
    {
        HRESULT hResult = ipUnknown->QueryInterface(__uuidof(IXMLDOMAttribute), (void**)&m_ipAttribute);

        if (FAILED(hResult))
        {
            m_ipAttribute = NULL;
        }
    }

    return *this;
}

// Assignment
COpcXmlAttribute& COpcXmlAttribute::operator=(const COpcXmlAttribute& cAttribute)
{
    if (this == &cAttribute)
    {
        return *this;
    }

    *this = cAttribute.m_ipAttribute;
    return *this;
}

// GetName
COpcString COpcXmlAttribute::GetName()
{
	if (m_ipAttribute != NULL)
	{
		BSTR bstrName = NULL;

		HRESULT hResult = m_ipAttribute->get_name(&bstrName);
		OPC_ASSERT(SUCCEEDED(hResult));

		COpcString cName = bstrName;
		SysFreeString(bstrName);

		return cName;
	}

	return (LPCWSTR)NULL;
}

// GetPrefix
COpcString COpcXmlAttribute::GetPrefix()
{
	if (m_ipAttribute != NULL)
	{
		BSTR bstrName = NULL;

		HRESULT hResult = m_ipAttribute->get_prefix(&bstrName);
		OPC_ASSERT(SUCCEEDED(hResult));

		COpcString cName = bstrName;
		SysFreeString(bstrName);

		return cName;
	}

	return (LPCWSTR)NULL;
}

// GetQualifiedName
OpcXml::QName COpcXmlAttribute::GetQualifiedName()
{
	OpcXml::QName cQName;

	if (m_ipAttribute != NULL)
	{
		BSTR bstrName = NULL;

		HRESULT hResult = m_ipAttribute->get_baseName(&bstrName);
		OPC_ASSERT(SUCCEEDED(hResult));

		cQName.SetName(bstrName);
		cQName.SetNamespace(GetNamespace());
		
		SysFreeString(bstrName);
	}

	return cQName;
}

// GetNamespace
COpcString COpcXmlAttribute::GetNamespace()
{
	if (m_ipAttribute != NULL)
	{
		BSTR bstrName = NULL;

		HRESULT hResult = m_ipAttribute->get_namespaceURI(&bstrName);
		OPC_ASSERT(SUCCEEDED(hResult));

		COpcString cName = bstrName;
		SysFreeString(bstrName);

		return cName;
	}

	return (LPCWSTR)NULL;
}

// GetValue
COpcString COpcXmlAttribute::GetValue()
{
    VARIANT cVariant; OpcVariantInit(&cVariant);

    if (FAILED(m_ipAttribute->get_value(&cVariant))) { OPC_ASSERT(false); }

    COpcString cValue = cVariant.bstrVal;
    OpcVariantClear(&cVariant);

    return cValue;
}

