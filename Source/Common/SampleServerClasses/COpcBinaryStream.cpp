//============================================================================
// TITLE: COpcBinaryStream.cpp
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

#include "StdAfx.h"
#include "COpcBinaryStream.h"

// IsArrayField
bool COpcBinaryStream::IsArrayField(COpcFieldType* pField, bool& bIsArray)
{
	bIsArray = false;

	if (pField->ElementCountSpecified)
	{
		if (pField->ElementCountRefSpecified || pField->FieldTerminatorSpecified)
		{
			return false; 
		}

		bIsArray = true;
	}

	else if (pField->ElementCountRefSpecified)
	{
		if (pField->FieldTerminatorSpecified)
		{
			return false;
		}

		bIsArray = true;
	}

	else if (pField->FieldTerminatorSpecified)
	{
		bIsArray = true;
	}

	return true;
}

// InitializeContext
bool COpcBinaryStream::InitializeContext(
	COpcTypeDictionary* pDictionary, 
	const COpcString&   cTypeID, 
	COpcContext&        cContext
)
{
	cContext.Dictionary     = pDictionary;
	cContext.Type           = NULL;
	cContext.BigEndian      = pDictionary->DefaultBigEndian;
	cContext.CharWidth      = pDictionary->DefaultCharWidth;
	cContext.StringEncoding = pDictionary->DefaultStringEncoding;
	cContext.FloatFormat    = pDictionary->DefaultFloatFormat;

	for (UINT ii = 0; ii < cContext.Dictionary->Types.GetSize(); ii++)
	{
		COpcTypeDescription* pType = cContext.Dictionary->Types[ii];

		if (pType->TypeID == cTypeID)
		{
			cContext.Type = pType;

			if (pType->DefaultBigEndianSpecified)      cContext.BigEndian      = pType->DefaultBigEndian;
			if (pType->DefaultCharWidthSpecified)      cContext.CharWidth      = pType->DefaultCharWidth;
			if (pType->DefaultStringEncodingSpecified) cContext.StringEncoding = pType->DefaultStringEncoding;
			if (pType->DefaultFloatFormatSpecified)    cContext.FloatFormat    = pType->DefaultFloatFormat;

			break;
		}
	}

	if (cContext.Type == NULL)
	{
		return false; 
	}

	return true;
}	

// SwapBytes
void COpcBinaryStream::SwapBytes(BYTE* pBuffer, UINT uLength)
{
	for (UINT ii = 0; ii < uLength/2; ii++)
	{
		BYTE bTemp            = pBuffer[uLength-1-ii];
		pBuffer[uLength-1-ii] = pBuffer[ii];
		pBuffer[ii]           = bTemp;
	}
}

// GetFieldName
COpcString COpcBinaryStream::GetFieldName(COpcFieldType* pField)
{
	return OpcBinaryGetFieldName(*pField);
}

// GetFieldType
COpcString COpcBinaryStream::GetFieldType(COpcFieldType* pField)
{
	return OpcBinaryGetFieldType(*pField);
}

// GetIntegerType
OpcXml::Type COpcBinaryStream::GetIntegerType(COpcFieldType* pField, UINT& uLength, bool& bSigned)
{
	switch (pField->Type)
	{
		case OPC_BINARY_INT8:   { uLength = 1; bSigned = true;  return OpcXml::XML_SBYTE;  }
		case OPC_BINARY_INT16:  { uLength = 2; bSigned = true;  return OpcXml::XML_SHORT;  }
		case OPC_BINARY_INT32:  { uLength = 4; bSigned = true;  return OpcXml::XML_INT;    }
		case OPC_BINARY_INT64:  { uLength = 8; bSigned = true;  return OpcXml::XML_LONG;   }
		case OPC_BINARY_UINT8:  { uLength = 1; bSigned = false; return OpcXml::XML_BYTE;   }
		case OPC_BINARY_UINT16: { uLength = 2; bSigned = false; return OpcXml::XML_USHORT; }
		case OPC_BINARY_UINT32: { uLength = 4; bSigned = false; return OpcXml::XML_UINT;   }
		case OPC_BINARY_UINT64: { uLength = 8; bSigned = false; return OpcXml::XML_ULONG;  }

		case OPC_BINARY_INTEGER:
		{
			if (bSigned)
			{
				switch (uLength)
				{
					case 1: { return OpcXml::XML_SBYTE; }
					case 2: { return OpcXml::XML_SHORT; }
					case 4: { return OpcXml::XML_INT;   }
					case 8: { return OpcXml::XML_LONG;  }
				}
			}
			else
			{
				switch (uLength)
				{
					case 1: { return OpcXml::XML_BYTE;   }
					case 2: { return OpcXml::XML_USHORT; }
					case 4: { return OpcXml::XML_UINT;   }
					case 8: { return OpcXml::XML_ULONG;  }
				}
			}

			break;
		}
	}

	return OpcXml::XML_EMPTY;
}

// GetFloatingPointType
OpcXml::Type COpcBinaryStream::GetFloatingPointType(COpcFieldType* pField, UINT& uLength, COpcString& cFormat)
{
	switch (pField->Type)
	{
		case OPC_BINARY_SINGLE: { uLength = 4; cFormat = OPC_FLOAT_FORMAT_IEEE754;  return OpcXml::XML_FLOAT;  }
		case OPC_BINARY_DOUBLE: { uLength = 8; cFormat = OPC_FLOAT_FORMAT_IEEE754;  return OpcXml::XML_DOUBLE; }

		case OPC_BINARY_FLOATING_POINT:
		{
			if (cFormat == OPC_FLOAT_FORMAT_IEEE754)
			{
				switch (uLength)
				{
					case 4: { return OpcXml::XML_FLOAT;  }
					case 8: { return OpcXml::XML_DOUBLE; }
				}
			}

			break;
		}
	}

	return OpcXml::XML_EMPTY;
}

// ReadBytes
bool COpcBinaryStream::ReadBytes(LPCWSTR wszBuffer, BYTE* pBuffer, UINT uLength)
{
	memset(pBuffer, 0, uLength);

	UINT uStrLen = wcslen(wszBuffer);

	for (UINT ii = 0; ii < uLength && ii*2 < uStrLen; ii++)
	{
		pBuffer[ii] = 0;

		for (UINT jj = 0; jj < 2 && ii*2+jj < uStrLen; jj++)
		{
			WCHAR wzBuffer = wszBuffer[ii*2+jj];

			if (!iswxdigit(wzBuffer))
			{
				return false;
			}

			if (iswlower(wzBuffer))
			{
				wzBuffer = towupper(wzBuffer);
			}
				
			pBuffer[ii] <<= 4;

			if (isdigit(wzBuffer))
			{ 
				pBuffer[ii] += (BYTE)wzBuffer - 0x30;
			}
			else
			{
				pBuffer[ii] += (BYTE)wzBuffer - 0x41 + 0x0A;
			}
		}
	}

	return true;
}

// ReadByteString
bool COpcBinaryStream::ReadByteString(
	COpcContext& cContext, 
	BYTE*        pString,
	UINT         uLength
)
{
	memset(pString, 0, uLength);

	// read value from binary buffer.
	if (cContext.Mode == OPC_MODE_BINARY)
	{
		if (cContext.Index + uLength > cContext.BufSize)
		{
			return false;
		}

		memcpy(pString, cContext.Buffer+cContext.Index, uLength);
	}
		
	// read value from hex string.
	else
	{
		COpcString cBuffer;
		
		if (!OpcXml::ReadXml(cContext.Element, cBuffer))
		{
			return false;
		}

		if (!ReadBytes((LPCWSTR)cBuffer, pString, uLength))
		{
			return false;
		}	
	}

	return true;
}

// WriteByteString
bool COpcBinaryStream::WriteByteString(
	COpcContext& cContext, 
	BYTE*        pString,
	UINT         uLength
)
{
	// write value to binary buffer.
	if (cContext.Mode == OPC_MODE_BINARY)
	{
		if (cContext.Buffer != NULL)
		{
			if (cContext.Index + uLength > cContext.BufSize)
			{
				return false;
			}

			memcpy(cContext.Buffer+cContext.Index, pString, uLength);
		}
	}
		
	// write value to hex string.
	else
	{
		LPWSTR szBuffer = OpcArrayAlloc(WCHAR, (uLength+1)*2);

		UINT ii;
		for (ii = 0; ii < uLength; ii++)
		{
			swprintf(szBuffer+ii*2, L"%02X", pString[ii]);
		}

		szBuffer[ii*2] = 0;

		COpcString cBuffer = szBuffer;
		OpcFree(szBuffer);
		
		if (!OpcXml::WriteXml(cContext.Element, cBuffer))
		{
			return false;
		}
	}

	return true;
}

// GetTerminator
bool COpcBinaryStream::GetTerminator(
	COpcContext      cContext, 
	COpcFieldType*   pField,
	OpcXml::AnyType& cTerminator
)
{
	if (!pField->FieldTerminatorSpecified)
	{
		return false;
	}

	OpcXml::AnyType cText(pField->FieldTerminator);

	if (cText.stringValue == NULL)
	{
		return false;
	}

	cTerminator.Clear();

	cTerminator.Alloc(OpcXml::XML_BYTE, wcslen(cText.stringValue)/2);

	if (!ReadBytes(cText.stringValue, cTerminator.pbyteValue, cTerminator.iLength))
	{
		return false;
	}
	
	return true;
}
