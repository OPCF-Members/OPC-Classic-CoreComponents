//============================================================================
// TITLE: COpcDaCacheItem.cpp
//
// CONTENTS:
// 
// A single item in the global cache for an OPC server.
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
// 2002/09/03 RSA   First release.
// 2002/11/16 RSA   Second release.
// 2003/03/22 RSA   Added support for complex data.
// 2003/06/25 RSA   Fixed memory problems.

#include "StdAfx.h"
#include "COpcDaCacheItem.h"
#include "COpcDaCache.h"

//============================================================================
// Local Declarations

#define MAX_PROPERTY_DESCRIPTION 1024

//============================================================================
// COpcDaCacheItem

// Constructor
COpcDaCacheItem::COpcDaCacheItem(const COpcString& cItemID, IOpcDaDevice* pDevice)
{
    Init();

    m_pDevice = pDevice;
    m_cItemID = cItemID;
}

// Init
void COpcDaCacheItem::Init()
{	
	m_ftTimestamp = OpcMinDate();
	m_wQuality    = OPC_QUALITY_GOOD;
}

// Clear
void COpcDaCacheItem::Clear()
{
    m_cValue.Clear();
    Init();
}

// GetDataType
VARTYPE COpcDaCacheItem::GetDataType()
{
    if (m_pDevice == NULL)
    {
        return VT_EMPTY;
    }

    VARIANT cProperty; OpcVariantInit(&cProperty);

    HRESULT hResult = m_pDevice->Read(m_cItemID, OPC_PROPERTY_DATATYPE, cProperty);

    if (FAILED(hResult))
    {
        return VT_EMPTY;
    }

    return cProperty.iVal;
}

// GetAccessRights
DWORD COpcDaCacheItem::GetAccessRights()
{
    if (m_pDevice == NULL)
    {
        return OPC_READABLE | OPC_WRITEABLE;
    }

    VARIANT cProperty; OpcVariantInit(&cProperty);

    HRESULT hResult = m_pDevice->Read(m_cItemID, OPC_PROPERTY_ACCESS_RIGHTS, cProperty);

    if (FAILED(hResult))
    {
        return OPC_READABLE | OPC_WRITEABLE;
    }

    return cProperty.lVal;
}

// GetEUType
OPCEUTYPE COpcDaCacheItem::GetEUType()
{
    if (m_pDevice == NULL)
    {
        return OPC_NOENUM;
    }

    VARIANT cProperty; OpcVariantInit(&cProperty);

    HRESULT hResult = m_pDevice->Read(m_cItemID, OPC_PROPERTY_EU_TYPE, cProperty);

    if (FAILED(hResult))
    {
        return OPC_NOENUM;
    }

    return (OPCEUTYPE)cProperty.lVal;
}

// GetLimits
bool COpcDaCacheItem::GetLimits(double& dblMinValue, double& dblMaxValue)
{
    if (m_pDevice == NULL)
    {
        return false;
    }

    VARIANT cProperty; OpcVariantInit(&cProperty);
    
    HRESULT hResult = m_pDevice->Read(m_cItemID, OPC_PROPERTY_LOW_EU, cProperty);

    if (FAILED(hResult))
    {
        return false;
    }

    dblMinValue = cProperty.dblVal;

    hResult = m_pDevice->Read(m_cItemID, OPC_PROPERTY_HIGH_EU, cProperty);

    if (FAILED(hResult))
    {
        return false;
    }

    dblMaxValue = cProperty.dblVal;

    return true;
}

// GetEnumValues
bool COpcDaCacheItem::GetEnumValues(COpcStringArray& cEnumValues)
{
    cEnumValues.RemoveAll();

    if (m_pDevice == NULL)
    {
        return false;
    }

    VARIANT cProperty; OpcVariantInit(&cProperty);
    
    HRESULT hResult = m_pDevice->Read(m_cItemID, OPC_PROPERTY_EU_INFO, cProperty);

    if (FAILED(hResult))
    {
        return false;
    }

    COpcSafeArray cArray(cProperty);

    if (cArray.GetLength() > 0)
    {
        cEnumValues.SetSize(cArray.GetLength());

        cArray.Lock();

        BSTR* pValues = (BSTR*)cArray.GetData();

        for (UINT ii = 0; ii < cEnumValues.GetSize(); ii++)
        {
            cEnumValues[ii] = pValues[ii];
        }

        cArray.Unlock();
    }

    OpcVariantClear(&cProperty);

    return true;
}

// GetItemResult
void COpcDaCacheItem::GetItemResult(OPCITEMRESULT& cResult)
{
	cResult.vtCanonicalDataType = GetDataType();
	cResult.dwAccessRights      = GetAccessRights();
	cResult.dwBlobSize          = 0;
	cResult.pBlob               = NULL;
}

// GetItemAttributes
void COpcDaCacheItem::GetItemAttributes(OPCITEMATTRIBUTES& cAttributes)
{
    cAttributes.vtCanonicalDataType = GetDataType();
    cAttributes.dwAccessRights      = GetAccessRights();
    cAttributes.dwEUType            = GetEUType();
   
    OpcVariantInit(&cAttributes.vEUInfo);

    switch (cAttributes.dwEUType)
    {
        case OPC_ENUMERATED:
        {
            COpcStringArray cEnumValues;

            if (!GetEnumValues(cEnumValues))
            {
                break;
            }

            // return enumerations as a VT_ARRAY | VT_BSTR.
            COpcSafeArray cArray(cAttributes.vEUInfo);

            cArray.Alloc(VT_BSTR, cEnumValues.GetSize());

            cArray.Lock();

            BSTR* pValues = (BSTR*)cArray.GetData();

            for (UINT ii = 0; ii < cEnumValues.GetSize(); ii++)
            {
                pValues[ii] = SysAllocString((LPCWSTR)cEnumValues[ii]);
            }

            cArray.Unlock();
            break;
        }

        case OPC_ANALOG:
        {
            double dblMin = 0;
            double dblMax = 0;

            if (!GetLimits(dblMin, dblMax))
            {
                break;
            }

            // return engineering unit range as a VT_ARRAY | VT_R8.
            COpcSafeArray cArray(cAttributes.vEUInfo);

            cArray.Alloc(VT_R8, 2);

            cArray.Lock();

            double* pValues = (double*)cArray.GetData();

            pValues[0] = dblMin;
            pValues[1] = dblMax;

            cArray.Unlock();

            break;
        }
    }
}

// GetProperties
HRESULT COpcDaCacheItem::GetProperties(
	bool                bReturnValues,
	COpcDaPropertyList& cProperties
)
{
    if (m_pDevice == NULL)
    {
        return OPC_E_INVALID_PID;
    }

	return m_pDevice->GetAvailableProperties(m_cItemID, bReturnValues, cProperties);
}

// GetProperties
HRESULT COpcDaCacheItem::GetProperties(
	const COpcList<DWORD>& cIDs,
	bool                   bReturnValues,
	COpcDaPropertyList&    cProperties
)
{
    if (m_pDevice == NULL)
    {
        return OPC_E_INVALID_PID;
    }

	return m_pDevice->GetAvailableProperties(m_cItemID, cIDs, bReturnValues, cProperties);
}

// GetProperty
HRESULT COpcDaCacheItem::GetProperty(DWORD dwPropertyID, VARIANT& cValue)
{
    if (m_pDevice == NULL)
    {
        return OPC_E_INVALID_PID;
    }

	return m_pDevice->Read(m_cItemID, dwPropertyID, cValue);
}

// Refresh
HRESULT COpcDaCacheItem::Refresh()
{
	// do nothing if item does not have an underlying device.
	if (m_pDevice == NULL)
	{
		return S_OK;
	}

	// read current value and quality from device.
	HRESULT hResult = m_pDevice->Read(
        m_cItemID, 
        NULL, 
        m_cValue.GetRef(), 
        &m_ftTimestamp, 
        &m_wQuality);

	if (FAILED(hResult))
	{        
        return hResult;
	}

    // update cache timestamp - since data is known to be accurate.
    m_ftTimestamp = OpcUtcNow();

	return hResult;
}

// Read
HRESULT COpcDaCacheItem::Read(
	LCID      lcid,
	VARTYPE   vtReqType,
    DWORD     dwMaxAge,
    DWORD     dwPropertyID,
	VARIANT&  cValue,
	FILETIME& ftTimestamp,
	WORD&     wQuality
)
{
	HRESULT hDeviceResult = S_OK;

    // check max age - read from device if required.
    FILETIME ftUtcNow = OpcUtcNow();

    ULONGLONG ullTicks = 0;
    ULONGLONG ullTicksMin = 0;

    memcpy(&ullTicks,    &m_ftTimestamp, sizeof(ULONGLONG));
    memcpy(&ullTicksMin, &ftUtcNow,      sizeof(ULONGLONG));

    ullTicksMin -= dwMaxAge*10000;      

    if (ullTicksMin >= ullTicks)
    {
        hDeviceResult = Refresh();
    }

	if (FAILED(hDeviceResult))
	{
		return hDeviceResult;
	}
	
	HRESULT hResult = S_OK;

    // handle 'value' read.
    if (dwPropertyID == NULL || dwPropertyID == OPC_PROPERTY_VALUE)
    {
        // check if conversion to enumeration string is required.
        OPCEUTYPE eEUType = GetEUType();

        if (eEUType == OPC_ENUMERATED && (vtReqType & VT_TYPEMASK) == VT_BSTR)
        {
            hResult = ToEnumValue(cValue, m_cValue);
        
            if (FAILED(hResult))
            {
                return hResult;
            }
        }

        // else change to requested type.
        else
        {
            hResult = COpcVariant::ChangeType(cValue, m_cValue, lcid, vtReqType);
        
            if (FAILED(hResult))
            {
                return hResult;
            }
        }

        ftTimestamp = m_ftTimestamp;
        wQuality    = m_wQuality;
    }

    // handle property read.
    else
    {
		// check if no conversion is required.
		if (vtReqType == VT_EMPTY)
		{
			hResult = GetProperty(dwPropertyID, cValue);
		}

		// handle type conversion.
		else
		{
			VARIANT cProperty; OpcVariantInit(&cProperty);
			
			hResult = GetProperty(dwPropertyID, cProperty);
			
			if (SUCCEEDED(hResult))
			{
				hResult = COpcVariant::ChangeType(cValue, cProperty, lcid, vtReqType);
				OpcVariantClear(&cProperty);
			}
		}
    
		// check for error reading/converting property.
        if (FAILED(hResult))
        {
            return hResult;
        }

        ftTimestamp = m_ftTimestamp;
 
        // always good quality for property reads.
        wQuality = OPC_QUALITY_GOOD;
    }

	return hDeviceResult;
}

// Write
HRESULT COpcDaCacheItem::Write(
	LCID      lcid,
    DWORD     dwPropertyID,
	VARIANT&  cValue,
	FILETIME* pftTimestamp,
	WORD*     pwQuality
)
{
	HRESULT hResult = S_OK;

    COpcVariant cNewValue;

    // handle 'value' write.
    if (dwPropertyID == NULL || dwPropertyID == OPC_PROPERTY_VALUE)
    {
        // check access rights (compliance test requires this).
        if ((GetAccessRights() & OPC_WRITEABLE) == 0)
        {
            return OPC_E_BADRIGHTS;
        }

        // check if conversion from enumeration string is required.
        OPCEUTYPE eEUType = GetEUType();

        if (eEUType == OPC_ENUMERATED && (cValue.vt & VT_TYPEMASK) == VT_BSTR)
        {
            hResult = FromEnumValue(cNewValue.GetRef(), cValue);
        
            if (FAILED(hResult))
            {
                return hResult;
            }
        }

        // else change to requested type.
        else
        {
            // can't write null values.
            if (cValue.vt == VT_EMPTY)
            {
                return OPC_E_BADTYPE;
            }

            // convert to canonical data type.
            hResult = COpcVariant::ChangeType(
                cNewValue.GetRef(), 
                cValue, 
                lcid, 
                GetDataType());
        
            if (FAILED(hResult))
            {
                return hResult;
            }
        }
    }

    // handle property write.
    else
    {
        hResult = COpcVariant::ChangeType(
            cNewValue.GetRef(), 
            cValue, 
            lcid, 
            OpcGetPropertyType(dwPropertyID));
    
        if (FAILED(hResult))
        {
            return hResult;
        }
    }

    // write to device.
    if (m_pDevice != NULL)
    {
        hResult = m_pDevice->Write(
            m_cItemID,  
            dwPropertyID, 
            cNewValue, 
            pftTimestamp, 
            pwQuality);

        if (FAILED(hResult))
        {
            return hResult;
        }
    }
  
    return S_OK;
}

// FindEnumIndex
int COpcDaCacheItem::FindEnumIndex(
    COpcStringArray& cEnumValues, 
    LPCWSTR          szEnumValue
)
{
    if (szEnumValue == NULL)
    {
        return -1;
    }

    for (UINT ii = 0; ii < cEnumValues.GetSize(); ii++)
    {
        if (wcscmp(szEnumValue, (LPCWSTR)cEnumValues[ii]) == 0)
        {
            return ii;
        }
    }

    return -1;
}

// FindEnumValue
BSTR COpcDaCacheItem::FindEnumValue(
    COpcStringArray& cEnumValues, 
    int              iEnumIndex
)
{
    if (iEnumIndex < 0 || iEnumIndex >= (int)cEnumValues.GetSize())
    {
        return NULL;
    }

    return SysAllocString((LPCWSTR)cEnumValues[iEnumIndex]);
}

// ToEnumValue
HRESULT COpcDaCacheItem::ToEnumValue(	
	VARIANT&       cDst,
    const VARIANT& cSrc
)
{
    // get list of enumerations.
    COpcStringArray cEnumValues;

    if (!GetEnumValues(cEnumValues))
    {
        return OPC_E_BADTYPE;
    }

    // check already a string.
    if ((cSrc.vt & VT_TYPEMASK) == VT_BSTR)
    {
        OpcVariantCopy(&cDst, (VARIANT*)&cSrc);
        return S_OK;
    }

    // convert from array of indicies to an array of strings.
    if (cSrc.vt & VT_ARRAY)
    {
        COpcSafeArray cSrcArray((VARIANT&)cSrc);
        COpcSafeArray cDstArray(cDst);

        UINT uLength = cSrcArray.GetLength();

        cDstArray.Alloc(VT_BSTR, uLength);

        int*  pSrc = (int*)cSrcArray.GetData();
        BSTR* pDst = (BSTR*)cDstArray.GetData();
        
        UINT ii;
        for (ii = 0; ii < uLength; ii++)
        {
            pDst[ii] = FindEnumValue(cEnumValues, pSrc[ii]);

            if (pDst[ii] == NULL)
            {
                OpcVariantClear(&cDst);
                break;
            }
        }

        if (ii < uLength)
        {
            return OPC_E_BADTYPE;
        }
    }

    // convert from an index to a string.
    else
    {
        cDst.vt      = VT_BSTR;
        cDst.bstrVal = FindEnumValue(cEnumValues, cSrc.lVal);

        if (cDst.bstrVal == NULL)
        {
            return OPC_E_BADTYPE;
        }    
    }

    return S_OK;
}

// FromEnumValue
HRESULT COpcDaCacheItem::FromEnumValue(	
    VARIANT&       cDst,
    const VARIANT& cSrc
)
{
    // get list of enumerations.
    COpcStringArray cEnumValues;

    if (!GetEnumValues(cEnumValues))
    {
        return OPC_E_BADTYPE;
    }

    // convert from array of strings to an array of indicies.
    if (cSrc.vt & VT_ARRAY)
    {
        COpcSafeArray cSrcArray((VARIANT&)cSrc);
        COpcSafeArray cDstArray(cDst);

        UINT uLength = cSrcArray.GetLength();

        cDstArray.Alloc(VT_I4, uLength);

        BSTR* pSrc = (BSTR*)cSrcArray.GetData();
        int*  pDst = (int*)cDstArray.GetData();
        
        UINT ii;
        for (ii = 0; ii < uLength; ii++)
        {
            pDst[ii] = FindEnumIndex(cEnumValues, pSrc[ii]);

            if (pDst[ii] == -1)
            {
                OpcVariantClear(&cDst);
                break;
            }
        }

        if (ii < uLength)
        {
            return OPC_E_BADTYPE;
        }
    }

    // convert from strings to index.
    else
    {
        cDst.vt   = VT_I4;
        cDst.lVal = FindEnumIndex(cEnumValues, cSrc.bstrVal);

        if (cDst.lVal == -1)
        {
            return OPC_E_BADTYPE;
        }    
    }

    return S_OK;
}