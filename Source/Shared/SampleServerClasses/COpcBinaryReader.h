/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
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
