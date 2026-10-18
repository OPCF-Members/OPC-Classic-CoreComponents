/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _IOpcDaDevice_H_
#define _IOpcDaDevice_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaProperty.h"

//============================================================================
// INTERFACE: IOpcDaDevice
// PURPOSE:   Abstract interface to basic device I/O functions.

interface IOpcDaDevice
{
	// BuildAddressSpace
	virtual bool BuildAddressSpace() = 0;

	// ClearAddressSpace
	virtual void ClearAddressSpace() = 0;

	// IsKnownItem
	virtual bool IsKnownItem(const COpcString& cItemID) = 0;

	// GetAvailableProperties
    virtual HRESULT GetAvailableProperties(
        const COpcString&   cItemID, 
		bool                bReturnValues,
		COpcDaPropertyList& cProperties) = 0;

	// GetAvailableProperties
    virtual HRESULT GetAvailableProperties(
        const COpcString&      cItemID, 
		const COpcList<DWORD>& cIDs,
		bool                   bReturnValues,
		COpcDaPropertyList&    cProperties) = 0;

	// Read
	virtual HRESULT Read(
        const COpcString& cItemID, 
        DWORD             dwPropertyID,
        VARIANT&          cValue, 
        FILETIME*         pftTimestamp = NULL,
        WORD*             pwQuality = NULL) = 0;

	// Write
	virtual HRESULT Write(
        const COpcString& cItemID, 
        DWORD             dwPropertyID,
        const VARIANT&    cValue, 
        FILETIME*         pftTimestamp = NULL,
        WORD*             pwQuality = NULL) = 0;
};

//============================================================================
// FUNCTION: GetDevice
// PURPOSE:  Returns a pointer (must never be null) to the primary device.

extern IOpcDaDevice* GetDevice(const COpcString& cItemID);

#endif // _IOpcDaDevice_H_
