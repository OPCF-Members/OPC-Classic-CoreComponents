//============================================================================
// TITLE: COpcBinary.h
//
// CONTENTS:
// 
// A classes that stores the OPCBinary schema components in memory.
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
// 2003/10/01 RSA   Initial implementation.

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
