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
