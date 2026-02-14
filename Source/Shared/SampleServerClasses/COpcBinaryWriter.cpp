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
#include "COpcBinaryWriter.h"

//============================================================================
// COpcBinaryWriter

// Write
bool COpcBinaryWriter::Write(
	OpcXml::AnyType&    cValue, 
	COpcTypeDictionary* pDictionary, 
	const COpcString&   cTypeID,
	BYTE**              ppBuffer,
	UINT*               puBufSize
)
{
	// initialize context object.
	COpcContext cContext;

	bool bResult = InitializeContext(pDictionary, cTypeID, cContext);		

	if (!bResult)
	{
		return bResult;
	}

	// determine the size of pBuffer required.
	cContext.Mode = OPC_MODE_BINARY;

	UINT uBytesRequired = 0;
	
	bResult = WriteType(cContext, cValue, uBytesRequired);
	
	if (!bResult)
	{
		return bResult;
	}

	if (uBytesRequired == 0)
	{
		return false;
	}

	// allocate pBuffer.
	cContext.Buffer  = OpcArrayAlloc(BYTE, uBytesRequired);
	cContext.BufSize = uBytesRequired;

	// initialize memory to zero.
	memset(cContext.Buffer, 0, cContext.BufSize);

	// write data into pBuffer.
	UINT uBytesWritten  = 0;
	
	bResult = WriteType(cContext, cValue, uBytesWritten);
	
	if (!bResult)
	{
		OpcFree(cContext.Buffer);
		return bResult;
	}

	if (uBytesWritten != uBytesRequired)
	{
		OpcFree(cContext.Buffer);
		return false;
	}

	*ppBuffer  = cContext.Buffer;
	*puBufSize = cContext.BufSize;

	return true;
}

// Write
bool COpcBinaryWriter::Write(
	OpcXml::AnyType&    cValue, 
	COpcTypeDictionary* pDictionary, 
	const COpcString&   cTypeID,
	COpcXmlDocument&    cDocument
)
{
	// initialize context object.
	COpcContext cContext;

	bool bResult = InitializeContext(pDictionary, cTypeID, cContext);		

	if (!bResult)
	{
		return bResult;
	}

	// initialize new document.
	if (!cDocument.New(cTypeID, pDictionary->Name))
	{
		return false;
	}

	cContext.Mode    = OPC_MODE_XML; 
	cContext.Element = cDocument.GetRoot();

	// write data into pBuffer.
	UINT uBytesWritten  = 0;
	
	bResult = WriteType(cContext, cValue, uBytesWritten);
	
	if (!bResult)
	{
		return bResult;
	}

	return true;
}

// WriteType
bool COpcBinaryWriter::WriteType(
	COpcContext      cContext, 
	OpcXml::AnyType& cValue, 
	UINT&            uBytesWritten
)
{
	bool bResult = true;

	COpcTypeDescription* pType       = cContext.Type;
	BYTE*                pBuffer     = cContext.Buffer;
	UINT                 uStartIndex = cContext.Index;

	if (cValue.eType != OpcXml::XML_ANY_TYPE || cValue.iLength < 0)
	{
		return false; 
	}

	if (cValue.iLength != pType->Fields.GetSize())
	{
		return false;
	}

	COpcXmlElement cParent = cContext.Element;

	UINT uBitOffset = 0;

	for (UINT ii = 0; ii < pType->Fields.GetSize(); ii++)
	{
		COpcFieldType* pField = pType->Fields[ii];

		if (cContext.Mode == OPC_MODE_XML)
		{
			cContext.Element = cParent.AppendChild(GetFieldName(pField));
		}

		UINT uBytesWritten = 0;			
		
		// clear the bit offset and skip to end of byte if the field is not a bit string.
		if (uBitOffset != 0)
		{
			if (pField->Type != OPC_BINARY_BIT_STRING)
			{
				cContext.Index++;
				uBitOffset = 0;
			}
		}

		bool bIsArray = false;

		if (!IsArrayField(pField, bIsArray))
		{
			return false;
		}

		if (bIsArray)
		{
			bResult = WriteArrayField(cContext, pField, ii, cValue, cValue.panyTypeValue[ii], uBytesWritten);
		}
		else
		{
			bResult = WriteField(cContext, pField, ii, cValue, cValue.panyTypeValue[ii], uBytesWritten, uBitOffset);
		}

		if (!bResult)
		{
			return bResult;
		}

		if (uBytesWritten == 0 && uBitOffset == 0)
		{
			return false; 
		}
		
		cContext.Index += uBytesWritten;
	}		
	
	if (uBitOffset != 0)
	{
		cContext.Index++;
	}
	
	uBytesWritten = (cContext.Index - uStartIndex);
	return true;
}

// WriteField
bool COpcBinaryWriter::WriteField(
	COpcContext      cContext, 
	COpcFieldType*   pField,
	int              iFieldIndex,
	OpcXml::AnyType& cFieldValues, 
	OpcXml::AnyType& cFieldValue, 
	UINT&            uBytesWritten,
	UINT&            uBitOffset
)
{
	bool bResult = true;

	switch (pField->Type)
	{
		case OPC_BINARY_INTEGER:
		case OPC_BINARY_INT8:
		case OPC_BINARY_INT16:
		case OPC_BINARY_INT32:
		case OPC_BINARY_INT64:
		case OPC_BINARY_UINT8:
		case OPC_BINARY_UINT16:
		case OPC_BINARY_UINT32:
		case OPC_BINARY_UINT64:
		{
			bResult = WriteInteger(cContext, pField, cFieldValue, uBytesWritten);
			break;
		}

		case OPC_BINARY_TYPE_REFERENCE:
		{
			bResult = WriteTypeReference(cContext, pField, cFieldValue, uBytesWritten);
			break;
		}

		case OPC_BINARY_FLOATING_POINT:
		case OPC_BINARY_SINGLE:
		case OPC_BINARY_DOUBLE:
		{
			bResult = WriteFloatingPoint(cContext, pField, cFieldValue, uBytesWritten);
			break;
		}

		case OPC_BINARY_CHAR_STRING:
		case OPC_BINARY_ASCII:
		case OPC_BINARY_UNICODE:
		{
			bResult = WriteCharString(cContext, pField, iFieldIndex, cFieldValues, cFieldValue, uBytesWritten);
			break;
		}
		
		case OPC_BINARY_BIT_STRING:
		{
			bResult = WriteBitString(cContext, pField, cFieldValue, uBytesWritten, uBitOffset);
			break;
		}

		default:
		{
			bResult = false;
			break;
		}
	}

	return bResult;
}

// WriteTypeReference
bool COpcBinaryWriter::WriteTypeReference(
	COpcContext      cContext, 
	COpcFieldType*   pField,
	OpcXml::AnyType& cFieldValue, 
	UINT&            uBytesWritten
)
{
	for (UINT ii = 0; ii < cContext.Dictionary->Types.GetSize(); ii++)
	{
		COpcTypeDescription* pType = cContext.Dictionary->Types[ii];

		if (pType->TypeID == pField->TypeID)
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

	if (cFieldValue.eType != OpcXml::XML_ANY_TYPE || cFieldValue.iLength < 0)
	{
		return false; 
	}		

	return WriteType(cContext, cFieldValue, uBytesWritten);
}

// WriteInteger
bool COpcBinaryWriter::WriteInteger(
	COpcContext      cContext, 
	COpcFieldType*   pField,
	OpcXml::AnyType& cFieldValue, 
	UINT&            uBytesWritten
)
{
	bool bResult = true;

	BYTE* pBuffer = cContext.Buffer;

	// initialize serialization parameters.
	UINT         uLength = pField->Length;
	bool         bSigned = (!pField->SignedSpecified || (pField->SignedSpecified && pField->Signed));
	OpcXml::Type eType   = OpcXml::XML_EMPTY;

	// apply defaults for built in types.
	eType = GetIntegerType(pField, uLength, bSigned);

	// check for sufficient memory.
	if (cContext.Buffer != NULL)
	{
		if (cContext.Index + uLength > cContext.BufSize)
		{
			return false;
		}
	}

	// check for a non-standard integer type that requires different handling.
	if (eType == OpcXml::XML_EMPTY)
	{
		// must be stored as an array of bytes in little endian form.
		if (cFieldValue.eType != OpcXml::XML_BYTE || cFieldValue.iLength != uLength)
		{
			return false;
		}

		// write bytes to stream.
		bResult = WriteByteString(cContext, cFieldValue.pbyteValue, uLength);

		if (!bResult)
		{
			return bResult;
		}

		// convert to big endian.
		if (cContext.Buffer != NULL && cContext.BigEndian)
		{
			SwapBytes(cContext.Buffer+cContext.Index, uLength);
		}

		uBytesWritten = uLength;
		return true;
	}

	// convert value to appropriate integer type.
	OpcXml::AnyType cInteger;

	if (!cFieldValue.CopyTo(cInteger, eType))
	{
		return false;
	}

	if (cContext.Mode == OPC_MODE_XML)
	{
		// write data to an XML element.
		if (bSigned)
		{
			switch (uLength)
			{
				case 1:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.sbyteValue)) return false; break; }
				case 2:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.shortValue)) return false; break; }
				case 4:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.intValue))   return false; break; }
				case 8:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.longValue))  return false; break; }
				default: { OPC_ASSERT(false); break; }
			}
		}
		else
		{
			switch (uLength)
			{
				case 1:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.byteValue))   return false; break; }
				case 2:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.ushortValue)) return false; break; }
				case 4:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.uintValue))   return false; break; }
				case 8:  { if (!OpcXml::WriteXml(cContext.Element, cInteger.ulongValue))  return false; break; }
				default: { OPC_ASSERT(false); break; }
			}
		}	
	}
	else
	{
		if (cContext.Buffer != NULL)
		{
			// copy and swap bytes if required.
			memcpy(pBuffer+cContext.Index, &cInteger.byteValue, uLength);
			
			if (cContext.BigEndian)
			{
				SwapBytes(cContext.Buffer+cContext.Index, uLength);
			}
		}
	}
	
	uBytesWritten = uLength;
	return true;
}

// WriteFloatingPoint
bool COpcBinaryWriter::WriteFloatingPoint(
	COpcContext      cContext, 
	COpcFieldType*   pField,
	OpcXml::AnyType& cFieldValue, 
	UINT&            uBytesWritten
)
{
	bool bResult = true;

	BYTE* pBuffer = cContext.Buffer;

	// initialize serialization parameters.
	UINT         uLength = pField->Length;
	COpcString   cFormat = (pField->FloatFormatSpecified)?pField->FloatFormat:cContext.FloatFormat;
	OpcXml::Type eType   = OpcXml::XML_EMPTY;

	// apply defaults for built in types.
	eType = GetFloatingPointType(pField, uLength, cFormat);

	// check for sufficient memory.
	if (cContext.Buffer != NULL)
	{
		if (cContext.Index + uLength > cContext.BufSize)
		{
			return false;
		}
	}

	// check for a non-standard integer type that requires different handling.
	if (eType == OpcXml::XML_EMPTY)
	{
		// must be stored as an array of bytes in little endian form.
		if (cFieldValue.eType != OpcXml::XML_BYTE || cFieldValue.iLength != uLength)
		{
			return false;
		}

		// write bytes to stream.
		bResult = WriteByteString(cContext, cFieldValue.pbyteValue, uLength);

		if (!bResult)
		{
			return bResult;
		}

		uBytesWritten = uLength;
		return true;
	}

	// convert value to appropriate floating point type.
	OpcXml::AnyType cFloatingPoint;

	if (!cFieldValue.CopyTo(cFloatingPoint, eType))
	{
		return false;
	}

	if (cContext.Mode == OPC_MODE_XML)
	{
		// write data to an XML element.
		if (cFormat == OPC_FLOAT_FORMAT_IEEE754)
		{
			switch (uLength)
			{
				case 4:  { if (!OpcXml::WriteXml(cContext.Element, cFloatingPoint.floatValue))  return false; break; }
				case 8:  { if (!OpcXml::WriteXml(cContext.Element, cFloatingPoint.doubleValue)) return false; break; }
				default: { OPC_ASSERT(false); break; }
			}
		}
	}
	else
	{
		if (cContext.Buffer != NULL)
		{
			memcpy(pBuffer+cContext.Index, &cFloatingPoint.byteValue, uLength);
		}
	}
	
	uBytesWritten = uLength;
	return true;
}

// WriteCharString
bool COpcBinaryWriter::WriteCharString(
	COpcContext      cContext, 
	COpcFieldType*   pField,
	int              iFieldIndex,
	OpcXml::AnyType& cFieldValues, 
	OpcXml::AnyType& cFieldValue, 
	UINT&            uBytesWritten
)
{
	bool bResult = true;

	BYTE* pBuffer = cContext.Buffer;

	// initialize serialization parameters.
	UINT uCharWidth = (pField->CharWidthSpecified)?pField->CharWidth:cContext.CharWidth;
	UINT uCharCount = (pField->LengthSpecified)?pField->Length:-1;

	switch (pField->Type)
	{
		case OPC_BINARY_ASCII:   { uCharWidth = 1; break; }
		case OPC_BINARY_UNICODE: { uCharWidth = 2; break; }
	}
	
	if (uCharCount == -1)
	{
		if (uCharWidth > 2)
		{
			if (cFieldValue.eType != OpcXml::XML_BYTE || cFieldValue.iLength < 0)
			{
				return false;
			}

			uCharCount = cFieldValue.iLength/uCharWidth;
		}
		else
		{
			if (cFieldValue.eType != OpcXml::XML_STRING)
			{
				return false;
			}

			uCharCount = 1;

			if (cFieldValue.stringValue != NULL)
			{
				uCharCount += wcslen(cFieldValue.stringValue);
			}
		}
	}

	// update the char count reference.
	if (pField->CharCountRefSpecified)
	{
		if (!WriteReference(cContext, pField, iFieldIndex, cFieldValues, pField->CharCountRef, uCharCount))
		{
			return false;
		}
	}

	// check for sufficient memory.
	if (cContext.Buffer != NULL)
	{
		if (cContext.Index + uCharCount*uCharWidth > cContext.BufSize)
		{
			return false;
		}
	}	
	
	// check for a non-standard integer type that requires different handling.
	if (uCharWidth > 2)
	{
		// must be stored as an array of bytes in little endian form.
		if (cFieldValue.eType != OpcXml::XML_BYTE || cFieldValue.iLength != uCharCount*uCharWidth)
		{
			return false;
		}

		// write bytes to stream.
		bResult = WriteByteString(cContext, cFieldValue.pbyteValue, uCharCount*uCharWidth);

		if (!bResult)
		{
			return bResult;
		}
			
		// swap bytes.
		if (pBuffer != NULL)
		{
			if (cContext.BigEndian)
			{
				for (UINT ii = 0; ii < uCharCount*uCharWidth; ii += uCharWidth)
				{
					SwapBytes(pBuffer+cContext.Index+ii, uCharWidth);
				}
			}
		}

		uBytesWritten = uCharCount*uCharWidth;
		return true;
	}

	// write the field to the buffer.
	if (cContext.Mode == OPC_MODE_XML)
	{
		if (!OpcXml::WriteXml(cContext.Element, cFieldValue.stringValue))  
		{
			return false;
		}
	}
	else
	{
		if (cContext.Buffer != NULL)
		{
			if (uCharWidth == 1)
			{
				LPSTR szBuffer = COpcString::ToMultiByte(cFieldValue.stringValue);

				if (szBuffer != NULL)
				{
					strncpy((LPSTR)(pBuffer+cContext.Index), szBuffer, uCharCount);
					OpcFree(szBuffer);
				}
			}
			else
			{
				if (cFieldValue.stringValue != NULL)
				{
					wcsncpy((LPWSTR)(pBuffer+cContext.Index), cFieldValue.stringValue, uCharCount);
				}

				if (cContext.BigEndian)
				{
					for (UINT ii = 0; ii < uCharCount*uCharWidth; ii += uCharWidth)
					{
						SwapBytes(pBuffer+cContext.Index+ii, uCharWidth);
					}
				}
			}
		}
	}
	
	uBytesWritten = uCharCount*uCharWidth;
	return true;
}

// WriteBitString
bool COpcBinaryWriter::WriteBitString(
	COpcContext      cContext, 
	COpcFieldType*   pField,
	OpcXml::AnyType& cFieldValue, 
	UINT&            uBytesWritten,
	UINT&            uBitOffset
)
{
	bool bResult = true;

	BYTE* pBuffer = cContext.Buffer;

	// initialize serialization parameters.
	UINT uBits   = (pField->LengthSpecified)?pField->Length:8;
	UINT uLength = (uBits%8 == 0)?uBits/8:uBits/8+1; 

	// must be stored as an array of bytes in little endian form.
	if (cFieldValue.eType != OpcXml::XML_BYTE || cFieldValue.iLength != uLength)
	{
		return false;
	}

	if (cContext.Mode == OPC_MODE_BINARY)
	{
		if (pBuffer != NULL)
		{
			UINT uBitsLeft = uBits;

			// The bit mask used to reconstruct a field that overlaps the byte boundary.
			BYTE uMask = (uBitOffset == 0)?0xFF:((0x80>>(uBitOffset-1))-1);

			// loop until all bits read.
			for (UINT ii = 0; uBitsLeft >= 0 && ii < uLength; ii++)
			{
				// add the bits from the lower byte.
				pBuffer[cContext.Index+ii] += ((uMask & ((1<<uBitsLeft)-1) & cFieldValue.pbyteValue[ii])<<uBitOffset);

				// check if no more bits need to be read.
				if (uBitsLeft + uBitOffset <= 8)
				{
					break;
				}

				// check if possible to read the next byte.
				if (cContext.Index + ii + 1 >= cContext.BufSize)
				{
					return false;
				}

				// add the bytes from the higher byte.
				pBuffer[cContext.Index+ii+1] += ((~uMask & ((1<<uBitsLeft)-1) & cFieldValue.pbyteValue[ii])>>(8-uBitOffset));

				// check if all done.
				if (uBitsLeft <= 8)
				{
					break;
				}

				// decrement the bit count.
				uBitsLeft -= 8;
			}
		}

		// update the bytes written and bit offset.
		uBytesWritten = (uBitOffset + uBits)/8;
		uBitOffset    = (uBitOffset + uBits)%8;
	}
	else
	{
		// write bytes to stream.
		bResult = WriteByteString(cContext, cFieldValue.pbyteValue, uLength);

		if (!bResult)
		{
			return bResult;
		}

		uBitOffset    = 0;
		uBytesWritten = uLength;
	}

	return true;
}

// WriteArrayField
bool COpcBinaryWriter::WriteArrayField(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	UINT             uFieldIndex,
	OpcXml::AnyType& cFieldValues, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesWritten
)
{
	bool bResult = true;

	UINT uStartIndex = cContext.Index;

	if (cFieldValue.iLength < 0)
	{
		return false;
	}

	UINT uBitOffset = 0;

	// write array to XML document.
	if (cContext.Mode == OPC_MODE_XML)
	{	
		COpcXmlElement cParent = cContext.Element;

		for (int ii = 0; ii < cFieldValue.iLength; ii++)
		{
			OpcXml::AnyType cElementValue;
			cFieldValue.GetElement(ii, cElementValue);

			cContext.Element = cParent.AppendChild(GetFieldType(pField));

			bResult = WriteField(cContext, pField, uFieldIndex, cFieldValues, cElementValue, uBytesWritten, uBitOffset);

			if (!bResult ||(uBytesWritten == 0 && uBitOffset == 0))
			{
				break;
			}

			cContext.Index += uBytesWritten;
		}
	}

	// read fixed length array.
	else if (pField->ElementCountSpecified)
	{
		UINT uCount = 0;

		for (int ii = 0; ii < cFieldValue.iLength; ii++)
		{
			// ignore any excess elements.
			if (uCount == pField->ElementCount)
			{
				break;
			}

			OpcXml::AnyType cElementValue;
			cFieldValue.GetElement(ii, cElementValue);
		
			bResult = WriteField(cContext, pField, uFieldIndex, cFieldValues, cElementValue, uBytesWritten, uBitOffset);

			if (!bResult || (uBytesWritten == 0 && uBitOffset == 0))
			{
				break;
			}

			cContext.Index += uBytesWritten;
			uCount++;
		}

		// write a NULL value for any missing elements.
		while (uCount < pField->ElementCount)
		{
			return false;
		}
	}

	// read variable length array.
	else if (pField->ElementCountRefSpecified)
	{
		UINT uCount = 0;

		for (int ii = 0; ii < cFieldValue.iLength; ii++)
		{
			OpcXml::AnyType cElementValue;
			cFieldValue.GetElement(ii, cElementValue);

			bResult = WriteField(cContext, pField, uFieldIndex, cFieldValues, cElementValue, uBytesWritten, uBitOffset);

			if (!bResult || (uBytesWritten == 0 && uBitOffset == 0))
			{
				break;
			}

			cContext.Index += uBytesWritten;
			uCount++;
		}

		// update the value of the referenced pField with the correct element uCount.
		WriteReference(cContext, pField, uFieldIndex, cFieldValues, pField->ElementCountRef, uCount);
	}

	// read terminated array.
	else if (pField->FieldTerminatorSpecified)
	{
		UINT uCount = 0;

		for (int ii = 0; ii < cFieldValue.iLength; ii++)
		{
			OpcXml::AnyType cElementValue;
			cFieldValue.GetElement(ii, cElementValue);

			bResult = WriteField(cContext, pField, uFieldIndex, cFieldValues, cElementValue, uBytesWritten, uBitOffset);

			if (!bResult || (uBytesWritten == 0 && uBitOffset == 0))
			{
				return false;
			}

			cContext.Index += uBytesWritten;
			uCount++;
		}

		// get the terminator.
		OpcXml::AnyType cTerminator;
		bResult = GetTerminator(cContext, pField, cTerminator);

		if (!bResult || cTerminator.eType != OpcXml::XML_BYTE || cTerminator.iLength < 0)
		{
			return false;
		}

		// write the terminator.
		if (cContext.Buffer != NULL)
		{
			if (cContext.Index + cTerminator.iLength > cContext.BufSize)
			{
				return false;
			}

			memcpy(cContext.Buffer+cContext.Index, cTerminator.pbyteValue, cTerminator.iLength);
		}

		cContext.Index += cTerminator.iLength;
	}

	// clear the bit offset and skip to end of byte at the end of the array.
	if (uBitOffset != 0)
	{
		cContext.Index++;
	}

	// return the total bytes written.
	uBytesWritten = (cContext.Index - uStartIndex);

	return true;
}

// WriteReference
bool COpcBinaryWriter::WriteReference(
	COpcContext       cContext,
	COpcFieldType*    pField, 
	UINT              uFieldIndex,
	OpcXml::AnyType&  cFieldValues,
	const COpcString& cFieldName, 
	UINT              uCount
)
{
	if (cFieldName.IsEmpty())
	{
		uFieldIndex = uFieldIndex-1;

		if (uFieldIndex >= (UINT)cFieldValues.iLength-1)
		{
			return false;
		}
	}
	else
	{
		uFieldIndex = -1;

		for (int ii = 0; ii < cFieldValues.iLength; ii++)
		{
			if (cFieldName == cFieldValues.panyTypeValue[ii].cSchema.GetName().GetName())
			{
				uFieldIndex = ii;
				break;
			}
		}

		if (uFieldIndex == -1)
		{
			return false;
		}
	}

	if (cContext.Buffer == NULL)
	{
		switch (cFieldValues.panyTypeValue[uFieldIndex].eType)
		{
			default:                 { return false; }
			case OpcXml::XML_SBYTE:  { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::SByte)uCount);  break; }
			case OpcXml::XML_SHORT:  { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::Short)uCount);  break; }
			case OpcXml::XML_INT:    { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::Int)uCount);    break; }
			case OpcXml::XML_LONG:   { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::Long)uCount);   break; }
			case OpcXml::XML_BYTE:   { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::Byte)uCount);   break; }
			case OpcXml::XML_USHORT: { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::UShort)uCount); break; }
			case OpcXml::XML_UINT:   { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::UInt)uCount);   break; }
			case OpcXml::XML_ULONG:  { cFieldValues.panyTypeValue[uFieldIndex].Set((OpcXml::ULong)uCount);  break; }
		}
			
	}

	return true;
}
