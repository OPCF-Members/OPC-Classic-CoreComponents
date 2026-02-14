//============================================================================
// TITLE: COpcBinaryReader.h
//
// CONTENTS:
// 
// A class that reads a complex data item from a binary buffer.
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
// 2003/10/01 RSA   First release.

#ifndef _COpcBinaryStream_H
#define _COpcBinaryStream_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcBinary.h"

#define OPC_MODE_BINARY 0
#define OPC_MODE_XML    1

//============================================================================
// CLASS:   COpcContext
// PURPOSE: Stores the current serialization cContext.

struct COpcContext
{    
	OPC_CLASS_NEW_DELETE_ARRAY();

public:

    //========================================================================
    // Public Operators

    // Constructor
	COpcContext() { Init(); }

	// Copy Constructor
	COpcContext(const COpcContext& cContext) { Init(); *this = cContext; }

    // Destructor
    ~COpcContext() {}

	// Assignment
	COpcContext& operator=(const COpcContext& cContext) 
	{
	    Index          = cContext.Index;
	    Dictionary     = cContext.Dictionary;
	    Type           = cContext.Type;
	    BigEndian      = cContext.BigEndian;
	    CharWidth      = cContext.CharWidth;
		StringEncoding = cContext.StringEncoding;
		FloatFormat    = cContext.FloatFormat;
		Mode           = cContext.Mode;
	    Buffer         = cContext.Buffer;
		BufSize        = cContext.BufSize;
		Element        = cContext.Element;

		return *this;
	}

    //========================================================================
    // Public Properties

	// serialization context.
	UINT                 Index;
	COpcTypeDictionary*  Dictionary;
	COpcTypeDescription* Type;
	bool                 BigEndian;
	UINT                 CharWidth;
	COpcString           StringEncoding;
	COpcString           FloatFormat;
	// indicates whether to use XML or Binary serialization.
	int                  Mode;
	// binary buffer for binary serialization.
	BYTE*                Buffer;
	UINT                 BufSize;
	// XML element for XML serialization.
	COpcXmlElement       Element;

	//========================================================================
    // Public Methods

	// Init
	void Init()
	{
	    Index          = 0;
	    Dictionary     = NULL;
	    Type           = NULL;
	    BigEndian      = false;
	    CharWidth      = 0;
		StringEncoding = (LPCWSTR)NULL;
		FloatFormat    = (LPCWSTR)NULL;
		Mode           = OPC_MODE_BINARY;
	    Buffer         = NULL;
	    BufSize        = 0;
	}
};

//============================================================================
// CLASS:   COpcBinaryReader
// PURPOSE: A class that reads a complex data item from a binary buffer.

class COpcBinaryStream
{    
	OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
	COpcBinaryStream() {}

    // Destructor
    ~COpcBinaryStream() {}
	
	// Determines if a pField contains an array of values.
	bool IsArrayField(COpcFieldType* pField, bool& bIsArray);

	// Looks up the type name in the dictionary and initializes the context.
	bool InitializeContext(
		COpcTypeDictionary* pDictionary, 
		const COpcString&   cTypeName, 
		COpcContext&        cContext
	);
	
	// swaps the order of bytes in the buffer.
	void SwapBytes(BYTE* pBuffer, UINT uLength);

	// ReadByteString
	bool ReadByteString(
		COpcContext& cContext, 
		BYTE*        pString,
		UINT         uLength
	);

	// WriteByteString
	bool WriteByteString(
		COpcContext& cContext, 
		BYTE*        pString,
		UINT         uLength
	);

	// Converts the terminator from a string to an instance of the field value.
	bool GetTerminator(
		COpcContext      cContext, 
		COpcFieldType*   pField,
		OpcXml::AnyType& cTerminator
	);
	
	// GetFieldName
	COpcString GetFieldName(COpcFieldType* pField);

	// GetFieldType
	COpcString GetFieldType(COpcFieldType* pField);
	
	// Reads a string containing a sequence of bytes into an array.
	bool ReadBytes(LPCWSTR wszBuffer, BYTE* pBuffer, UINT uLength);

	// Gets the standard XML type for an integer field.
	OpcXml::Type GetIntegerType(COpcFieldType* pField, UINT& uLength, bool& bSigned);
	
	// Gets the standard XML type for a floating point field.
	OpcXml::Type GetFloatingPointType(COpcFieldType* pField, UINT& uLength, COpcString& cFormat);
};

#endif // _COpcBinaryStream_H
