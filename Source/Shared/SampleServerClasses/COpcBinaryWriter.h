/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
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
