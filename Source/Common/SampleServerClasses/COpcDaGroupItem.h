//============================================================================
// TITLE: COpcDaGroupItem.h
//
// CONTENTS:
// 
// A single item in a group in an OPC server.
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
// 2003/07/23 RSA   Fixed problem with update and sample time calculations.
// 2003/08/11 RSA   Added check to ensure errors are only returned once.
// 2003/08/15 RSA   Fixed problem with update and sample time calculations again.

#ifndef _COpcDaGroupItem_H_
#define _COpcDaGroupItem_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaBuffer.h"
#include "COpcDaCache.h"

//============================================================================
// MACROS:  OPC_NO_XXX
// PURPOSE: Defines values to indicate that a property is not specified.

#define OPC_NO_DEADBAND      -1.0
#define OPC_NO_SAMPLING_RATE 0xFFFFFFFF
#define OPC_NO_REQ_TYPE      VT_ILLEGAL

//============================================================================
// CLASS:   COpcDaGroupItem
// PURPOSE: A class that implements the IOPCServer interface.

class COpcDaGroupItem 
{
    OPC_CLASS_NEW_DELETE()

public:

    //=========================================================================
    // Operators

    // Constructor
    COpcDaGroupItem() { Init(); }

    // Destructor 
    ~COpcDaGroupItem() { Clear(); }

    //=========================================================================
    // Public Methods   

	// Init
	void Init();

	// Clear
	void Clear();

	// GetHandle
	OPCHANDLE GetHandle() const { return m_hServer; }

	// GetItemID
	const COpcString& GetItemID() const { return m_cItemID; }
	
    // GetClientHandle
	OPCHANDLE GetClientHandle() const { return m_hClient; }

	// Init
	HRESULT Init(LCID lcid, const OPCITEMDEF& cItem, OPCITEMRESULT& cResult);

	// Clone
	COpcDaGroupItem* Clone();

	// Active
    BOOL GetActive() const { return m_bActive; }
	void SetActive(BOOL bActive) { m_bActive = bActive; }

	// SetClientHandle
	void SetClientHandle(OPCHANDLE hClient) { m_hClient = hClient; }

	// SetReqType
	void SetReqType(VARTYPE vtReqType) { m_vtReqType = vtReqType; }

	// Deadband
	FLOAT GetDeadband() const { return m_fltDeadband; }
	void SetDeadband(FLOAT fltDeadband) { m_fltDeadband = fltDeadband; }

	// SamplingRate
    UINT GetSamplingRate() const { return m_uSamplingRate; }
	void SetSamplingRate(UINT uSamplingRate) { m_uSamplingRate = uSamplingRate; }

	// BufferEnabled
    BOOL GetBufferEnabled() const { return m_bBufferEnabled; }
	void SetBufferEnabled(BOOL bBufferEnabled);

	// GetItemAttributes
	HRESULT GetItemAttributes(OPCITEMATTRIBUTES& cAttributes);

	// Read
	HRESULT Read(
        DWORD         dwSource, 
        LCID          lcid, 
        OPCITEMSTATE& cState
    );

    // Read
    HRESULT Read(
        DWORD     dwMaxAge,
        LCID      lcid, 
        VARIANT&  cValue,
        FILETIME& ftTimestamp,
        WORD&     wQuality
    );

	// Write
	HRESULT Write(LCID lcid, VARIANT& cValue);

    // Write
    HRESULT Write(
        LCID      lcid, 
        VARIANT&  cValue,
        FILETIME* pftTimestamp,
        WORD*     pwQuality
    );

    // Update
    DWORD Update(
        LONGLONG uTick, 
        UINT     uInterval,
        LCID     lcid,
        UINT     uUpdateRate,
        FLOAT    fltDeadband
    );

    // ReadBuffer
    bool ReadBuffer(
        DWORD&        dwIndex,
        OPCITEMSTATE* pItems,
        HRESULT*      pErrors
    );

	// DoSample
	void DoSample(LCID lcid, FLOAT fltDeadband);

	// ResetLastUpdate
	void ResetLastUpdate();

private:

    //=========================================================================
    // Private Methods

    // HasChanged
    bool HasChanged(
        OPCITEMSTATE& cNewValue, 
        OPCITEMSTATE& cOldValue, 
        FLOAT         fltDeadband
    );

    //=========================================================================
    // Private Members

	OPCHANDLE          m_hServer;
	OPCHANDLE          m_hClient;
	COpcString         m_cItemID;
	COpcString         m_cAccessPath;
	BOOL               m_bActive;

	VARTYPE            m_vtReqType;
	FLOAT              m_fltDeadband;
	UINT               m_uSamplingRate;
    BOOL               m_bBufferEnabled;

    OPCITEMSTATE       m_cLatestValue;
	HRESULT            m_hResult;
    COpcDaBuffer       m_cSamples;

    OPCEUTYPE          m_eEUType;
    double             m_dblMinValue;
    double             m_dblMaxValue;
};

//============================================================================
// TYPE:    COpcDaGroupItemMap
// PURPOSE: A table of group items indexed by server handle.

typedef COpcMap<OPCHANDLE,COpcDaGroupItem*> COpcDaGroupItemMap;

#endif // _COpcDaGroupItem_H_