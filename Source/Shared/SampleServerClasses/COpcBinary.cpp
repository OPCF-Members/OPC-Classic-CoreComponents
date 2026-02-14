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

#include "StdAfx.h"
#include "COpcBinary.h"

//============================================================================
// Local Declarations

#define TAG_FIELD                   _T("Field")
#define TAG_BIT_STRING              _T("BitString")
#define TAG_INTEGER                 _T("Integer")
#define TAG_INT8                    _T("Int8")
#define TAG_INT16                   _T("Int16")
#define TAG_INT32                   _T("Int32")
#define TAG_INT64                   _T("Int64")
#define TAG_UINT8                   _T("UInt8")
#define TAG_UINT16                  _T("UInt16")
#define TAG_UINT32                  _T("UInt32")
#define TAG_UINT64                  _T("UInt64")
#define TAG_FLOATING_POINT          _T("FloatingPoint")
#define TAG_SINGLE                  _T("Single")
#define TAG_DOUBLE                  _T("Double")
#define TAG_CHAR_STRING             _T("CharString")
#define TAG_ASCII                   _T("Ascii")
#define TAG_UNICODE                 _T("Unicode")
#define TAG_TYPE_REFERENCE          _T("TypeReference")
#define TAG_DICTIONARY_NAME         _T("DictionaryName")
#define TAG_FIELD_NAME              _T("Name")
#define TAG_TYPE_ID                 _T("TypeID")
#define TAG_FORMAT                  _T("Format")
#define TAG_LENGTH                  _T("Length")
#define TAG_ELEMENT_COUNT           _T("ElementCount")
#define TAG_ELEMENT_COUNT_REF       _T("ElementCountRef")
#define TAG_FIELD_TERMINATOR        _T("FieldTerminator")
#define TAG_SIGNED                  _T("Signed")
#define TAG_FLOAT_FORMAT            _T("FieldFormat")
#define TAG_CHAR_WIDTH              _T("CharWidth")
#define TAG_STRING_ENCODING         _T("StringEncoding")
#define TAG_CHAR_COUNT_REF          _T("CharCountRef")
#define TAG_DEFAULT_BIG_ENDIAN      _T("DefaultBigEndian")
#define TAG_DEFAULT_CHAR_WIDTH      _T("DefaultCharWidth")
#define TAG_DEFAULT_STRING_ENCODING _T("DefaultStringEncoding")
#define TAG_DEFAULT_FLOAT_FORMAT    _T("DefaultFloatFormat")

// default values.
#define DEFAULT_BIG_ENDIAN      true
#define DEFAULT_STRING_ENCODING _T("UCS-2");
#define DEFAULT_CHAR_WIDTH      2
#define DEFAULT_FLOAT_FORMAT    _T("IEEE-754");

// GetFieldName
COpcString OpcBinaryGetFieldName(const COpcFieldType& cField)
{
	COpcString cName = cField.Name;

	if (!cName.IsEmpty())
	{
		return cName;
	}

	return OpcBinaryGetFieldType(cField);
}

// GetFieldType
COpcString OpcBinaryGetFieldType(const COpcFieldType& cField)
{
	switch (cField.Type)
	{	
		case OPC_BINARY_BIT_STRING:     { return TAG_BIT_STRING;     }
		case OPC_BINARY_INTEGER:        { return TAG_INTEGER;        }
		case OPC_BINARY_INT8:           { return TAG_INT8;           }
		case OPC_BINARY_INT16:          { return TAG_INT16;          }
		case OPC_BINARY_INT32:          { return TAG_INT32;          }
		case OPC_BINARY_INT64:          { return TAG_INT64;          }
		case OPC_BINARY_UINT8:          { return TAG_UINT8;          }
		case OPC_BINARY_UINT16:         { return TAG_UINT16;         }
		case OPC_BINARY_UINT32:         { return TAG_UINT32;         }
		case OPC_BINARY_UINT64:         { return TAG_UINT64;         }
		case OPC_BINARY_FLOATING_POINT: { return TAG_FLOATING_POINT; }
		case OPC_BINARY_SINGLE:         { return TAG_SINGLE;         }
		case OPC_BINARY_DOUBLE:         { return TAG_DOUBLE;         }
		case OPC_BINARY_CHAR_STRING:    { return TAG_CHAR_STRING;    }
		case OPC_BINARY_ASCII:          { return TAG_ASCII;          }
		case OPC_BINARY_UNICODE:        { return TAG_CHAR_STRING;    }
		case OPC_BINARY_TYPE_REFERENCE: { return cField.TypeID;     }
	}

	return TAG_FIELD;
}


//============================================================================
// COpcFieldType

// Init
void COpcFieldType::Init()
{
	Name                     = (LPCWSTR)NULL;
	Type                     = 0;
	Format                   = (LPCWSTR)NULL;
	Length                   = 0;
	LengthSpecified          = false;
	ElementCount             = 0;
	ElementCountSpecified    = false;
	ElementCountRef          = (LPCWSTR)NULL;
	ElementCountRefSpecified = false;
	FieldTerminator          = (LPCWSTR)NULL;
	FieldTerminatorSpecified = false;
	Signed                   = false;
	SignedSpecified          = false;
	FloatFormat              = (LPCWSTR)NULL;
	FloatFormatSpecified     = false;
	CharWidth                = false;
	CharWidthSpecified       = false;
	StringEncoding           = (LPCWSTR)NULL;
	StringEncodingSpecified  = false;
	CharCountRef             = (LPCWSTR)NULL;
	CharCountRefSpecified    = false;
	TypeID                   = (LPCWSTR)NULL; 
	TypeIDSpecified          = false;
}

// Clear
void COpcFieldType::Clear()
{	
	Init();
}

// Read
bool COpcFieldType::Read(COpcXmlElement& cElement)
{
	Clear();

	OpcXml::QName cType;
	
	if (cElement.GetType(cType))
	{
		COpcString cTypeName = cType.GetName();

		if (cTypeName == TAG_BIT_STRING)     Type = OPC_BINARY_BIT_STRING;
		if (cTypeName == TAG_INTEGER)        Type = OPC_BINARY_INTEGER;
		if (cTypeName == TAG_INT8)           Type = OPC_BINARY_INT8;
		if (cTypeName == TAG_INT16)          Type = OPC_BINARY_INT16;
		if (cTypeName == TAG_INT32)          Type = OPC_BINARY_INT32;
		if (cTypeName == TAG_INT64)          Type = OPC_BINARY_INT64;
		if (cTypeName == TAG_UINT8)          Type = OPC_BINARY_UINT8;
		if (cTypeName == TAG_UINT16)         Type = OPC_BINARY_UINT16;
		if (cTypeName == TAG_UINT32)         Type = OPC_BINARY_UINT32;
		if (cTypeName == TAG_UINT64)         Type = OPC_BINARY_UINT64;
		if (cTypeName == TAG_FLOATING_POINT) Type = OPC_BINARY_FLOATING_POINT;
		if (cTypeName == TAG_SINGLE)         Type = OPC_BINARY_SINGLE;
		if (cTypeName == TAG_DOUBLE)         Type = OPC_BINARY_DOUBLE;
		if (cTypeName == TAG_CHAR_STRING)    Type = OPC_BINARY_CHAR_STRING;
		if (cTypeName == TAG_ASCII)          Type = OPC_BINARY_ASCII;
		if (cTypeName == TAG_UNICODE)        Type = OPC_BINARY_UNICODE;
		if (cTypeName == TAG_TYPE_REFERENCE) Type = OPC_BINARY_TYPE_REFERENCE;

		if (Type == 0)
		{
			return false;
		}
	}

	READ_ATTRIBUTE(TAG_FIELD_NAME, Name);
	READ_ATTRIBUTE(TAG_FORMAT, Format);
	READ_OPTIONAL_ATTRIBUTE(TAG_LENGTH, Length);
	READ_OPTIONAL_ATTRIBUTE(TAG_ELEMENT_COUNT, ElementCount);
	READ_OPTIONAL_ATTRIBUTE(TAG_ELEMENT_COUNT_REF, ElementCountRef)
	READ_OPTIONAL_ATTRIBUTE(TAG_FIELD_TERMINATOR, FieldTerminator);
	READ_OPTIONAL_ATTRIBUTE(TAG_SIGNED, Signed);
	READ_OPTIONAL_ATTRIBUTE(TAG_FLOAT_FORMAT, FloatFormat);
	READ_OPTIONAL_ATTRIBUTE(TAG_CHAR_WIDTH, CharWidth);
	READ_OPTIONAL_ATTRIBUTE(TAG_STRING_ENCODING, StringEncoding);
	READ_OPTIONAL_ATTRIBUTE(TAG_CHAR_COUNT_REF, CharCountRef);
	READ_OPTIONAL_ATTRIBUTE(TAG_TYPE_ID, TypeID);

	return true;
}

// Write
bool COpcFieldType::Write(COpcXmlElement& cElement)
{
	OPC_ASSERT(false);

	// not implemented.

	return false;
}

//============================================================================
// COpcTypeDescription

// Init
void COpcTypeDescription::Init()
{
	TypeID                         = (LPCWSTR)NULL;
	DefaultBigEndian               = false;
	DefaultBigEndianSpecified      = false;
	DefaultCharWidth               = 0;
	DefaultCharWidthSpecified      = false;
	DefaultStringEncoding          = (LPCWSTR)NULL;
	DefaultStringEncodingSpecified = false;
	DefaultFloatFormat             = (LPCWSTR)NULL;
	DefaultFloatFormatSpecified    = false;
}

// Clear
void COpcTypeDescription::Clear()
{	
	for (UINT ii = 0; ii < Fields.GetSize(); ii++)
	{
		delete Fields[ii];
	}

	Fields.RemoveAll();

	Init();
}

// Read
bool COpcTypeDescription::Read(COpcXmlElement& cElement)
{
	Clear();

	READ_ATTRIBUTE(TAG_TYPE_ID, TypeID);
	READ_OPTIONAL_ATTRIBUTE(TAG_DEFAULT_BIG_ENDIAN, DefaultBigEndian);
	READ_OPTIONAL_ATTRIBUTE(TAG_DEFAULT_CHAR_WIDTH, DefaultCharWidth);
	READ_OPTIONAL_ATTRIBUTE(TAG_DEFAULT_STRING_ENCODING, DefaultStringEncoding);
	READ_OPTIONAL_ATTRIBUTE(TAG_DEFAULT_FLOAT_FORMAT, DefaultFloatFormat);

	COpcXmlElementList cChildren;

	if (cElement.GetChildren(cChildren) > 0)
	{
		Fields.SetSize(cChildren.GetSize());

		for (UINT ii = 0; ii < cChildren.GetSize(); ii++)
		{
			COpcFieldType* pField = new COpcFieldType();

			if (!pField->Read(cChildren[ii]))
			{
				delete pField;
				return false;
			}

			Fields[ii] = pField;
		}
	}

	return true;
}

// Write
bool COpcTypeDescription::Write(COpcXmlElement& cElement)
{
	OPC_ASSERT(false);

	// not implemented.

	return false;
}

//============================================================================
// COpcTypeDictionary

// Init
void COpcTypeDictionary::Init()
{
	Name                      = (LPCWSTR)NULL;
	DefaultBigEndian          = DEFAULT_BIG_ENDIAN;
	DefaultCharWidth          = DEFAULT_CHAR_WIDTH;
	DefaultStringEncoding     = DEFAULT_STRING_ENCODING;
	DefaultFloatFormat        = DEFAULT_FLOAT_FORMAT;
}

// Clear
void COpcTypeDictionary::Clear()
{	
	for (UINT ii = 0; ii < Types.GetSize(); ii++)
	{
		delete Types[ii];
	}

	Types.RemoveAll();

	Init();
}

// Read
bool COpcTypeDictionary::Read(COpcXmlElement& cElement)
{
	Clear();

	READ_ATTRIBUTE(TAG_DICTIONARY_NAME, Name);
	READ_DEFAULT_ATTRIBUTE(TAG_DEFAULT_BIG_ENDIAN, DefaultBigEndian, DEFAULT_BIG_ENDIAN);
	READ_DEFAULT_ATTRIBUTE(TAG_DEFAULT_CHAR_WIDTH, DefaultCharWidth, DEFAULT_CHAR_WIDTH);
	READ_DEFAULT_ATTRIBUTE(TAG_DEFAULT_STRING_ENCODING, DefaultStringEncoding, DEFAULT_STRING_ENCODING);
	READ_DEFAULT_ATTRIBUTE(TAG_DEFAULT_FLOAT_FORMAT, DefaultFloatFormat, DEFAULT_FLOAT_FORMAT);

	COpcXmlElementList cChildren;

	if (cElement.GetChildren(cChildren) > 0)
	{
		Types.SetSize(cChildren.GetSize());

		for (UINT ii = 0; ii < cChildren.GetSize(); ii++)
		{
			COpcTypeDescription* pType = new COpcTypeDescription();

			if (!pType->Read(cChildren[ii]))
			{
				delete pType;
				return false;
			}

			Types[ii] = pType;
		}
	}

	return true;
}

// Write
bool COpcTypeDictionary::Write(COpcXmlElement& cElement)
{
	OPC_ASSERT(false);

	// not implemented.

	return false;
}
