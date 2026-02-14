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

#ifndef _COpcBinaryWriter_H
#define _COpcBinaryWriter_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcBinaryStream.h"

//============================================================================
// CLASS:   COpcBinaryWriter
// PURPOSE: A class that writes a complex data item to a binary buffer.

class COpcBinaryWriter : public COpcBinaryStream
{    
	OPC_CLASS_NEW_DELETE_ARRAY();

public:

    //========================================================================
    // Public Operators

    // Constructor
	COpcBinaryWriter() {}

    // Destructor
    ~COpcBinaryWriter() {}

	//========================================================================
    // Public Methods

	// Reads a value of the specified type from the buffer.
	bool Write(
		OpcXml::AnyType&    cValue, 
		COpcTypeDictionary* pDictionary, 
		const COpcString&   cTypeName,
		BYTE**              ppBuffer,
		UINT*               pBufSize
	);

	// Write
	bool Write(
		OpcXml::AnyType&    cValue, 
		COpcTypeDictionary* pDictionary, 
		const COpcString&   cTypeID,
		COpcXmlDocument&    cDocument
	);
	
private:

	//========================================================================
    // Private Methods

	// Writes an instance of a type to the buffer
	bool WriteType(
		COpcContext      cContext, 
		OpcXml::AnyType& cValue, 
		UINT&            uBytesWritten
	);

	// Writes the value contained in a field to the buffer.
	bool WriteField(
		COpcContext      cContext, 
		COpcFieldType*   pField,
		int              iFieldIndex,
		OpcXml::AnyType& cFieldValues, 
		OpcXml::AnyType& cFieldValue, 
		UINT&            uBytesWritten,
		UINT&            uBitOffset
	);

	// WriteTypeReference
	bool WriteTypeReference(
		COpcContext      cContext, 
		COpcFieldType*   pField,
		OpcXml::AnyType& cFieldValue, 
		UINT&            uBytesWritten
	);

	// WriteTypeReference
	bool WriteInteger(
		COpcContext      cContext, 
		COpcFieldType*   pField,
		OpcXml::AnyType& cFieldValue, 
		UINT&            uBytesWritten
	);

	// WriteFloatingPoint
	bool WriteFloatingPoint(
		COpcContext      cContext, 
		COpcFieldType*   pField,
		OpcXml::AnyType& cFieldValue, 
		UINT&            uBytesWritten
	);
	
	// WriteCharString
	bool WriteCharString(
		COpcContext      cContext, 
		COpcFieldType*   pField,
		int              iFieldIndex,
		OpcXml::AnyType& cFieldValues, 
		OpcXml::AnyType& cFieldValue, 
		UINT&            uBytesWritten
	);
	
	// WriteBitString
	bool WriteBitString(
		COpcContext      cContext, 
		COpcFieldType*   pField,
		OpcXml::AnyType& cFieldValue, 
		UINT&            uBytesWritten,
		UINT&            uBitOffset
	);

	// WriteArrayField
	bool WriteArrayField(
		COpcContext      cContext, 
		COpcFieldType*   pField, 
		UINT             uFieldIndex,
		OpcXml::AnyType& cFieldValues, 
		OpcXml::AnyType& cFieldValue,
		UINT&            uBytesWritten
	);

	// WriteReference
	bool WriteReference(
		COpcContext       cContext,
		COpcFieldType*    pField, 
		UINT              uFieldIndex,
		OpcXml::AnyType&  cFieldValues, 
		const COpcString& cFieldName,
		UINT              uCount
	);
};
	
#endif // _COpcBinaryWriter_H
