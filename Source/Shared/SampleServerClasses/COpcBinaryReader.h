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

#ifndef _COpcBinaryReader_H
#define _COpcBinaryReader_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcBinaryStream.h"

//============================================================================
// CLASS:   COpcBinaryReader
// PURPOSE: A class that reads a complex data item from a binary buffer.

class COpcBinaryReader : public COpcBinaryStream
{    
	OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
	COpcBinaryReader() {}

    // Destructor
    ~COpcBinaryReader() {}

    //========================================================================
    // Public Methods

	// Reads a value of the specified type from the buffer.
	bool Read(
		BYTE*               pBuffer, 
		UINT                uBufSize,
		COpcTypeDictionary* pDictionary, 
		const COpcString&   cTypeName, 
		OpcXml::AnyType&    cValue
	);

	// Reads a value of the specified type from the document.
	bool Read(
		COpcXmlDocument&    cDocument,
		COpcTypeDictionary* pDictionary, 
		const COpcString&   cTypeName, 
		OpcXml::AnyType&    cValue
	);

private:

    //========================================================================
    // Private Methods

	// Reads an instance of a type from the buffer.
	bool ReadType(
		COpcContext      cContext, 
		OpcXml::AnyType& cValue, 
		UINT&            uBytesRead
	);

	// Reads the value contained in a field from the buffer.
	bool ReadField(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		int              iFieldIndex,
		OpcXml::AnyType& cFieldValues,
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesRead,
		UINT&            uBitOffset
	);
	
	// Reads a field containing an array of values.
	bool ReadArrayField(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		int              iFieldIndex,
		OpcXml::AnyType& cFieldValues, 
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesRead
	);

	// Reads an integer from the buffer.
	bool ReadInteger(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesRead
	);

	// Reads a floating point value from the buffer.
	bool ReadFloatingPoint(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesRead
	);

	// ReadCharString
	bool ReadCharString(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		int              iFieldIndex,
		OpcXml::AnyType& cFieldValues,
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesRead
	);

	// ReadBitString
	bool ReadBitString(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesRead,
		UINT&            uBitOffset
	);

	// Reads a complex type from the buffer.
	bool ReadTypeReference(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesRead
	);

	// Finds the integer value referenced by the field name.
	bool ReadReference(
		COpcContext       cContext, 
		COpcFieldType*    pField, 
		int               iFieldIndex,
		OpcXml::AnyType&  cFieldValues,
		const COpcString& cFieldName,
		UINT&             uCount
	);
};

#endif // _COpcBinaryReader_H
