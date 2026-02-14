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
#include "COpcDaDeviceItem.h"
#include "COpcDaProperty.h"
#include "COpcDaCache.h"
#include "COpcBinaryReader.h"
#include "COpcBinaryWriter.h"

#include <float.h>
#include <limits.h>
#include <math.h>

//============================================================================
// Local Declarations

#define TAG_XML           _T("XML")
#define TAG_NAME          _T("Name")
#define TAG_SEPARATOR     _T("/")
#define TAG_PROPERTY_ID   _T("PropertyID")
#define TAG_PERIOD        _T("Period")
#define TAG_SAMPLING_RATE _T("SamplingRate")
#define TAG_MAX_VALUE     _T("MaxValue")
#define TAG_MIN_VALUE     _T("MinValue")
#define TAG_WAVEFORM      _T("Waveform")
#define TAG_DICTIONARY    _T("Dictionary")

// numeric upper/lower bounds.
#define MIN_SBYTE   _I8_MIN
#define MAX_SBYTE   _I8_MAX
#define MAX_BYTE    _UI8_MAX
#define MIN_SHORT   _I16_MIN
#define MAX_SHORT   _I16_MAX
#define MAX_USHORT  _UI16_MAX
#define MIN_INT     _I32_MIN
#define MAX_INT     _I32_MAX
#define MAX_UINT    _UI32_MAX   
#define MIN_LONG    _I64_MIN
#define MAX_LONG    _I64_MAX
#define MAX_ULONG   _UI64_MAX 
#define MIN_FLOAT   FLT_MIN
#define MAX_FLOAT   FLT_MAX    
#define MIN_DOUBLE  DBL_MIN
#define MAX_DOUBLE  DBL_MAX    
#define MIN_DECIMAL (_I64_MIN/10000)
#define MAX_DECIMAL (_I64_MAX/10000)

#define PI   3.14159265358979
#define QNAN _FPCLASS_QNAN

// convenience macros.
#define SET_MAX(xParams, xMax) if (xMax == QNAN || xParams.dblMaxValue == QNAN || xParams.dblMaxValue > xMax) xParams.dblMaxValue = xMax;
#define SET_MIN(xParams, xMin) if (xMin == QNAN || xParams.dblMinValue == QNAN || xParams.dblMinValue < xMin) xParams.dblMinValue = xMin;

// possible waveform types.
#define NONE 1
#define RAMP 1
#define SINE 2
#define ENUM 3

//============================================================================
// COpcDaDeviceItem

// GetAvailableProperties
HRESULT COpcDaDeviceItem::GetAvailableProperties(
	const COpcString&     cItemPath,
	bool                  bReturnValues,
	COpcDaPropertyList&   cProperties
)
{
	// find complex item referenced by the item path.
	COpcDaComplexItem* pItem = FindItem(cItemPath);

	if (pItem == NULL && m_pNativeFormat != NULL)
	{
		return OPC_E_UNKNOWNITEMID;
	}
	
	COpcList<DWORD> cIDs;

	// add standard properties.
	cIDs.AddTail(OPC_PROPERTY_DATATYPE);
	cIDs.AddTail(OPC_PROPERTY_VALUE);
	cIDs.AddTail(OPC_PROPERTY_QUALITY);
	cIDs.AddTail(OPC_PROPERTY_TIMESTAMP);
	cIDs.AddTail(OPC_PROPERTY_ACCESS_RIGHTS);
	cIDs.AddTail(OPC_PROPERTY_SCAN_RATE);
	cIDs.AddTail(OPC_PROPERTY_EU_TYPE);
	cIDs.AddTail(OPC_PROPERTY_EU_INFO);

	// add eu limits for analog items.
	if (m_eEUType == OPC_ANALOG)
	{
		cIDs.AddTail(OPC_PROPERTY_HIGH_EU);
		cIDs.AddTail(OPC_PROPERTY_LOW_EU);
	}

	// add complex item properties.
	if (pItem != NULL)
	{
		if (!IsFilterItem(cItemPath))
		{			
			cIDs.AddTail(OPC_PROPERTY_TYPE_SYSTEM_ID);
			cIDs.AddTail(OPC_PROPERTY_DICTIONARY_ID);
			cIDs.AddTail(OPC_PROPERTY_TYPE_ID);
			cIDs.AddTail(OPC_PROPERTY_CONSISTENCY_WINDOW);
			cIDs.AddTail(OPC_PROPERTY_WRITE_BEHAVIOR);

			if (!pItem->TypeName.IsEmpty())
			{
				cIDs.AddTail(OPC_PROPERTY_UNCONVERTED_ITEM_ID);
			}
		}

		else 
		{
			COpcString cFilterName = GetFilterName(cItemPath);

			if (!cFilterName.IsEmpty())
			{
				cIDs.AddTail(OPC_PROPERTY_TYPE_SYSTEM_ID);
				cIDs.AddTail(OPC_PROPERTY_DICTIONARY_ID);
				cIDs.AddTail(OPC_PROPERTY_TYPE_ID);
			}

			if (!pItem->TypeName.IsEmpty())
			{
				cIDs.AddTail(OPC_PROPERTY_UNCONVERTED_ITEM_ID);
			}

			cIDs.AddTail(OPC_PROPERTY_UNFILTERED_ITEM_ID);

			if (!cFilterName.IsEmpty())
			{
				cIDs.AddTail(OPC_PROPERTY_DATA_FILTER_VALUE);
			}
		}
	}

	// add any additional properties.
    OPC_POS pos = m_cProperties.GetStartPosition(); 

    while (pos != NULL)
    {  
		DWORD dwPropertyID = 0;
        m_cProperties.GetNextAssoc(pos, dwPropertyID);
		cIDs.AddTail(dwPropertyID);
    }

	// fill in the property item ids and values.
	return GetAvailableProperties(cItemPath, cIDs, bReturnValues, cProperties);
}

// GetAvailableProperties
HRESULT COpcDaDeviceItem::GetAvailableProperties(
	const COpcString&      cItemPath,
	const COpcList<DWORD>& cIDs,
	bool                   bReturnValues,
	COpcDaPropertyList&    cProperties
)
{
	// find complex item referenced by the item path.
	COpcDaComplexItem* pItem = FindItem(cItemPath);

	if (pItem == NULL && m_pNativeFormat != NULL)
	{
		return OPC_E_UNKNOWNITEMID;
	}
	
	COpcDaProperty::Create(cIDs, cProperties);

	for (UINT ii = 0; ii < cProperties.GetSize(); ii++)
	{
		HRESULT hResult = S_OK;
	
		DWORD dwID = cProperties[ii]->GetID();

		if (bReturnValues)
		{
			hResult = Read(cItemPath, dwID, cProperties[ii]->GetValue());
		}
		else
		{
			hResult = ValidatePropertyID(pItem, cItemPath, dwID, OPC_READABLE | OPC_WRITEABLE);

			// don't care about access rights when not reading the value.
			if (hResult == OPC_E_BADRIGHTS)
			{
				hResult = S_OK;
			}
		}

		cProperties[ii]->SetError(hResult);

		if (SUCCEEDED(hResult))
		{
			if (dwID >= OPC_PROPERTY_EU_UNITS && dwID <= OPC_PROPERTY_TIMEZONE)
			{
				cProperties[ii]->SetItemID(OpcConstructItemID(m_cItemID, dwID));
			}
			else if (dwID == OPC_PROPERTY_DICTIONARY_ID)
			{
				cProperties[ii]->SetItemID(pItem->DictionaryItemID);
			}
			else if (dwID == OPC_PROPERTY_TYPE_ID)
			{
				cProperties[ii]->SetItemID(pItem->TypeItemID);
			}
		}
	}

	return S_OK;
}

// Read
HRESULT COpcDaDeviceItem::Read(
	const COpcString&     cItemPath,
    DWORD                 dwPropertyID,
    VARIANT&              cValue, 
    FILETIME*             pftTimestamp,
    WORD*                 pwQuality
)
{
	// find complex item referenced by the item path.
	COpcDaComplexItem* pItem = FindItem(cItemPath);

	if (pItem == NULL && m_pNativeFormat != NULL)
	{
		return OPC_E_UNKNOWNITEMID;
	}
	
	// validate property.
	HRESULT hResult = ValidatePropertyID(pItem, cItemPath, dwPropertyID, OPC_READABLE);

	if (FAILED(hResult))
	{
		return hResult;
	}

	// set default quality and timestamp (overridden when returning the item value).
    if (pftTimestamp != NULL) *pftTimestamp = OpcUtcNow();
    if (pwQuality != NULL)    *pwQuality    = OPC_QUALITY_GOOD;

	// read the property value.
	switch (dwPropertyID)
	{	
		case NULL:
		case OPC_PROPERTY_VALUE:
		{
			return Read(pItem, cItemPath, cValue, pftTimestamp, pwQuality);
		}

		case OPC_PROPERTY_DATATYPE:              
		{ 
			if (cItemPath.IsEmpty())
			{
				OpcWriteVariant(cValue, (short)m_vtDataType);   
			}
			else
			{
				OpcWriteVariant(cValue, (short)VT_BSTR); 
			}
				
			break;
		}

		// standard properties.
		case OPC_PROPERTY_QUALITY:       
		{ 	
			COpcString cFilterName = GetFilterName(cItemPath);

			// no reads from the default data filter item allowed.
			if (IsFilterItem(cItemPath) && cFilterName.IsEmpty())
			{
				return OPC_E_BADRIGHTS;
			}

			OpcWriteVariant(cValue, (short)m_wQuality);
			break; 
		}	
		
		case OPC_PROPERTY_TIMESTAMP:    
		{ 
			COpcString cFilterName = GetFilterName(cItemPath);

			// no reads from the default data filter item allowed.
			if (IsFilterItem(cItemPath) && cFilterName.IsEmpty())
			{
				return OPC_E_BADRIGHTS;
			}

			OpcWriteVariant(cValue, m_ftTimestamp);     
			break; 
		}

		case OPC_PROPERTY_ACCESS_RIGHTS:
		{					
			COpcString cFilterName = GetFilterName(cItemPath);

			// no reads from the default data filter item allowed.
			if (IsFilterItem(cItemPath) && cFilterName.IsEmpty())
			{
				OpcWriteVariant(cValue, (int)OPC_WRITEABLE);  
				break;
			}

			OpcWriteVariant(cValue, m_iAccessRights);  
			break;
		}

		case OPC_PROPERTY_SCAN_RATE:     { OpcWriteVariant(cValue, m_fltScanRate);     break; }
		case OPC_PROPERTY_EU_TYPE:       { OpcWriteVariant(cValue, (int)m_eEUType);    break; }
		case OPC_PROPERTY_EU_INFO:       { OpcWriteVariant(cValue, m_cEnumValues);     break; }
		case OPC_PROPERTY_HIGH_EU:       { OpcWriteVariant(cValue, m_dblMaxValue);     break; }
		case OPC_PROPERTY_LOW_EU:        { OpcWriteVariant(cValue, m_dblMinValue);     break; }
		
		// complex data properties.
		case OPC_PROPERTY_TYPE_SYSTEM_ID:
		{			
			if (pItem == NULL)
			{
				return OPC_E_INVALID_PID;
			}

			OpcWriteVariant(cValue, pItem->TypeSystemID); 
			break;
		}

		case OPC_PROPERTY_DICTIONARY_ID:
		{
			if (pItem == NULL)
			{
				return OPC_E_INVALID_PID;
			}

			OpcWriteVariant(cValue, pItem->DictionaryID); 
			break; 
		}

		case OPC_PROPERTY_TYPE_ID:
		{
			if (pItem == NULL)
			{
				return OPC_E_INVALID_PID;
			}

			OpcWriteVariant(cValue, pItem->TypeID); 
			break; 
		}
		
		case OPC_PROPERTY_CONSISTENCY_WINDOW:
		{
			if (pItem == NULL)
			{
				return OPC_E_INVALID_PID;
			}

			OpcWriteVariant(cValue, (COpcString)OPC_CONSISTENCY_WINDOW_UNKNOWN); 
			break; 
		}

		case OPC_PROPERTY_WRITE_BEHAVIOR:
		{
			if (pItem == NULL)
			{
				return OPC_E_INVALID_PID;
			}

			OpcWriteVariant(cValue, (COpcString)OPC_WRITE_BEHAVIOR_ALL_OR_NOTHING); 
			break; 
		}
		
		case OPC_PROPERTY_UNCONVERTED_ITEM_ID:
		{	
			if (pItem->TypeID.IsEmpty())
			{
				return OPC_E_INVALID_PID;
			}

			OpcWriteVariant(cValue, m_cItemID); 
			break; 
		}	
		
		case OPC_PROPERTY_UNFILTERED_ITEM_ID:
		{	
			if (!IsFilterItem(cItemPath))
			{
				return OPC_E_INVALID_PID;
			}

			OpcWriteVariant(cValue, GetItemID(pItem->TypeName)); 
			break; 
		}

		case OPC_PROPERTY_DATA_FILTER_VALUE:
		{		
			if (!IsFilterItem(cItemPath))
			{
				return OPC_E_INVALID_PID;
			}

			COpcString cFilter;

			if (!pItem->Filters.Lookup(GetFilterName(cItemPath), cFilter))
			{
				return OPC_E_UNKNOWNITEMID;
			}

			OpcWriteVariant(cValue, cFilter); 
			break; 
		}

		// other defined properties.
		default:
		{
			OpcXml::AnyType* pProperty = NULL;

			if (!m_cProperties.Lookup(dwPropertyID, pProperty))
			{
				return OPC_E_INVALID_PID;
			}
			
			// simulate changing property values.
			if (dwPropertyID == OPC_PROPERTY_SIMULATION_COUNT)
			{
				pProperty->uintValue++;				
			}
			
			if (dwPropertyID == OPC_PROPERTY_SIMULATION_TIMESTAMP)
			{
				pProperty->dateTimeValue = ::OpcUtcNow();		
			}

			pProperty->Get(cValue);
			break;
		}
	}

	// read completed successfully.
    return S_OK;
}

// Read
HRESULT COpcDaDeviceItem::Read(
   	COpcDaComplexItem*    pItem,
	const COpcString&     cItemPath,
    VARIANT&              cValue, 
    FILETIME*             pftTimestamp,
    WORD*                 pwQuality
)
{
	// handle non-complex reads.
	if (pItem == NULL)
	{
		if (!m_cValue.Get(cValue))
		{		
			return OPC_E_BADTYPE;
		}
		
		if (pftTimestamp != NULL) *pftTimestamp = m_ftTimestamp;
		if (pwQuality != NULL)    *pwQuality    = m_wQuality;	
		
		return S_OK;
	}		
			
	COpcString cFilterName = GetFilterName(cItemPath);

	// no reads from the default data filter item allowed.
	if (IsFilterItem(cItemPath) && cFilterName.IsEmpty())
	{
		return OPC_E_BADRIGHTS;
	}

	// fetch the type dictionary
	COpcDaTypeDictionary* pDictionary = ::GetCache().GetTypeDictionary(pItem->DictionaryItemID);

	if (pDictionary == NULL)
	{
		return false;
	}

	if (pItem->TypeSystemID == OPC_TYPE_SYSTEM_XMLSCHEMA)
	{
		// write value into a XML document.
		COpcBinaryWriter cWriter;
		COpcXmlDocument  cDocument;

		if (!cWriter.Write(m_cValue, pDictionary->GetBinaryDictionary(), pItem->TypeID, cDocument))
		{
			return OPC_E_BADTYPE; 
		}

		// apply any filter.
		if (!cFilterName.IsEmpty())
		{
			// check that the filter is still valid.
			COpcString cFilterValue;

			if (!pItem->Filters.Lookup(cFilterName, cFilterValue))
			{
				return OPC_E_UNKNOWNITEMID;
			}

			// apply XPATH filter to document.
			COpcXmlElementList cElements;

			if (cDocument.FindElements(cFilterValue, cElements) == 0)
			{				
				// no elements found - return success code.
				cValue.vt = VT_EMPTY;

				if (pftTimestamp != NULL) *pftTimestamp = m_ftTimestamp;
				if (pwQuality != NULL)    *pwQuality    = m_wQuality;	

				return OPCCPX_S_FILTER_NO_DATA; 
			}

			COpcXmlDocument cFilteredDocument;

			// construct a document with a single element.
			if (cElements.GetSize() == 1)
			{
				if (!cFilteredDocument.New(cElements[0]))
				{
					return OPCCPX_E_FILTER_ERROR; 
				}
			}

			// construct a document with multiple elements.
			else
			{
				COpcXmlElement cRoot = cDocument.GetRoot();

				if (!cFilteredDocument.New(cRoot))
				{
					return OPCCPX_E_FILTER_ERROR; 
				}

				cRoot = cFilteredDocument.GetRoot();

				for (UINT ii = 0; ii < cElements.GetSize(); ii++)
				{
					cRoot.AppendChild(cElements[ii]);
				}
			}

			// replace document with filtered version.
			cDocument = cFilteredDocument;
		}

		// fetch xml stream.
		COpcString cXml;

		if (!cDocument.GetXml(cXml))
		{
			return OPC_E_BADTYPE; 
		}

		// return value as xml.
		OpcWriteVariant(cValue, cXml);
	}
	else
	{
		// write value into a buffer.
		COpcBinaryWriter cWriter;

		BYTE* pBuffer  = NULL;
		UINT  uBufSize = 0;

		if (!cWriter.Write(m_cValue, pDictionary->GetBinaryDictionary(), pItem->TypeID, &pBuffer, &uBufSize))
		{
			return OPC_E_BADTYPE; 
		}

		// apply any filter.
		if (!cFilterName.IsEmpty())
		{
			OpcFree(pBuffer);
			return OPCCPX_E_FILTER_INVALID;
		}

		// copy buffer into a variant.
		COpcSafeArray cArray(cValue);

		cArray.Alloc(VT_UI1, uBufSize);

		cArray.Lock();
		BYTE* pData = (BYTE*)cArray.GetData();
		memcpy(pData, pBuffer, uBufSize); 
		cArray.Unlock();

		OpcFree(pBuffer);
	}

	// return quality and timestamp.
	if (pftTimestamp != NULL) *pftTimestamp = m_ftTimestamp;
	if (pwQuality != NULL)    *pwQuality    = m_wQuality;	

	return S_OK;
}

// Write
HRESULT COpcDaDeviceItem::Write(
	const COpcString&     cItemPath,
	bool                  bInternalWrite,
    DWORD                 dwPropertyID,
    const VARIANT&        cValue, 
    FILETIME*             pftTimestamp,
    WORD*                 pwQuality
)
{
	// find complex item referenced by the item path.
	COpcDaComplexItem* pItem = FindItem(cItemPath);

	if (pItem == NULL && m_pNativeFormat != NULL)
	{
		return OPC_E_UNKNOWNITEMID;
	}

	// validate property.
	HRESULT hResult = ValidatePropertyID(pItem, cItemPath, dwPropertyID, OPC_WRITEABLE);

	if (FAILED(hResult))
	{
		return hResult;
	}

	// handle value writes.
	if (dwPropertyID == NULL || dwPropertyID == OPC_PROPERTY_VALUE)
	{
		if (!bInternalWrite && m_bInternalWritesOnly)
		{
			return OPC_E_BADRIGHTS;
		}

		return Write(pItem, cItemPath, cValue, pftTimestamp, pwQuality);
	}

	// check for a correct data type.
	VARTYPE vtType = OpcGetPropertyType(dwPropertyID);

	if (vtType != VT_VARIANT && vtType != cValue.vt)
	{
		return OPC_E_BADTYPE;
	}

	// write non-value
	switch (dwPropertyID)
	{	
		// standard properties.
		case OPC_PROPERTY_DATATYPE:      { return OpcReadVariant(m_vtDataType,    cValue); }
		case OPC_PROPERTY_QUALITY:       { return OpcReadVariant(m_wQuality,      cValue); }	
		case OPC_PROPERTY_TIMESTAMP:     { return OpcReadVariant(m_ftTimestamp,   cValue); }
		case OPC_PROPERTY_ACCESS_RIGHTS: { return OpcReadVariant(m_iAccessRights, cValue); }
		case OPC_PROPERTY_SCAN_RATE:   	 { return OpcReadVariant(m_fltScanRate,   cValue); }
		case OPC_PROPERTY_EU_TYPE:       { return OpcReadVariant((int&)m_eEUType, cValue); }
		case OPC_PROPERTY_EU_INFO:       { return OpcReadVariant(m_cEnumValues,   cValue); }
		case OPC_PROPERTY_HIGH_EU:       { return OpcReadVariant(m_dblMaxValue,   cValue); }
		case OPC_PROPERTY_LOW_EU:        { return OpcReadVariant(m_dblMinValue,   cValue); }
				
		// other defined properties.
		default:
		{
			OpcXml::AnyType* pProperty = NULL;

			if (!m_cProperties.Lookup(dwPropertyID, pProperty))
			{
				return OPC_E_INVALID_PID;
			}

			pProperty->Set(cValue);
			break;
		}
	}

	// write complete.
	return S_OK;
}

// WriteFilter
HRESULT COpcDaDeviceItem::WriteFilter(
	COpcDaComplexItem* pItem,
	const COpcString&  cItemPath,
	const VARIANT&     cValue
)
{
	// check data type.
	if (cValue.vt != VT_BSTR)
	{
		return OPC_E_BADTYPE;
	}

	COpcString cFilterName = GetFilterName(cItemPath);

	// update an existing filter.
	if (!cFilterName.IsEmpty())
	{
		if (!pItem->Filters.Lookup(cFilterName))
		{
			return OPC_E_UNKNOWNITEMID;
		}

		// update the filter value.
		if (wcslen(cValue.bstrVal) != 0)
		{			
			pItem->Filters[cFilterName] = cValue.bstrVal;
		}

		// delete the filter item.
		else
		{
			pItem->Filters.RemoveKey(cFilterName);
			RemoveItemAndLink(cItemPath);
		}
		
		return S_OK;
	}

	// parse the filter parameters.
	COpcXmlDocument cDocument;

	if (!cDocument.LoadXml(cValue.bstrVal))
	{
		return OPCCPX_E_FILTER_INVALID;
	}

	COpcXmlElement cElement = cDocument.GetRoot();

	// get the name for the new data filter item.
	COpcXmlAttribute cAttribute = cElement.GetAttribute(TAG_NAME);

	if (cAttribute == NULL)
	{
		return OPCCPX_E_FILTER_INVALID;
	}

	// check for duplicate item names.
	cFilterName = cAttribute.GetValue();

	if (pItem->Filters.Lookup(cFilterName))
	{
		return OPCCPX_E_FILTER_DUPLICATE;
	}
	
	// add new item to address space.
	COpcString cNewItemPath;

	cNewItemPath += cItemPath;
	cNewItemPath += TAG_SEPARATOR;
	cNewItemPath += cFilterName;

	if (!AddItemAndLink(cNewItemPath))
	{
		return E_FAIL;
	}

	// update item filter table.
	pItem->Filters[cFilterName] = cElement.GetValue();

	return S_OK;
}

// Write
HRESULT COpcDaDeviceItem::Write(
	COpcDaComplexItem* pItem,
	const COpcString&  cItemPath,
	const VARIANT&     cValue,
    FILETIME*          pftTimestamp,
    WORD*              pwQuality
)
{
	// handle non-complex writes.
	if (pItem == NULL)
	{
		if (m_vtDataType != VT_EMPTY && cValue.vt != m_vtDataType)
		{
			return OPC_E_BADTYPE;
		}

		if (m_eEUType == OPC_ANALOG)
		{
			if (!CheckRange(cValue))
			{
				return OPC_E_RANGE;
			}
		}

		m_cValue.Set(cValue);
		
		if (pftTimestamp != NULL) m_ftTimestamp = *pftTimestamp;
		if (pwQuality != NULL)    m_wQuality    = *pwQuality;	
		
		return S_OK;
	}	
	
	// handle a filter item request.
	if (IsFilterItem(cItemPath))
	{
		return WriteFilter(pItem, cItemPath, cValue);
	}

	// fetch the type dictionary.
	COpcDaTypeDictionary* pDictionary = ::GetCache().GetTypeDictionary(pItem->DictionaryItemID);

	if (pDictionary == NULL)
	{
		return false;
	}
		
	if (pItem->TypeSystemID == OPC_TYPE_SYSTEM_XMLSCHEMA)
	{
		if (cValue.vt != VT_BSTR)
		{
			return OPC_E_BADTYPE;
		}

		COpcXmlDocument cDocument;

		if (!cDocument.LoadXml(cValue.bstrVal))
		{
			return OPC_E_BADTYPE;
		}

		COpcBinaryReader cReader;
		OpcXml::AnyType  cNewValue;

		if (!cReader.Read(cDocument, pDictionary->GetBinaryDictionary(), pItem->TypeID, cNewValue))
		{
			return OPC_E_BADTYPE;
		}

		// update the value.
		cNewValue.MoveTo(m_cValue);
	}
	else
	{
		if (cValue.vt != (VT_ARRAY | VT_UI1))
		{
			return OPC_E_BADTYPE;
		}

		// parse the buffer.
		COpcSafeArray cArray((VARIANT&)cValue);

		cArray.Lock();

		BYTE* pBuffer  = (BYTE*)cArray.GetData();
		UINT  uBufSize = cArray.GetLength();

		COpcBinaryReader cReader;
		OpcXml::AnyType  cNewValue;

		bool bResult = cReader.Read(pBuffer, uBufSize, pDictionary->GetBinaryDictionary(), pItem->TypeID, cNewValue);

		cArray.Unlock();

		if (!bResult)
		{
			return OPC_E_BADTYPE;
		}

		// update the value.
		cNewValue.MoveTo(m_cValue);
	}

	// update the quality and timestamp.
	if (pftTimestamp != NULL) m_ftTimestamp = *pftTimestamp;
	if (pwQuality != NULL)    m_wQuality    = *pwQuality;	

	return S_OK;
}

//========================================================================
// IOpcXmlSerialize

// Init
void COpcDaDeviceItem::Init()
{
	m_bInternalWritesOnly = false;

    m_vtDataType    = VT_EMPTY;
    m_wQuality      = OPC_QUALITY_GOOD;  
    m_ftTimestamp   = OpcMinDate();
    m_iAccessRights = OPC_READABLE | OPC_WRITEABLE;
    m_fltScanRate   = 0;
    m_eEUType       = OPC_NOENUM;
    m_dblMaxValue   = QNAN;
    m_dblMinValue   = QNAN;
	m_pNativeFormat = NULL;
    m_cValue.Clear();
    m_cEnumValues.RemoveAll();
	m_cProperties.RemoveAll();
	m_cTypeConversions.RemoveAll();
}

// Clear
void COpcDaDeviceItem::Clear()
{
    OPC_POS pos = m_cProperties.GetStartPosition();

    while (pos != NULL)
    {
        DWORD dwID = NULL;
        OpcXml::AnyType* pProperty = NULL;
        m_cProperties.GetNextAssoc(pos, dwID, pProperty);

        delete pProperty;
    }

	m_cProperties.RemoveAll();

	pos = m_cTypeConversions.GetStartPosition();

    while (pos != NULL)
    {
        COpcString cTypeName;
        COpcDaComplexItem* pItem = NULL;
        m_cTypeConversions.GetNextAssoc(pos, cTypeName, pItem);

        delete pItem;
    }

	m_cTypeConversions.RemoveAll();

	delete m_pNativeFormat;

    Init();
}

// Read
bool COpcDaDeviceItem::Read(COpcXmlElement& cElement)
{
    Clear();

    // get item properties.
    COpcXmlElementList cChildren;

    UINT uCount = cElement.GetChildren(cChildren);

    // must have at least a value specified.
    if (uCount == 0)
    {
        return false;
    }

    // read initial value.
    if (!OpcXml::ReadXml(cChildren[0], m_cValue)) 
    {
        return false;
    }

	// get data type.
	m_vtDataType = OpcXml::GetVarType(m_cValue.eType);

	if (m_cValue.iLength >= 0)
	{
		m_vtDataType |= VT_ARRAY;
	} 

	// read complex type attributes.
	COpcString cDictionary;
	OpcXml::QName cQName(TAG_DICTIONARY);

	if (m_cValue.cSchema.Get(cQName, cDictionary))
	{
		COpcDaTypeDictionary* pDictionary = ::GetCache().CreateTypeDictionary(cDictionary);

		if (pDictionary != NULL)
		{
			COpcString cTypeName = m_cValue.cSchema.GetType().GetName();

			m_pNativeFormat = new COpcDaComplexItem();

			m_pNativeFormat->TypeName         = (LPCWSTR)NULL;
			m_pNativeFormat->TypeSystemID     = pDictionary->GetTypeSystemID();
			m_pNativeFormat->DictionaryID     = pDictionary->GetDictionaryID();
			m_pNativeFormat->DictionaryItemID = pDictionary->GetItemID();
			m_pNativeFormat->TypeID           = pDictionary->GetTypeID(cTypeName);
			m_pNativeFormat->TypeItemID       = pDictionary->GetTypeItemID(cTypeName);
			m_pNativeFormat->EnableFilters    = false;
			
			COpcDaTypeDictionary* pMapping = ::GetCache().CreateXmlSchemaMapping(cDictionary);

			if (pMapping != NULL)
			{
				COpcDaComplexItem* pXmlFormat = new COpcDaComplexItem();

				pXmlFormat->TypeName         = TAG_XML;
				pXmlFormat->TypeSystemID     = pMapping->GetTypeSystemID();
				pXmlFormat->DictionaryID     = pMapping->GetDictionaryID();
				pXmlFormat->DictionaryItemID = pMapping->GetItemID();
				pXmlFormat->TypeID           = pMapping->GetTypeID(cTypeName);
				pXmlFormat->TypeItemID       = pMapping->GetTypeItemID(cTypeName);
				pXmlFormat->EnableFilters    = true;

				m_cTypeConversions.SetAt(TAG_XML, pXmlFormat);
			}
		}

		m_vtDataType = VT_ARRAY | VT_UI1;
	}

    for (UINT ii = 1; ii < uCount; ii++)
    {
        // read property id.
        DWORD dwID = 0;

        if (!OpcXml::ReadXml(cChildren[ii].GetAttribute(TAG_PROPERTY_ID), dwID)) 
        {
            return false;
        }

        // read property value.
		OpcXml::AnyType cValue;
        
        if (!OpcXml::ReadXml(cChildren[ii], cValue)) 
        {
            return false;
		}
	
		// save property value.
		switch (dwID)
		{	
			case OPC_PROPERTY_QUALITY:       { if (!cValue.Get(m_wQuality))        return false; break; }	
			case OPC_PROPERTY_TIMESTAMP:     { if (!cValue.Get(m_ftTimestamp))     return false; break; }
			case OPC_PROPERTY_ACCESS_RIGHTS: { if (!cValue.Get(m_iAccessRights))   return false; break; }
			case OPC_PROPERTY_SCAN_RATE:   	 { if (!cValue.Get(m_fltScanRate))     return false; break; }
			case OPC_PROPERTY_EU_TYPE:       { if (!cValue.Get((int&)m_eEUType))   return false; break; }									 
			case OPC_PROPERTY_EU_INFO:       { if (!cValue.Get(m_cEnumValues))     return false; break; }		
			case OPC_PROPERTY_HIGH_EU:       { if (!cValue.Get(m_dblMaxValue))     return false; break; }
			case OPC_PROPERTY_LOW_EU:        { if (!cValue.Get(m_dblMinValue))     return false; break; }	
			
			default:
			{
				OpcXml::AnyType* pProperty = NULL;

				if (!m_cProperties.Lookup(dwID, pProperty))
				{
					m_cProperties[dwID] = pProperty = new OpcXml::AnyType();
				}
				
				*pProperty = cValue;
				break;
			}
		}
    }

    return true;
}

// Write
bool COpcDaDeviceItem::Write(COpcXmlElement& cElement)
{
	OPC_ASSERT(false);  
	
	// not implemented.
	
	return false;
}
	
// FindItem
COpcDaComplexItem* COpcDaDeviceItem::FindItem(const COpcString& cItemPath)
{
	if (m_pNativeFormat == NULL)
	{
		return NULL;
	}

	COpcDaComplexItem* pItem = NULL;

	if (!cItemPath.IsEmpty()) 
	{
		COpcString cTypeName = cItemPath;

		int iIndex = cTypeName.Find(TAG_SEPARATOR);

		if (iIndex != -1)
		{
			cTypeName = cTypeName.SubStr(0, iIndex);
		}
		
		if (cTypeName == CPX_DATA_FILTERS)
		{
			return m_pNativeFormat;
		}

		COpcDaComplexItem* pItem = NULL;

		if (!m_cTypeConversions.Lookup(cTypeName, pItem))
		{
			return NULL;
		}

		return pItem;
	}

	return m_pNativeFormat;
}

// IsFilterItem
bool COpcDaDeviceItem::IsFilterItem(const COpcString& cItemPath)
{
	return (cItemPath.Find(CPX_DATA_FILTERS) != -1);
}

// GetFilterName
COpcString COpcDaDeviceItem::GetFilterName(const COpcString& cItemPath)
{
	int iIndex = cItemPath.Find(CPX_DATA_FILTERS);

	if (iIndex != -1)
	{
		return cItemPath.SubStr(iIndex + _tcslen(CPX_DATA_FILTERS) + 1);
	}

	return (LPCWSTR)NULL;
}

// GetItemID
COpcString COpcDaDeviceItem::GetItemID(const COpcString& cItemPath)
{
	COpcString cItemID = m_cItemID;

	if (!cItemPath.IsEmpty())
	{
		cItemID += TAG_SEPARATOR;
		cItemID += CPX_DATABASE_ROOT;
		cItemID += TAG_SEPARATOR;
		cItemID += cItemPath;
	}

	return cItemID;
}

// AddItemAndLink
bool COpcDaDeviceItem::AddItemAndLink(const COpcString& cItemPath)
{
	if (!::GetCache().AddItemAndLink(GetItemID(cItemPath)))
	{
		return false;
	}

	return true;
}

// RemoveItemAndLink
void COpcDaDeviceItem::RemoveItemAndLink(const COpcString& cItemPath)
{
	::GetCache().RemoveItemAndLink(GetItemID(cItemPath));
}

// BuildAddressSpace
bool COpcDaDeviceItem::BuildAddressSpace()
{      
    if (!::GetCache().AddItemAndLink(m_cItemID))
	{
		return false;
	}

	// nothing more to do for non-complex items.
	if (m_pNativeFormat == NULL)
	{
		return true;
	}

	// add the data filter item.
	if (m_pNativeFormat->EnableFilters)
	{
		if (!AddItemAndLink(CPX_DATA_FILTERS))
		{
			return false;
		}
	}

	// add the type conversion items.
	OPC_POS pos = m_cTypeConversions.GetStartPosition();
	
	while (pos != NULL)
	{
		COpcString cTypeName;
		COpcDaComplexItem* pItem = NULL;
		m_cTypeConversions.GetNextAssoc(pos, cTypeName, pItem);

		if (!AddItemAndLink(cTypeName))
		{
			return false;
		}

		// add the data filter item for the type conversion item.
		if (pItem->EnableFilters)
		{			
			cTypeName += TAG_SEPARATOR;
			cTypeName += CPX_DATA_FILTERS;

			if (!AddItemAndLink(cTypeName))
			{
				return false;
			}
		}
	}

	return true;
}

// ClearAddressSpace
void COpcDaDeviceItem::ClearAddressSpace()
{
	if (m_pNativeFormat != NULL)
	{
		if (m_pNativeFormat->EnableFilters)
		{
			// remove all data filter instance items.
			OPC_POS pos = m_pNativeFormat->Filters.GetStartPosition();

			while (pos != NULL)
			{
				COpcString cName;
				m_pNativeFormat->Filters.GetNextAssoc(pos, cName);

				COpcString cItemPath;

				cItemPath += CPX_DATA_FILTERS;
				cItemPath += TAG_SEPARATOR;
				cItemPath += cName;

				RemoveItemAndLink(cItemPath);
			}

			// remove the data filter item.
			RemoveItemAndLink(CPX_DATA_FILTERS);
		}

		// remove the type conversion items.
		OPC_POS pos = m_cTypeConversions.GetStartPosition();
		
		while (pos != NULL)
		{
			COpcString cTypeName;
			COpcDaComplexItem* pItem = NULL;
			m_cTypeConversions.GetNextAssoc(pos, cTypeName, pItem);

			// remove the data filter item for the type conversion item.
			if (pItem->EnableFilters)
			{	
				// remove all data filter instance items.
				OPC_POS pos2 = pItem->Filters.GetStartPosition();

				while (pos2 != NULL)
				{
					COpcString cName;
					pItem->Filters.GetNextAssoc(pos2, cName);

					COpcString cItemPath;

					cItemPath += CPX_DATA_FILTERS;
					cItemPath += TAG_SEPARATOR;
					cItemPath += cName;

					RemoveItemAndLink(cItemPath);
				}

				// remove the data filter item.
				RemoveItemAndLink(CPX_DATA_FILTERS);
			}

			// remove the type conversion item.
			RemoveItemAndLink(cTypeName);
		}
	}

	::GetCache().RemoveItemAndLink(m_cItemID);
}

// Parameters Constructor
COpcDaDeviceItem::Parameters::Parameters()
{		
	uTicks         = 0;
	uInterval      = 1;
	uPeriod        = 0;
	uSamplingRate  = 0;
	eWaveform      = NONE;
	dblMaxValue    = QNAN;
	dblMinValue    = QNAN; 
}

// Parameters Copy Constructor
COpcDaDeviceItem::Parameters::Parameters(const Parameters& cParameters)
{		
	uTicks         = cParameters.uTicks;
	uInterval      = cParameters.uInterval;
	uPeriod        = cParameters.uPeriod;
	uSamplingRate  = cParameters.uSamplingRate;
	eWaveform      = cParameters.eWaveform;
	dblMaxValue    = cParameters.dblMaxValue;
	dblMinValue    = cParameters.dblMinValue; 
}

// Calculate
OpcXml::SByte COpcDaDeviceItem::Calculate(OpcXml::SByte cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, MAX_SBYTE);
	SET_MIN(cParameters, MIN_SBYTE);

	return (OpcXml::SByte)Calculate((OpcXml::Long)cValue, cParameters);
}

// Calculate
OpcXml::Short COpcDaDeviceItem::Calculate(OpcXml::Short cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, MAX_SHORT);
	SET_MIN(cParameters, MIN_SHORT);

	return (OpcXml::Short)Calculate((OpcXml::Long)cValue, cParameters);
}

// Calculate
OpcXml::Int COpcDaDeviceItem::Calculate(OpcXml::Int cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, MAX_INT);
	SET_MIN(cParameters, MIN_INT);

	return (OpcXml::Int)Calculate((OpcXml::Long)cValue, cParameters);
}

// Calculate
OpcXml::Decimal COpcDaDeviceItem::Calculate(OpcXml::Decimal cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, QNAN);
	SET_MIN(cParameters, QNAN);

	OpcXml::Decimal cDecimal;
	cDecimal.int64 = Calculate((OpcXml::Long)cValue.int64, cParameters);
	return cDecimal;
}

// Calculate
OpcXml::Long COpcDaDeviceItem::Calculate(OpcXml::Long cValue, Parameters& cParameters)
{
	// determine upper and lower bounds.
	OpcXml::Long cMax = (cParameters.dblMaxValue == QNAN)?MAX_LONG:(OpcXml::Long)cParameters.dblMaxValue;
	OpcXml::Long cMin = (cParameters.dblMinValue == QNAN)?MIN_LONG:(OpcXml::Long)cParameters.dblMinValue;

	double dblRange = ((double)cMax - (double)cMin);
	double dblDelta = dblRange*(((double)cParameters.uSamplingRate)/((double)cParameters.uPeriod));

	// calculate next value in a ramp.
	if (cParameters.eWaveform == RAMP)
	{
		if (((double)cMax) <= ((double)cValue + dblDelta))
		{
			return cMin;
		}

		return cValue + (OpcXml::Long)dblDelta;
	}

	// calculate next value in a sinusoid.
	if (cParameters.eWaveform == SINE)
	{				
		double dblFraction = (double)((cParameters.uTicks*cParameters.uInterval)%cParameters.uPeriod);
		
		dblFraction /= cParameters.uPeriod;

		double dblDelta = (dblRange*(sin(2*PI*dblFraction)+1.0)/2.0);

		if (dblDelta > MAX_LONG)
		{
			return cMin + (OpcXml::Long)(dblDelta - (double)(MAX_LONG>>2)) + (MAX_LONG>>2);
		}

		return cMin + (OpcXml::Long)dblDelta;
	}

	// calculate the next in an enumeration sequence.
	if (cParameters.eWaveform == ENUM)
	{
		if (cValue == (OpcXml::Long)cParameters.dblMaxValue)
		{
			return 0;
		}

		return cValue + 1;
	}

	// value does not change.
	return cValue;
}

// Calculate
OpcXml::Byte COpcDaDeviceItem::Calculate(OpcXml::Byte cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, MAX_BYTE);
	SET_MIN(cParameters, 0);

	return (OpcXml::Byte)Calculate((OpcXml::ULong)cValue, cParameters);
}

// Calculate
OpcXml::UShort COpcDaDeviceItem::Calculate(OpcXml::UShort cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, MAX_USHORT);
	SET_MIN(cParameters, 0);

	return (OpcXml::UShort)Calculate((OpcXml::ULong)cValue, cParameters);
}

// Calculate
OpcXml::UInt COpcDaDeviceItem::Calculate(OpcXml::UInt cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, MAX_UINT);
	SET_MIN(cParameters, 0);

	return (OpcXml::UInt)Calculate((OpcXml::ULong)cValue, cParameters);
}

// Calculate
OpcXml::DateTime COpcDaDeviceItem::Calculate(OpcXml::DateTime cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, QNAN);
	SET_MIN(cParameters, QNAN);
	
	OpcXml::ULong cDateTime = *((OpcXml::ULong*)&cValue);
	cDateTime = Calculate(cDateTime, cParameters);
	return *((OpcXml::DateTime*)&cValue);
}

// Calculate
OpcXml::ULong COpcDaDeviceItem::Calculate(OpcXml::ULong cValue, Parameters& cParameters)
{
	// determine upper and lower bounds.
	OpcXml::ULong cMax = (cParameters.dblMaxValue == QNAN)?MAX_ULONG:(OpcXml::ULong)cParameters.dblMaxValue;
	OpcXml::ULong cMin = (cParameters.dblMinValue == QNAN)?0:(OpcXml::ULong)cParameters.dblMinValue;

	double dblRange = ((double)cMax - (double)cMin);
	double dblDelta = dblRange*(((double)cParameters.uSamplingRate)/((double)cParameters.uPeriod));

	// calculate next value in a ramp.
	if (cParameters.eWaveform == RAMP)
	{
		if (((double)cMax) <= ((double)cValue + dblDelta))
		{
			return cMin;
		}

		return cValue + (OpcXml::ULong)dblDelta;
	}

	// calculate next value in a sinusoid.
	if (cParameters.eWaveform == SINE)
	{				
		double dblFraction = (double)((cParameters.uTicks*cParameters.uInterval)%cParameters.uPeriod);
		
		dblFraction /= cParameters.uPeriod;

		double dblDelta = (dblRange*(sin(2*PI*dblFraction)+1.0)/2.0);

		if (dblDelta > MAX_ULONG)
		{
			return cMin + (OpcXml::ULong)(dblDelta - (double)(MAX_ULONG>>2)) + (MAX_ULONG>>2);
		}

		return cMin + (OpcXml::ULong)dblDelta;
	}

	// value does not change.
	return cValue;
}

// Calculate
OpcXml::Float COpcDaDeviceItem::Calculate(OpcXml::Float cValue, Parameters& cParameters)
{	
	SET_MAX(cParameters, +MAX_FLOAT);
	SET_MIN(cParameters, -MAX_FLOAT);

	return (OpcXml::Float)Calculate((OpcXml::Double)cValue, cParameters);
}

// Calculate
OpcXml::Double COpcDaDeviceItem::Calculate(OpcXml::Double cValue, Parameters& cParameters)
{
	// determine upper and lower bounds.
	OpcXml::Double cMax = (cParameters.dblMaxValue == QNAN)?+MAX_DOUBLE:(OpcXml::Double)cParameters.dblMaxValue;
	OpcXml::Double cMin = (cParameters.dblMinValue == QNAN)?-MAX_DOUBLE:(OpcXml::Double)cParameters.dblMinValue;

	double dblRange = ((double)cMax - (double)cMin);
	double dblDelta = dblRange*(((double)cParameters.uSamplingRate)/((double)cParameters.uPeriod));

	// calculate next value in a ramp.
	if (cParameters.eWaveform == RAMP)
	{
		if (((double)cMax) <= ((double)cValue + dblDelta))
		{
			return cMin;
		}

		return cValue + (OpcXml::Double)dblDelta;
	}

	// calculate next value in a sinusoid.
	if (cParameters.eWaveform == SINE)
	{				
		double dblFraction = (double)((cParameters.uTicks*cParameters.uInterval)%cParameters.uPeriod);	
		dblFraction /= cParameters.uPeriod;

		return cMin + (OpcXml::Double)(dblRange*(sin(2*PI*dblFraction)+1.0)/2.0);
	}

	// value does not change.
	return cValue;
}

// Calculate
OpcXml::Boolean COpcDaDeviceItem::Calculate(OpcXml::Boolean cValue, Parameters& cParameters)
{
	return !cValue;
}

// Calculate
OpcXml::String COpcDaDeviceItem::Calculate(OpcXml::String& cValue, Parameters& cParameters)
{
	double dblFraction = (double)((cParameters.uTicks*cParameters.uInterval)%cParameters.uPeriod);	
	dblFraction /= cParameters.uPeriod;

	UINT uSeed = (UINT)(dblFraction*MAX_UINT);

	if (cValue != NULL && uSeed == 0)
	{
		for (UINT ii = 0; ii < wcslen(cValue); ii++) uSeed += cValue[ii];
	}

	srand(uSeed);

	int iLength = abs(rand())%80;

	OpcFree(cValue);
	cValue = NULL;

	OpcXml::String cString = OpcArrayAlloc(WCHAR, iLength+1);
	memset(cString, 0, sizeof(WCHAR)*(iLength+1));

	for (int ii = 0; ii < iLength; ii++)
	{
		cString[ii] = (abs(rand())%(126-32))+32;
	}

	return cString;
}

// Update
void COpcDaDeviceItem::Update(LONGLONG uTicks, UINT uInterval)
{
	// build simulation parameter lists.
	Parameters cParameters;

	cParameters.uTicks        = uTicks;
	cParameters.uInterval     = uInterval;
	cParameters.uPeriod       = 0;
	cParameters.uSamplingRate = 0;
	cParameters.eWaveform     = RAMP;
	cParameters.dblMaxValue   = QNAN;
	cParameters.dblMinValue   = QNAN;

	switch (m_eEUType)
	{
		case OPC_ANALOG:
		{
			cParameters.dblMaxValue = m_dblMaxValue;
			cParameters.dblMinValue = m_dblMinValue;
			break;
		}

		case OPC_ENUMERATED:
		{
			cParameters.dblMaxValue = m_cEnumValues.GetSize()-1;
			cParameters.dblMinValue = 0;
			break;
		}
	}

	if (Update(m_cValue, cParameters, uTicks, uInterval))
	{
		m_ftTimestamp = OpcUtcNow();
	}
}

// Init
void COpcDaDeviceItem::Init(OpcXml::AnyType& cValue, Parameters& cParameters)
{
	OpcXml::Schema& cSchema = cValue.cSchema;

	// get simulation period.
	OpcXml::QName cQName(TAG_PERIOD);
	
	UINT uPeriod = 0;

	if (cSchema.Get(cQName, uPeriod))
	{
		cParameters.uPeriod = uPeriod;
	}

	// get simulation sampling rate.
	cQName.SetName(TAG_SAMPLING_RATE);
	
	UINT uSamplingRate = 0;

	if (cSchema.Get(cQName, uSamplingRate))
	{
		cParameters.uSamplingRate = uSamplingRate;
	}
    
	// get simulation wave form.
	cQName.SetName(TAG_WAVEFORM);
	
	int eWaveform = 0;
	
	if (cSchema.Get(cQName, eWaveform))
	{
		cParameters.eWaveform = eWaveform;
	}

	// get simulation max value.
	cQName.SetName(TAG_MAX_VALUE);
	
	double dblMaxValue = 0;

	if (cSchema.Get(cQName, dblMaxValue))
	{
		cParameters.dblMaxValue = dblMaxValue;
	}

	// get simulation min value.
	cQName.SetName(TAG_MIN_VALUE);
	
	double dblMinValue = 0;
	
	if (cSchema.Get(cQName, dblMinValue))
	{
		cParameters.dblMinValue = dblMinValue;
	}
}

// Update
bool COpcDaDeviceItem::Update(OpcXml::AnyType& cValue, Parameters cParameters, LONGLONG uTicks, UINT uInterval)
{
	// update parameters from current schema.
	Init(m_cValue, cParameters);

	// nothing to do if value does not change.
	if (cValue.eType != OpcXml::XML_ANY_TYPE)
	{
		if (cParameters.uPeriod == 0)
		{
			return false;
		}
	}

	// check whether the sample rate has passed since the last change.
	if (cValue.eType != OpcXml::XML_ANY_TYPE)
	{
		LONGLONG uTicksPerPeriod = cParameters.uPeriod/uInterval;
		LONGLONG uTicksPerSample = cParameters.uSamplingRate/uInterval;

		if (uTicksPerSample > 1 && uTicks%uTicksPerSample != 0) 
		{ 
			return false;
		}
	}

	// update the value.
	if (cValue.iLength < 0)
	{
		switch (cValue.eType)
		{
			case OpcXml::XML_BOOLEAN:  { cValue.boolValue     = Calculate(cValue.boolValue,    cParameters);  break; }
			case OpcXml::XML_SBYTE:    { cValue.sbyteValue    = Calculate(cValue.sbyteValue,    cParameters); break; }
			case OpcXml::XML_BYTE:     { cValue.byteValue     = Calculate(cValue.byteValue,     cParameters); break; }
			case OpcXml::XML_SHORT:    { cValue.shortValue    = Calculate(cValue.shortValue,    cParameters); break; }
			case OpcXml::XML_USHORT:   { cValue.ushortValue   = Calculate(cValue.ushortValue,   cParameters); break; }
			case OpcXml::XML_INT:      { cValue.intValue      = Calculate(cValue.intValue,      cParameters); break; }
			case OpcXml::XML_UINT:     { cValue.uintValue     = Calculate(cValue.uintValue,     cParameters); break; }
			case OpcXml::XML_LONG:     { cValue.longValue     = Calculate(cValue.longValue,     cParameters); break; }
			case OpcXml::XML_ULONG:    { cValue.ulongValue    = Calculate(cValue.ulongValue,    cParameters); break; }
			case OpcXml::XML_FLOAT:    { cValue.floatValue    = Calculate(cValue.floatValue,    cParameters); break; }
			case OpcXml::XML_DOUBLE:   { cValue.doubleValue   = Calculate(cValue.doubleValue,   cParameters); break; }
			case OpcXml::XML_DECIMAL:  { cValue.decimalValue  = Calculate(cValue.decimalValue,  cParameters); break; }
			case OpcXml::XML_DATETIME: { cValue.dateTimeValue = Calculate(cValue.dateTimeValue, cParameters); break; }
			case OpcXml::XML_STRING:   { cValue.stringValue   = Calculate(cValue.stringValue,   cParameters); break; }
		}

		return true;
	}

	bool bChanged = false;

	for (int ii = 0; ii < cValue.iLength; ii++)
	{
		switch (cValue.eType)
		{
			case OpcXml::XML_BOOLEAN:  { cValue.pboolValue[ii]     = Calculate(cValue.pboolValue[ii],     cParameters); break; }
			case OpcXml::XML_SBYTE:    { cValue.psbyteValue[ii]    = Calculate(cValue.psbyteValue[ii],    cParameters); break; }
			case OpcXml::XML_BYTE:     { cValue.pbyteValue[ii]     = Calculate(cValue.pbyteValue[ii],     cParameters); break; }
			case OpcXml::XML_SHORT:    { cValue.pshortValue[ii]    = Calculate(cValue.pshortValue[ii],    cParameters); break; }
			case OpcXml::XML_USHORT:   { cValue.pushortValue[ii]   = Calculate(cValue.pushortValue[ii],   cParameters); break; }
			case OpcXml::XML_INT:      { cValue.pintValue[ii]      = Calculate(cValue.pintValue[ii],      cParameters); break; }
			case OpcXml::XML_UINT:     { cValue.puintValue[ii]     = Calculate(cValue.puintValue[ii],     cParameters); break; }
			case OpcXml::XML_LONG:     { cValue.plongValue[ii]     = Calculate(cValue.plongValue[ii],     cParameters); break; }
			case OpcXml::XML_ULONG:    { cValue.pulongValue[ii]    = Calculate(cValue.pulongValue[ii],    cParameters); break; }
			case OpcXml::XML_FLOAT:    { cValue.pfloatValue[ii]    = Calculate(cValue.pfloatValue[ii],    cParameters); break; }
			case OpcXml::XML_DOUBLE:   { cValue.pdoubleValue[ii]   = Calculate(cValue.pdoubleValue[ii],   cParameters); break; }
			case OpcXml::XML_DECIMAL:  { cValue.pdecimalValue[ii]  = Calculate(cValue.pdecimalValue[ii],  cParameters); break; }
			case OpcXml::XML_DATETIME: { cValue.pdateTimeValue[ii] = Calculate(cValue.pdateTimeValue[ii], cParameters); break; }
			case OpcXml::XML_STRING:   { cValue.pstringValue[ii]   = Calculate(cValue.pstringValue[ii],   cParameters); break; }

			case OpcXml::XML_ANY_TYPE:
			{
				if (Update(cValue.panyTypeValue[ii], cParameters, uTicks, uInterval))
				{
					bChanged = true;
				}

				break;
			}
		}
	}
	
	return bChanged;
}

// CheckRange
bool COpcDaDeviceItem::CheckRange(const VARIANT& cValue)
{
    if (m_eEUType == OPC_ANALOG)
    {
		VARTYPE vtType = (cValue.vt & VT_ARRAY)?VT_ARRAY | VT_R8:VT_R8;

		VARIANT cDouble; OpcVariantInit(&cDouble);

		HRESULT hResult = COpcVariant::ChangeType(cDouble, cValue, NULL, vtType);

		if (FAILED(hResult))
		{
			return false;
		}

		bool bValid = true;

		if (vtType & VT_ARRAY)
		{
			COpcSafeArray cArrayOfDouble(cDouble);

			UINT    uLength = cArrayOfDouble.GetLength();
			double* pValues = (double*)cArrayOfDouble.GetData();

			for (UINT ii = 0; ii < uLength; ii++)
			{
				if (pValues[ii] < m_dblMinValue || pValues[ii] > m_dblMaxValue)
				{
					bValid = false;
					break;
				}
			}
		}

		else
		{
			if (cDouble.dblVal < m_dblMinValue || cDouble.dblVal > m_dblMaxValue)
			{
				bValid = false;
			}
		}

		OpcVariantClear(&cDouble);

		return bValid;
	}

	return true;
}

// ValidatePropertyID
HRESULT COpcDaDeviceItem::ValidatePropertyID(
	COpcDaComplexItem* pItem,
	const COpcString&  cItemPath, 
	DWORD              dwID, 
	int                iAccessRequired
)
{
	// validate property id.
	switch (dwID)
	{
		// check access rights for value properties.
		case NULL:
		case OPC_PROPERTY_VALUE:
		case OPC_PROPERTY_QUALITY:
		case OPC_PROPERTY_TIMESTAMP:
		{
			if ((m_iAccessRights & iAccessRequired) == 0)
			{
				return OPC_E_BADRIGHTS;
			}

			break;
		}
		
		// no checks required for intrinsic properties.
		case OPC_PROPERTY_DATATYPE:
		case OPC_PROPERTY_ACCESS_RIGHTS:
		case OPC_PROPERTY_SCAN_RATE:
		case OPC_PROPERTY_EU_TYPE:
		case OPC_PROPERTY_EU_INFO:
		{
			break;
		}

		// complex data properties.
		case OPC_PROPERTY_TYPE_SYSTEM_ID:
		case OPC_PROPERTY_DICTIONARY_ID:
		case OPC_PROPERTY_TYPE_ID:
		case OPC_PROPERTY_CONSISTENCY_WINDOW:
		case OPC_PROPERTY_WRITE_BEHAVIOR:
		{
			if (IsFilterItem(cItemPath) && GetFilterName(cItemPath).IsEmpty())
			{
				return OPC_E_INVALID_PID;
			}

			if (iAccessRequired != OPC_READABLE)
			{
				return OPC_E_BADRIGHTS;
			}

			break;
		}

		case OPC_PROPERTY_UNCONVERTED_ITEM_ID:
		{	
			if (pItem == m_pNativeFormat)
			{
				return OPC_E_INVALID_PID;
			}

			if (iAccessRequired != OPC_READABLE)
			{
				return OPC_E_BADRIGHTS;
			}
			
			break;
		}	
		
		case OPC_PROPERTY_UNFILTERED_ITEM_ID:
		{	
			if (!IsFilterItem(cItemPath))
			{
				return OPC_E_INVALID_PID;
			}

			if (iAccessRequired != OPC_READABLE)
			{
				return OPC_E_BADRIGHTS;
			}

			break;
		}

		case OPC_PROPERTY_DATA_FILTER_VALUE:
		{
			if (!IsFilterItem(cItemPath) || GetFilterName(cItemPath).IsEmpty())
			{
				return OPC_E_INVALID_PID;
			}

			if (iAccessRequired != OPC_READABLE)
			{
				return OPC_E_BADRIGHTS;
			}

			break;
		}

		// eu limits only valid for analog items.
		case OPC_PROPERTY_HIGH_EU:
		case OPC_PROPERTY_LOW_EU:
		{
			if (m_eEUType != OPC_ANALOG)
			{
				return OPC_E_INVALID_PID;
			}

			break;
		}

		// lookup any addition property.
		default:
		{
			if (!m_cProperties.Lookup(dwID))
			{
				return OPC_E_INVALID_PID;
			}

			break;
		}
	}

	return S_OK;
}
