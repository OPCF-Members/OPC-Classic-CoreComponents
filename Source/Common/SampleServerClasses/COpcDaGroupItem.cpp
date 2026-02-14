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
#include "COpcDaGroupItem.h"
#include "COpcDaCache.h"

//============================================================================
// Local Declarations

#define OPC_DEVICE_READ_NEVER  0xFFFFFFFF
#define OPC_DEVICE_READ_ALWAYS 0x00000000

//============================================================================
// COpcDaGroupItem

// Init
void COpcDaGroupItem::Init()
{
	m_hServer        = (OPCHANDLE)(ULONG_PTR)(COpcDaGroupItem*)this;
	m_hClient        = NULL;
	m_cItemID        = OPC_EMPTY_STRING;
	m_cAccessPath    = OPC_EMPTY_STRING;
	m_bActive        = FALSE;
	m_vtReqType      = OPC_NO_REQ_TYPE;
	m_fltDeadband    = OPC_NO_DEADBAND;
	m_uSamplingRate  = OPC_NO_SAMPLING_RATE;
    m_bBufferEnabled = FALSE;
    m_eEUType        = OPC_NOENUM;
    m_dblMinValue    = 0;
    m_dblMaxValue    = 0;
	m_hResult        = S_OK;

    memset(&m_cLatestValue, 0, sizeof(m_cLatestValue));
}

// Clear
void COpcDaGroupItem::Clear()
{
    OpcVariantClear(&m_cLatestValue.vDataValue);

	Init();
}

// Clone
COpcDaGroupItem* COpcDaGroupItem::Clone()
{
	COpcDaGroupItem* pItem = new COpcDaGroupItem();

	pItem->m_cItemID        = m_cItemID;
	pItem->m_cAccessPath    = m_cAccessPath;
    pItem->m_eEUType        = m_eEUType;
    pItem->m_dblMinValue    = m_dblMinValue;
    pItem->m_dblMaxValue    = m_dblMaxValue;

    pItem->SetClientHandle(m_hClient);
    pItem->SetActive(m_bActive);
    pItem->SetReqType(m_vtReqType);
    pItem->SetDeadband(m_fltDeadband);
    pItem->SetSamplingRate(m_uSamplingRate);
    pItem->SetBufferEnabled(m_bBufferEnabled);

	return pItem;
}    

// Init
HRESULT COpcDaGroupItem::Init(LCID lcid, const OPCITEMDEF& cItem, OPCITEMRESULT& cResult)
{
    Init();

	memset(&cResult, 0, sizeof(OPCITEMRESULT));

    m_hClient        = cItem.hClient;
	m_cItemID        = cItem.szItemID;
	m_cAccessPath    = cItem.szAccessPath;
	m_bActive        = cItem.bActive;
	m_vtReqType      = cItem.vtRequestedDataType;

    // fill in item result structure.
	HRESULT hResult = ::GetCache().GetItemResult(m_cItemID, cResult);
	
    cResult.hServer = m_hServer;

    // check data conversion - conversions between scalars and arrays not supported.
    if (m_vtReqType != VT_EMPTY && m_vtReqType != VT_BSTR)
    {
        if ((m_vtReqType & VT_ARRAY) != (cResult.vtCanonicalDataType & VT_ARRAY))
        {
            return OPC_E_BADTYPE;
        }
    }

    // look up EU type.
    VARIANT cProperty; OpcVariantInit(&cProperty);

    hResult = ::GetCache().GetItemProperty(
        m_cItemID, 
        OPC_PROPERTY_EU_TYPE, 
        cProperty);

    if (SUCCEEDED(hResult))
    {
        m_eEUType = (OPCEUTYPE)cProperty.lVal;
    }

    // look up EU range.
    if (m_eEUType == OPC_ANALOG)
    {
        hResult = ::GetCache().GetItemProperty(
            m_cItemID, 
            OPC_PROPERTY_HIGH_EU, 
            cProperty);

        if (FAILED(hResult))
        {
            m_eEUType = OPC_NOENUM;
        }

        m_dblMaxValue = cProperty.dblVal;

        hResult = ::GetCache().GetItemProperty(
            m_cItemID, 
            OPC_PROPERTY_LOW_EU, 
            cProperty);

        if (FAILED(hResult))
        {
            m_eEUType = OPC_NOENUM;
        }

        m_dblMinValue = cProperty.dblVal;
    }


	return S_OK;
}

// GetItemAttributes
HRESULT COpcDaGroupItem::GetItemAttributes(OPCITEMATTRIBUTES& cAttributes)
{
	memset(&cAttributes, 0, sizeof(OPCITEMATTRIBUTES));

	HRESULT hResult = ::GetCache().GetItemAttributes(m_cItemID, cAttributes);
	
	cAttributes.szAccessPath        = OpcStrDup((LPCWSTR)m_cAccessPath);
	cAttributes.szItemID            = OpcStrDup((LPCWSTR)m_cItemID);
	cAttributes.hClient             = m_hClient;
	cAttributes.hServer             = m_hServer;
    cAttributes.bActive             = m_bActive;
	cAttributes.vtRequestedDataType = m_vtReqType;

	return hResult;
}

// Read
HRESULT COpcDaGroupItem::Read(DWORD dwSource, LCID lcid, OPCITEMSTATE& cState)
{
    // read current value.
	cState.hClient = m_hClient;

	HRESULT hResult = ::GetCache().Read(
        m_cItemID, 
        lcid, 
        m_vtReqType, 
        (dwSource == OPC_DS_CACHE)?OPC_DEVICE_READ_NEVER:OPC_DEVICE_READ_ALWAYS,
        cState.vDataValue,
        cState.ftTimeStamp,
        cState.wQuality
    );    

    if (FAILED(hResult))
    {
        return hResult;
    }

    // override quality if item is not active.
    if (!m_bActive && dwSource == OPC_DS_CACHE)
    {
        cState.wQuality = OPC_QUALITY_OUT_OF_SERVICE;
    }
	
	return hResult;
}

// Read
HRESULT COpcDaGroupItem::Read(
    DWORD     dwMaxAge,
    LCID      lcid, 
    VARIANT&  cValue,
    FILETIME& ftTimestamp,
    WORD&     wQuality
)
{
    // read current value.
	HRESULT hResult = ::GetCache().Read(
        m_cItemID, 
        lcid, 
        m_vtReqType, 
        dwMaxAge,
        cValue,
        ftTimestamp,
        wQuality
    );    

    return hResult;
}

// Write
HRESULT COpcDaGroupItem::Write(LCID lcid, VARIANT& cValue)
{
	return ::GetCache().Write(m_cItemID, lcid, cValue);
}

// Write
HRESULT COpcDaGroupItem::Write(
    LCID      lcid, 
    VARIANT&  cValue,
    FILETIME* pftTimestamp,
    WORD*     pwQuality
)
{
	HRESULT hResult = ::GetCache().Write(
        m_cItemID,
        lcid,
        cValue,
        pftTimestamp,
        pwQuality);

    return hResult;
}

// ResetLastUpdate
void COpcDaGroupItem::ResetLastUpdate()
{
    OpcVariantClear(&m_cLatestValue.vDataValue);
    memset(&m_cLatestValue, 0, sizeof(m_cLatestValue));
}

// Update
DWORD COpcDaGroupItem::Update(
    LONGLONG      uTick, 
    UINT          uInterval,
    LCID          lcid,
    UINT          uUpdateRate,
    FLOAT         fltDeadband
)
{
	// inactive item do not update.
	if (!m_bActive) return false;

    bool bDoSample = true;
    bool bDoUpdate = true;

    // determine if it is time for a group update.
	if (uUpdateRate/uInterval > 1) 
    { 
    	bDoUpdate = (uTick%(uUpdateRate/uInterval) == 0);
    }

    // use the item or group sampling rate to determine the item update rate,
    UINT uSamplingRate = (m_uSamplingRate != OPC_NO_SAMPLING_RATE)?m_uSamplingRate:uUpdateRate;

	// update value only if the update rate has elapsed since last update.
	if (uSamplingRate/uInterval > 1) 
    { 
    	bDoSample = (uTick%(uSamplingRate/uInterval) == 0);
    }

    // check if there is anything to do.
    if (!bDoSample && !bDoUpdate)
    {
        return 0;
    }

    // read next sample.
    if (bDoSample)
    {
		DoSample(lcid, fltDeadband);
    }

    // return number of samples available for return.
    if (bDoUpdate)
    {
		return m_cSamples.GetCount();
    }

    return 0;
}

// DoSample
void COpcDaGroupItem::DoSample(LCID lcid, FLOAT fltDeadband)
{
	if (!m_bActive) return;

	// read from the device and update the cache.
    OPCITEMSTATE cCurrentValue;
    memset(&cCurrentValue, 0, sizeof(OPCITEMSTATE));
    cCurrentValue.hClient = m_hClient;

    HRESULT hResult = ::GetCache().Read(
        m_cItemID, 
        lcid, 
        m_vtReqType, 
        OPC_DEVICE_READ_ALWAYS,
        cCurrentValue.vDataValue,
        cCurrentValue.ftTimeStamp,
        cCurrentValue.wQuality
    );    

    // check if value has changed.
    bool bChanged = true;

    // always return for error.
    if (m_hResult != hResult)
    {
		m_hResult = hResult;
    }

    // compare value to last returned value if sampling has not started yet.
    else if (m_cSamples.GetCount() == 0)
    {
        bChanged = HasChanged(cCurrentValue, m_cLatestValue, fltDeadband);
    }

    // compare to previous sample if sampling has started.
    else if (m_bBufferEnabled)
    {
        bChanged = HasChanged(cCurrentValue, m_cSamples[m_cSamples.GetCount()-1], 0);
    }

    // add changed value to the buffer.
    if (bChanged)
    {
        if (!m_bBufferEnabled)
        {
            m_cSamples.Clear();
        }

        m_cSamples.Append(cCurrentValue, hResult);
    }
    else
    {
        OpcVariantClear(&cCurrentValue.vDataValue);
    }
}

// ReadBuffer
bool COpcDaGroupItem::ReadBuffer(
    DWORD&        dwIndex,
    OPCITEMSTATE* pItems,
    HRESULT*      pErrors
)
{
    // return contents of sample buffer.
    DWORD dwCount = m_cSamples.GetCount();

    if (dwCount == 0)
    {
        return false;
    }

    // save latest returned value.
    OpcVariantClear(&m_cLatestValue.vDataValue);

    m_cLatestValue.ftTimeStamp = m_cSamples[dwCount-1].ftTimeStamp;
    m_cLatestValue.wQuality    = m_cSamples[dwCount-1].wQuality;

    OpcVariantCopy(&m_cLatestValue.vDataValue, &m_cSamples[dwCount-1].vDataValue);

    // copy buffer into return array.
    bool bOverflow = m_cSamples.GetOverflow();

    for (DWORD ii = 0; ii < dwCount; ii++)
    {
        memcpy(&(pItems[dwIndex]), &(m_cSamples[ii]), sizeof(OPCITEMSTATE));
        memset(&(m_cSamples[ii]), 0, sizeof(OPCITEMSTATE));

        pErrors[dwIndex] = m_cSamples.Error(ii);

        if (SUCCEEDED(pErrors[dwIndex]) && m_cSamples.GetOverflow())
        {
            pErrors[dwIndex] = OPC_S_DATAQUEUEOVERFLOW;
        }

        dwIndex++;
    }

    m_cSamples.Clear();

    return bOverflow;
}

// SetBufferEnabled
void COpcDaGroupItem::SetBufferEnabled(BOOL bBufferEnabled)
{ 
    if (m_bBufferEnabled == bBufferEnabled)
    {
        return;
    }

    m_bBufferEnabled = bBufferEnabled;

    if (m_bBufferEnabled)
    {
        m_cSamples.Alloc(OPC_MAX_BUF_SIZE);
    }
    else
    {
        m_cSamples.Alloc(1);
    }
}

// HasChanged
bool COpcDaGroupItem::HasChanged(
    OPCITEMSTATE& cNewValue, 
    OPCITEMSTATE& cOldValue, 
    FLOAT         fltDeadband)
{
    // check if the quality has changed.
    if (cNewValue.wQuality != cOldValue.wQuality)
    {
        return true;
    }

    // check if it has changed at all.
    if (COpcVariant::IsEqual(cNewValue.vDataValue, cOldValue.vDataValue))
    {
        return false;
    }

    // check deadband if required.
    double dblDeadband = ((m_fltDeadband != OPC_NO_DEADBAND)?m_fltDeadband:fltDeadband)/100;

    if (m_eEUType != OPC_ANALOG || dblDeadband == 0)
    {
        return true;
    }

    // check for trival case.
    double dblRange = m_dblMaxValue - m_dblMinValue;

    if (dblRange == 0)
    {
        return true;
    }

    HRESULT hResult = S_OK;

    COpcVariant cOldDouble;
    COpcVariant cNewDouble;

    // convert to array of doubles.
    if ((cOldValue.vDataValue.vt & VT_ARRAY) != 0)
    {
        hResult = VariantChangeType(&cNewDouble.GetRef(), &cNewValue.vDataValue, NULL, VT_ARRAY | VT_R8);

        if (FAILED(hResult))
        {
            return true;
        }

        hResult = VariantChangeType(&cOldDouble.GetRef(), &cOldValue.vDataValue, NULL, VT_ARRAY | VT_R8);

        if (FAILED(hResult))
        {
            return true;
        }

        COpcSafeArray cNewArray(cNewDouble.GetRef());
        COpcSafeArray cOldArray(cOldDouble.GetRef());

        if (cNewArray.GetLength() != cOldArray.GetLength())
        {
            return true;
        }

        bool bChanged = false;

        cNewArray.Lock();
        cOldArray.Lock();

        double* pNewData = (double*)cNewArray.GetData();
        double* pOldData = (double*)cOldArray.GetData();

        UINT uLength = cNewArray.GetLength();

        for (UINT ii = 0; ii < uLength; ii++)
        {
            double dblDelta = (pNewData[ii] - pOldData[ii])/dblRange;

            dblDelta = (dblDelta < 0)?-dblDelta:dblDelta;

            if (dblDelta >= dblDeadband)
            {
                bChanged = true;
                break;
            }
        }

        cNewArray.Unlock();
        cOldArray.Unlock();

        return bChanged;
    }

    hResult = VariantChangeType(&cNewDouble.GetRef(), &cNewValue.vDataValue, NULL, VT_R8);

    if (FAILED(hResult))
    {
        return true;
    }

    hResult = VariantChangeType(&cOldDouble.GetRef(), &cOldValue.vDataValue, NULL, VT_R8);

    if (FAILED(hResult))
    {
        return true;
    }

    double dblDelta = ((double)cNewDouble - (double)cOldDouble)/dblRange;

    dblDelta = (dblDelta < 0)?-dblDelta:dblDelta;

    return (dblDelta >= dblDeadband);
}
