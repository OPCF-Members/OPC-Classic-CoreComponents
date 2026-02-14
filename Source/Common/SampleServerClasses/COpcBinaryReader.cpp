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
#include "COpcBinaryReader.h"

// Read
bool COpcBinaryReader::Read(
	BYTE*               pBuffer, 
	UINT                uBufSize,
	COpcTypeDictionary* pDictionary, 
	const COpcString&   cTypeID, 
	OpcXml::AnyType&    cValue
)
{
	OPC_ASSERT(pBuffer != NULL);
	OPC_ASSERT(pDictionary != NULL);

	bool bResult = true;

	COpcContext cContext;
	
	bResult = InitializeContext(pDictionary, cTypeID, cContext);		
	
	if (!bResult)
	{
		return bResult;
	}

	cContext.Mode    = OPC_MODE_BINARY;
	cContext.Buffer  = pBuffer;
	cContext.BufSize = uBufSize;

	UINT uBytesRead = 0;
		
	bResult = ReadType(cContext, cValue, uBytesRead);

	if (!bResult)
	{
		return bResult;
	}
	
	if (uBytesRead == 0)
	{
		return false;
	}

	return true;
}

// Read
bool COpcBinaryReader::Read(
	COpcXmlDocument&    cDocument,
	COpcTypeDictionary* pDictionary, 
	const COpcString&   cTypeID, 
	OpcXml::AnyType&    cValue
)
{
	OPC_ASSERT(cDocument != NULL);
	OPC_ASSERT(pDictionary != NULL);

	bool bResult = true;

	COpcContext cContext;
	
	bResult = InitializeContext(pDictionary, cTypeID, cContext);		
	
	if (!bResult)
	{
		return bResult;
	}

	cContext.Mode    = OPC_MODE_XML;
	cContext.Element = cDocument.GetRoot();

	UINT uBytesRead = 0;
		
	bResult = ReadType(cContext, cValue, uBytesRead);

	if (!bResult)
	{
		return bResult;
	}
	
	if (uBytesRead == 0)
	{
		return false;
	}

	return true;
}

// ReadType
bool COpcBinaryReader::ReadType(
	COpcContext      cContext, 
	OpcXml::AnyType& cValue, 
	UINT&            uBytesRead
)
{
	cValue.Clear();

	bool bResult = true;

	COpcTypeDescription* pType       = cContext.Type;
	BYTE*                pBuffer     = cContext.Buffer;
	UINT                 uStartIndex = cContext.Index;

	cValue.cSchema.SetName(pType->TypeID);
	cValue.Alloc(OpcXml::XML_ANY_TYPE, pType->Fields.GetSize());

	COpcXmlElementList cChildren;
	
	if (cContext.Mode == OPC_MODE_XML)
	{
		if (cContext.Element.GetChildren(cChildren) == 0)
		{
			return false;
		}

		if (cChildren.GetSize() != pType->Fields.GetSize())
		{
			return false;
		}
	}

	// keeps track of the current bit position when reading sequential bit string fields.
	UINT uBitOffset = 0;

	for (UINT ii = 0; ii < pType->Fields.GetSize(); ii++)
	{
		COpcFieldType* pField = pType->Fields[ii];

		cValue.panyTypeValue[ii].cSchema.SetName(pField->Name);
			
		if (cContext.Mode == OPC_MODE_XML)
		{
			cContext.Element = cChildren[ii];
		}

		bool bIsArray = false;

		if (!IsArrayField(pField, bIsArray))
		{
			return false;
		}

		// clear the bit offset and skip to end of byte if the field is not a bit string.
		if (uBitOffset != 0)
		{
			if (pField->Type != OPC_BINARY_BIT_STRING)
			{
				cContext.Index++;
				uBitOffset = 0;
			}
		}

		if (bIsArray)
		{
			bResult = ReadArrayField(cContext, pField, ii, cValue, cValue.panyTypeValue[ii], uBytesRead);
		}
		else
		{
			bResult = ReadField(cContext, pField, ii, cValue, cValue.panyTypeValue[ii], uBytesRead, uBitOffset);
		}

		if (!bResult)
		{
			return bResult;
		}

		if (uBytesRead == 0 && uBitOffset == 0)
		{
			return false; 
		}
		
		cContext.Index += uBytesRead;
	}

	if (uBitOffset != 0)
	{
		cContext.Index++;
	}
			
	uBytesRead = (cContext.Index - uStartIndex);

	return true;
}

// ReadField
bool COpcBinaryReader::ReadField(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	int              iFieldIndex,
	OpcXml::AnyType& cFieldValues, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesRead,
	UINT&            uBitOffset
)
{
	cFieldValue.Clear();

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
			bResult = ReadInteger(cContext, pField, cFieldValue, uBytesRead);
			break;
		}

		case OPC_BINARY_FLOATING_POINT:
		case OPC_BINARY_SINGLE:
		case OPC_BINARY_DOUBLE:
		{
			bResult = ReadFloatingPoint(cContext, pField, cFieldValue, uBytesRead);
			break;
		}
		
		case OPC_BINARY_CHAR_STRING:
		case OPC_BINARY_ASCII:
		case OPC_BINARY_UNICODE:
		{
			bResult = ReadCharString(cContext, pField, iFieldIndex, cFieldValues, cFieldValue, uBytesRead);
			break;
		}

		case OPC_BINARY_TYPE_REFERENCE:
		{
			bResult = ReadTypeReference(cContext, pField, cFieldValue, uBytesRead);
			break;
		}
		
		case OPC_BINARY_BIT_STRING:
		{
			bResult = ReadBitString(cContext, pField, cFieldValue, uBytesRead, uBitOffset);
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

// ReadField
bool COpcBinaryReader::ReadTypeReference(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesRead
)
{
	cFieldValue.Clear();

	bool bResult = true;

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
	
	return ReadType(cContext, cFieldValue, uBytesRead);
}

// ReadInteger
bool COpcBinaryReader::ReadInteger(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesRead
)
{
	cFieldValue.Clear();

	bool bResult = true;

	BYTE* pBuffer = cContext.Buffer;

	// initialize serialization parameters.
	UINT uLength = (pField->LengthSpecified)?pField->Length:4;
	bool bSigned = (!pField->SignedSpecified || (pField->SignedSpecified && pField->Signed));

	// apply defaults for built in types.
	cFieldValue.eType = GetIntegerType(pField, uLength, bSigned);

	// check for a non-standard integer type that requires different handling.
	if (cFieldValue.eType == OpcXml::XML_EMPTY)
	{
		cFieldValue.Alloc(OpcXml::XML_BYTE, uLength);

		// read bytes from stream.
		bResult = ReadByteString(cContext, cFieldValue.pbyteValue, uLength);

		if (!bResult)
		{
			return bResult;
		}

		// convert to little endian.
		if (cContext.BigEndian)
		{
			SwapBytes(cFieldValue.pbyteValue, uLength);
		}

		uBytesRead = uLength;
		return true;
	}

    if (cContext.Mode == OPC_MODE_BINARY)
	{	
		// check if there is enough data left.
		if (cContext.BufSize - cContext.Index < uLength)
		{
			return false;
		}

		// read data binary buffer.
		BYTE* pBytes = &cFieldValue.byteValue;
		memcpy(pBytes, pBuffer+cContext.Index, uLength);
		
		// swap bytes if required.
		if (cContext.BigEndian)
		{
			SwapBytes(pBytes, uLength);
		}
	}
	else
	{
		// read data from contents of XML element.
		if (bSigned)
		{
			switch (uLength)
			{
				case 1:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.sbyteValue)) return false; break; }
				case 2:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.shortValue)) return false; break; }
				case 4:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.intValue))   return false; break; }
				case 8:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.longValue))  return false; break; }
				default: { OPC_ASSERT(false); break; }
			}
		}
		else
		{
			switch (uLength)
			{
				case 1:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.byteValue))   return false; break; }
				case 2:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.ushortValue)) return false; break; }
				case 4:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.uintValue))   return false; break; }
				case 8:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.ulongValue))  return false; break; }
				default: { OPC_ASSERT(false); break; }
			}
		}
	}

	uBytesRead = uLength;
	return true;
}

// ReadFloatingPoint
bool COpcBinaryReader::ReadFloatingPoint(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesRead
)
{
	cFieldValue.Clear();

	bool bResult = true;

	BYTE* pBuffer = cContext.Buffer;

	// initialize serialization parameters.
	UINT       uLength = (pField->LengthSpecified)?pField->Length:4;
	COpcString cFormat = (pField->FloatFormatSpecified)?pField->FloatFormat:cContext.FloatFormat;

	// apply defaults for built in types.
	cFieldValue.eType = GetFloatingPointType(pField, uLength, cFormat);

	// check for a non-standard floating point type that requires different handling.
	if (cFieldValue.eType == OpcXml::XML_EMPTY)
	{
		cFieldValue.Alloc(OpcXml::XML_BYTE, uLength);

		// read bytes from stream.
		bResult = ReadByteString(cContext, cFieldValue.pbyteValue, uLength);

		if (!bResult)
		{
			return bResult;
		}

		uBytesRead = uLength;
		return true;
	}

    if (cContext.Mode == OPC_MODE_BINARY)
	{	
		// check if there is enough data left.
		if (cContext.BufSize - cContext.Index < uLength)
		{
			return false;
		}

		// read data binary buffer.
		memcpy(&cFieldValue.byteValue, pBuffer+cContext.Index, uLength);
	}
	else
	{
		// read data from contents of XML element.
		if (cFormat == OPC_FLOAT_FORMAT_IEEE754)
		{
			switch (uLength)
			{
				case 4:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.floatValue))  return false; break; }
				case 8:  { if (!OpcXml::ReadXml(cContext.Element, cFieldValue.doubleValue)) return false; break; }
				default: { OPC_ASSERT(false); break; }
			}
		}
	}

	uBytesRead = uLength;
	return true;
}

// ReadCharString
bool COpcBinaryReader::ReadCharString(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	int              iFieldIndex,
	OpcXml::AnyType& cFieldValues, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesRead
)
{
	cFieldValue.Clear();

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
	
	if (pField->CharCountRefSpecified)
	{
		if (!ReadReference(cContext, pField, iFieldIndex, cFieldValues, pField->CharCountRef, uCharCount))
		{
			return false;
		}
	}

	// find null terminator
	if (uCharCount == -1)
	{
		uCharCount = 0;

		for (UINT ii = cContext.Index; ii < cContext.BufSize-uCharWidth+1; ii+=uCharWidth)
		{
			uCharCount++;

			bool bNull = true;

			for (UINT jj = 0; jj < uCharWidth; jj++)
			{
				if (cContext.Buffer[ii+jj] != 0)
				{
					bNull = false;
					break;
				}
			}

			if (bNull)
			{
				break;
			}
		}
	}

	// handle extremely wide character widths.
	if (uCharWidth > 2)
	{
		cFieldValue.Alloc(OpcXml::XML_BYTE, uCharCount*uCharWidth);

		// read bytes from stream.
		bResult = ReadByteString(cContext, cFieldValue.pbyteValue, uCharCount*uCharWidth);

		if (!bResult)
		{
			return bResult;
		}

		// swap bytes.
		if (cContext.BigEndian)
		{
			for (UINT ii = 0; ii < uCharCount*uCharWidth; ii += uCharWidth)
			{
				SwapBytes(cFieldValue.pbyteValue+ii, uCharWidth);
			}
		}

		uBytesRead = uCharCount*uCharWidth;
		return true;
	}

    if (cContext.Mode == OPC_MODE_BINARY)
	{	
		// check if there is enough data left.
		if (cContext.BufSize - cContext.Index < uCharCount*uCharWidth)
		{
			return false;
		}

		// read data from binary buffer.
		cFieldValue.eType       = OpcXml::XML_STRING;
		cFieldValue.stringValue = NULL;

		if (uCharWidth == 1)
		{
			cFieldValue.stringValue = COpcString::ToUnicode((LPCSTR)(pBuffer+cContext.Index), uCharCount);
		}
		else
		{
			cFieldValue.stringValue = OpcArrayAlloc(WCHAR, uCharCount+1);
			memset(cFieldValue.stringValue, 0, (uCharCount+1)*sizeof(WCHAR));
			wcsncpy(cFieldValue.stringValue, (LPCWSTR)(pBuffer+cContext.Index), uCharCount);

			// swap bytes.
			if (cContext.BigEndian)
			{
				for (UINT ii = 0; ii < uCharCount*uCharWidth; ii += 2)
				{
					SwapBytes(((BYTE*)cFieldValue.stringValue)+ii, 2);
				}
			}
		}
	}
	else
	{
		cFieldValue.Alloc(OpcXml::XML_BYTE, uCharCount*uCharWidth);

		// read bytes from stream.
		bResult = ReadByteString(cContext, cFieldValue.pbyteValue, uCharCount*uCharWidth);

		if (!bResult)
		{
			return bResult;
		}

		uBytesRead = uCharCount*uCharWidth;
	}

	uBytesRead = uCharCount*uCharWidth;
	return true;
}

// ReadBitString
bool COpcBinaryReader::ReadBitString(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesRead,
	UINT&            uBitOffset
)
{
	cFieldValue.Clear();

	bool bResult = true;

	BYTE* pBuffer = cContext.Buffer;

	// initialize serialization parameters.
	UINT uBits   = (pField->LengthSpecified)?pField->Length:8;
	UINT uLength = (uBits%8 == 0)?uBits/8:uBits/8+1; 

    if (cContext.Mode == OPC_MODE_BINARY)
	{	
		// check if there is enough data left.
		if (cContext.BufSize - cContext.Index < uLength)
		{
			return false;
		}

		// allocate array.
		cFieldValue.Alloc(OpcXml::XML_BYTE, uLength);

		UINT uBitsLeft = uBits;

		// The bit mask used to reconstruct a field that overlaps the byte boundary.
		BYTE uMask = ~((1<<uBitOffset)-1);

		// loop until all bits read.
		for (UINT ii = 0; uBitsLeft >= 0 && ii < uLength; ii++)
		{
			// add the bits from the lower byte.
			cFieldValue.pbyteValue[ii] = ((uMask & pBuffer[cContext.Index+ii])>>uBitOffset);

			// check if no more bits need to be read.
			if (uBitsLeft + uBitOffset <= 8)
			{
				// mask out un-needed bits.
				cFieldValue.pbyteValue[ii] &= ((1<<uBitsLeft)-1);
				break;
			}

			// check if possible to read the next byte.
			if (cContext.Index + ii + 1 >= cContext.BufSize)
			{
				return false;
			}

			// add the bytes from the higher byte.
			cFieldValue.pbyteValue[ii] += ((~uMask & pBuffer[cContext.Index+ii+1])<<(8-uBitOffset));

			// check if all done.
			if (uBitsLeft <= 8)
			{
				// mask out un-needed bits.
				cFieldValue.pbyteValue[ii] &= ((1<<uBitsLeft)-1);
				break;
			}

			// decrement the bit count.
			uBitsLeft -= 8;
		}

		// update the bytes read and bit offset.
		uBytesRead = (uBits + uBitOffset)/8;
		uBitOffset = (uBits + uBitOffset)%8;
	}
	else
	{
		cFieldValue.Alloc(OpcXml::XML_BYTE, uLength);

		// read bytes from stream.
		bResult = ReadByteString(cContext, cFieldValue.pbyteValue, uLength);

		if (!bResult)
		{
			return bResult;
		}

		// update the bytes read and bit offset.
		uBytesRead = uLength;
		uBitOffset = 0;
	}

	return true;
}

// ReadArrayField
bool COpcBinaryReader::ReadArrayField(
	COpcContext      cContext, 
	COpcFieldType*   pField, 
	int              iFieldIndex,
	OpcXml::AnyType& cFieldValues, 
	OpcXml::AnyType& cFieldValue,
	UINT&            uBytesRead
)
{
	cFieldValue.Clear();

	bool bResult = true;

	UINT uStartIndex = cContext.Index;

	OpcXml::AnyType cElements;
	
	UINT uBitOffset = 0;

	// read array from XML document.
	if (cContext.Mode == OPC_MODE_XML)
	{
		COpcXmlElementList cChildren;

		if (cContext.Element.GetChildren(cChildren) > 0)
		{
			cElements.Alloc(OpcXml::XML_ANY_TYPE, cChildren.GetSize());

			for (UINT ii = 0; ii < cChildren.GetSize(); ii++)
			{
				cContext.Element = cChildren[ii];

				bResult = ReadField(
					cContext, 
					pField, 
					iFieldIndex, 
					cFieldValues, 
					cElements.panyTypeValue[ii], 
					uBytesRead, 
					uBitOffset
				);

				if (!bResult || (uBytesRead == 0 && uBitOffset == 0))
				{
					break;
				}

				cContext.Index += uBytesRead;
			}
		}
	}

	// read fixed length array.
	else if (pField->ElementCountSpecified)
	{
		cElements.Alloc(OpcXml::XML_ANY_TYPE, pField->ElementCount);
		
		for (int ii = 0; ii < cElements.iLength; ii++)
		{
			bResult = ReadField(
				cContext, 
				pField, 
				iFieldIndex, 
				cFieldValues, 
				cElements.panyTypeValue[ii], 
				uBytesRead, 
				uBitOffset
			);

			if (!bResult || (uBytesRead == 0 && uBitOffset == 0))
			{
				break;
			}

			cContext.Index += uBytesRead;
		}
	}

	// read variable length array.
	else if (pField->ElementCountRefSpecified)
	{
		UINT uCount = 0;

		bResult = ReadReference(cContext, pField, iFieldIndex, cFieldValues, pField->ElementCountRef, uCount);

		if (bResult && uCount > 0)
		{
			cElements.Alloc(OpcXml::XML_ANY_TYPE, uCount);

			for (int ii = 0; ii < cElements.iLength; ii++)
			{
				bResult = ReadField(
					cContext, 
					pField, 
					iFieldIndex, 
					cFieldValues, 
					cElements.panyTypeValue[ii], 
					uBytesRead, 
					uBitOffset
				);

				if (!bResult || (uBytesRead == 0 && uBitOffset == 0))
				{
					break;
				}

				cContext.Index += uBytesRead;
			}
		}
	}

	// read terminated array.
	else if (pField->FieldTerminatorSpecified)
	{
		// get the terminator.
		OpcXml::AnyType cTerminator;
		bResult = GetTerminator(cContext, pField, cTerminator);

		if (!bResult || cTerminator.eType != OpcXml::XML_BYTE || cTerminator.iLength < 0)
		{
			return false;
		}

		COpcList<OpcXml::AnyType*> cList;

		while (cContext.Index < cContext.BufSize)
		{
			// check for terminator.
			if (memcmp(cTerminator.pbyteValue, cContext.Buffer+cContext.Index, cTerminator.iLength) == 0)
			{
				cContext.Index += cTerminator.iLength;
				break;
			}

			// read the next field.
			OpcXml::AnyType* pElement = new OpcXml::AnyType();
			
			bResult = ReadField(cContext, pField, iFieldIndex, cFieldValues, *pElement, uBytesRead, uBitOffset);

			if (!bResult || (uBytesRead == 0 && uBitOffset == 0))
			{
				delete pElement;
				break;
			}
		
			cList.AddTail(pElement);
			
			cContext.Index += uBytesRead;
		}

		if (!bResult)
		{
			while (cList.GetCount() > 0) delete cList.RemoveHead();
		}

		if (cList.GetCount() > 0)
		{
			cElements.Alloc(OpcXml::XML_ANY_TYPE, cList.GetCount());

			OPC_POS pos = cList.GetHeadPosition();

			for (UINT ii = 0; pos != NULL; ii++)
			{
				OpcXml::AnyType* pElement = cList.GetNext(pos);
				pElement->MoveTo(cElements.panyTypeValue[ii]);
				delete pElement;
			}

			cList.RemoveAll();
		}
	}

	// check for failure.
	if (!bResult)
	{
		return bResult;
	}

	// clear the bit offset and skip to end of byte at the end of the array.
	if (uBitOffset != 0)
	{
		cContext.Index++;
	}

	// determine if the array contains only values of a single type.
	OpcXml::Type eType = OpcXml::XML_EMPTY;

	for (int ii = 0; ii < cElements.iLength; ii++)
	{
		if (eType == OpcXml::XML_EMPTY)
		{
			eType = cElements.panyTypeValue[ii].eType;

			// check for arrays.
			if (cElements.panyTypeValue[ii].iLength >= 0)
			{
				eType = OpcXml::XML_ANY_TYPE;
				break;
			} 
		}
		else
		{
			if (eType != cElements.panyTypeValue[ii].eType || cElements.panyTypeValue[ii].iLength >= 0)
			{
				eType = OpcXml::XML_ANY_TYPE;
				break;
			}
		}
	}

	// convert array list to a fixed length array of a single type. 
	cFieldValue.Alloc(eType, cElements.iLength);

	for (int ii = 0; ii < cElements.iLength; ii++)
	{
		switch (eType)
		{
			case OpcXml::XML_SBYTE:    { cFieldValue.psbyteValue[ii]    = cElements.panyTypeValue[ii].sbyteValue;    break; }
			case OpcXml::XML_BYTE:     { cFieldValue.pbyteValue[ii]     = cElements.panyTypeValue[ii].byteValue;     break; }
			case OpcXml::XML_SHORT:    { cFieldValue.pshortValue[ii]    = cElements.panyTypeValue[ii].shortValue;    break; }
			case OpcXml::XML_USHORT:   { cFieldValue.pushortValue[ii]   = cElements.panyTypeValue[ii].ushortValue;   break; }
			case OpcXml::XML_INT:      { cFieldValue.pintValue[ii]      = cElements.panyTypeValue[ii].intValue;      break; }
			case OpcXml::XML_UINT:     { cFieldValue.puintValue[ii]     = cElements.panyTypeValue[ii].uintValue;     break; }
			case OpcXml::XML_LONG:     { cFieldValue.plongValue[ii]     = cElements.panyTypeValue[ii].longValue;     break; }
			case OpcXml::XML_ULONG:    { cFieldValue.pulongValue[ii]    = cElements.panyTypeValue[ii].ulongValue;    break; }
			case OpcXml::XML_FLOAT:    { cFieldValue.pfloatValue[ii]    = cElements.panyTypeValue[ii].floatValue;    break; }
			case OpcXml::XML_DOUBLE:   { cFieldValue.pdoubleValue[ii]   = cElements.panyTypeValue[ii].doubleValue;   break; }
			case OpcXml::XML_DECIMAL:  { cFieldValue.pdecimalValue[ii]  = cElements.panyTypeValue[ii].decimalValue;  break; }
			case OpcXml::XML_DATETIME: { cFieldValue.pdateTimeValue[ii] = cElements.panyTypeValue[ii].dateTimeValue; break; }
			case OpcXml::XML_BOOLEAN:  { cFieldValue.pboolValue[ii]     = cElements.panyTypeValue[ii].boolValue;     break; }
			
			case OpcXml::XML_STRING:   
			{ 
				cFieldValue.pstringValue[ii] = cElements.panyTypeValue[ii].stringValue;     
				
				cElements.panyTypeValue[ii].eType       = OpcXml::XML_EMPTY;
				cElements.panyTypeValue[ii].stringValue = NULL;
				break; 
			}

			case OpcXml::XML_ANY_TYPE:  
			{ 
				cElements.panyTypeValue[ii].MoveTo(cFieldValue.panyTypeValue[ii]);
				break;
			}
		}
	}

	// return the total bytes read.
	uBytesRead = (cContext.Index - uStartIndex);

	return true;
}
	
// ReadReference
bool COpcBinaryReader::ReadReference(
	COpcContext       cContext, 
	COpcFieldType*    pField, 
	int               iFieldIndex,
	OpcXml::AnyType&  cFieldValues,
	const COpcString& cFieldName,
	UINT&             uCount
)
{
	OpcXml::AnyType cValue;

	if (cFieldName.IsEmpty())
	{
		if (iFieldIndex > 0 && iFieldIndex-1 < cFieldValues.iLength)
		{
			OpcXml::AnyType& cField = cFieldValues.panyTypeValue[iFieldIndex-1];

			if (cField.CopyTo(cValue, OpcXml::XML_UINT))
			{
				uCount = cValue.uintValue;
				return true;
			}
		}
	}
	else
	{
		for (int ii = 0; ii < cFieldValues.iLength; ii++)
		{
			OpcXml::AnyType& cField = cFieldValues.panyTypeValue[ii];

			if (cField.cSchema.GetName().GetName() == cFieldName)
			{
				if (cField.CopyTo(cValue, OpcXml::XML_UINT))
				{
					uCount = cValue.uintValue;
					return true;
				}

				break;
			}
		}
	}

	return false; 
}
