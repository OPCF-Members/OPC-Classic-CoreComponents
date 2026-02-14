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

#ifndef _COpcBinary_H
#define _COpcBinary_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//============================================================================
// MACROS:  OPC_BINARY_XXX
// PURPOSE: Define constants for standard field definition types.

#define OPC_BINARY_BIT_STRING     1
#define OPC_BINARY_INTEGER        2
#define OPC_BINARY_INT8           3
#define OPC_BINARY_INT16          4
#define OPC_BINARY_INT32          5
#define OPC_BINARY_INT64          6
#define OPC_BINARY_UINT8          7
#define OPC_BINARY_UINT16         8
#define OPC_BINARY_UINT32         9
#define OPC_BINARY_UINT64         10
#define OPC_BINARY_FLOATING_POINT 11
#define OPC_BINARY_SINGLE         12
#define OPC_BINARY_DOUBLE         13
#define OPC_BINARY_CHAR_STRING    14
#define OPC_BINARY_ASCII          15
#define OPC_BINARY_UNICODE        16
#define OPC_BINARY_TYPE_REFERENCE 17

//============================================================================
// MACROS:  OPC_FLOAT_FORMAT_XXX
// PURPOSE: Define constants for standard float formats.

#define OPC_FLOAT_FORMAT_IEEE754 _T("IEEE-754")

//============================================================================
// MACROS:  OPC_STRING_ENCODING_XXX
// PURPOSE: Define constants for standard string encodings.

#define OPC_STRING_ENCODING_ASCII _T("ASCII")
#define OPC_STRING_ENCODING_UCS2  _T("UCS-2")

//============================================================================
// CLASS:   COpcFieldType
// PURPOSE: Contains the definition of a field within a type description.

class COpcFieldType : public IOpcXmlSerialize
{
    OPC_CLASS_NEW_DELETE_ARRAY()

public:

    //========================================================================
    // Public Operators

    // Constructor
	COpcFieldType() { Init(); }

    // Destructor
    ~COpcFieldType() { Clear(); }
    	
	//=========================================================================
    // Public Properties

	COpcString Name;
	UINT       Type;
	COpcString Format;
	UINT       Length;
	bool       LengthSpecified;
	UINT       ElementCount;
	bool       ElementCountSpecified;
	COpcString ElementCountRef;
	bool       ElementCountRefSpecified;
	COpcString FieldTerminator;
	bool       FieldTerminatorSpecified;
	bool       Signed;
	bool       SignedSpecified;
	COpcString FloatFormat;
	bool       FloatFormatSpecified;
	UINT       CharWidth;
	bool       CharWidthSpecified;
	COpcString StringEncoding;
	bool       StringEncodingSpecified;
	COpcString CharCountRef;
	bool       CharCountRefSpecified;
	COpcString TypeID;
	bool       TypeIDSpecified;

	//========================================================================
    // IOpcXmlSerialize
    
    // Init
    virtual void Init();

    // Clear
    virtual void Clear();

    // Read
    virtual bool Read(COpcXmlElement& cElement);

    // Write
    virtual bool Write(COpcXmlElement& cElement);
};

//============================================================================
// TYPEDEF: COpcFieldTypeList
// PURPOSE: A list of field definitions.

typedef COpcArray<COpcFieldType*> COpcFieldTypeList;

//============================================================================
// CLASS:   COpcTypeDescription
// PURPOSE: Contains the decription of a type within a type dictionary.

class COpcTypeDescription : public IOpcXmlSerialize
{
    OPC_CLASS_NEW_DELETE_ARRAY()

public:

    //========================================================================
    // Public Operators

    // Constructor
	COpcTypeDescription() { Init(); }

    // Destructor
    ~COpcTypeDescription() { Clear(); }
    	
	//=========================================================================
    // Public Properties

	COpcString              TypeID;
	bool                    DefaultBigEndian;
	bool                    DefaultBigEndianSpecified;
	UINT                    DefaultCharWidth;
	bool                    DefaultCharWidthSpecified;
	COpcString              DefaultStringEncoding;
	bool					DefaultStringEncodingSpecified;
	COpcString              DefaultFloatFormat;
	bool					DefaultFloatFormatSpecified;
	COpcFieldTypeList Fields;

	//========================================================================
    // IOpcXmlSerialize
    
    // Init
    virtual void Init();

    // Clear
    virtual void Clear();

    // Read
    virtual bool Read(COpcXmlElement& cElement);

    // Write
    virtual bool Write(COpcXmlElement& cElement);
};

//============================================================================
// TYPEDEF: COpcTypeDescriptionList
// PURPOSE: A list of type descriptions.

typedef COpcArray<COpcTypeDescription*> COpcTypeDescriptionList;

//============================================================================
// CLASS:   COpcTypeDictionary
// PURPOSE: Contains a set of complex type descriptions.

class COpcTypeDictionary : public IOpcXmlSerialize
{
    OPC_CLASS_NEW_DELETE_ARRAY()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcTypeDictionary() { Init(); }

    // Destructor
    ~COpcTypeDictionary() { Clear(); }
    	
	//=========================================================================
    // Public Properties

	COpcString              Name;
	bool                    DefaultBigEndian;
	UINT                    DefaultCharWidth;
	COpcString              DefaultStringEncoding;
	COpcString              DefaultFloatFormat;
	COpcTypeDescriptionList Types;

	//========================================================================
    // IOpcXmlSerialize
    
    // Init
    virtual void Init();

    // Clear
    virtual void Clear();

    // Read
    virtual bool Read(COpcXmlElement& cElement);

    // Write
    virtual bool Write(COpcXmlElement& cElement);
};

//============================================================================
// FUNCTION: OpcBinaryGetFieldName
// PURPOSE:  Gets a non-null name for a field.

COpcString OpcBinaryGetFieldName(const COpcFieldType& cField);

//============================================================================
// FUNCTION: OpcBinaryGetFieldType
// PURPOSE:  Gets a non-null name for a field type.

COpcString OpcBinaryGetFieldType(const COpcFieldType& cField);

#endif // _COpcBinary_H
