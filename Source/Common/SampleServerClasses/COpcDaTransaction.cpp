//============================================================================
// TITLE: COpcDaTransaction.cpp
//
// CONTENTS:
// 
// Contains all information required to process an asynchronous transaction.
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
// 2003/03/10 RSA   Ensured client assigned transaction ids were are saved.
// 2003/06/25 RSA   Added ChangeType().

#include "StdAfx.h"
#include "COpcDaTransaction.h"

//============================================================================
// Local Variables

static DWORD g_dwLastID = 0;

//============================================================================
// COpcDaTransaction

// Constructor
COpcDaTransaction::COpcDaTransaction(
	DWORD                dwType, 
	IOpcMessageCallback* ipCallback,
	const COpcString&    cName, 
	DWORD                dwTransactionID
)
: 
	COpcMessage(dwType, ipCallback)
{
    Init();   

    cGroupName = cName;
    dwClientID = dwTransactionID;
}

// Init
void COpcDaTransaction::Init()
{
    cGroupName.Empty();

    dwClientID     = NULL;
    dwSource       = OPC_DS_CACHE;
    hMasterError   = S_OK;
    hMasterQuality = S_OK;
    dwCount        = 0;
    pServerHandles = NULL;
    pClientHandles = NULL;
    pMaxAges       = NULL;
    pValueVQTs     = NULL;
    pErrors        = NULL;
    pValues        = NULL;
    pTimestamps    = NULL;
    pQualities     = NULL;
}

// Clear
void COpcDaTransaction::Clear()
{
    if (pValues != NULL)
    {
        for (DWORD ii = 0; ii < dwCount; ii++)
        {
            OpcVariantClear(&pValues[ii]);
        }
    }

    if (pValueVQTs != NULL)
    {
        for (DWORD ii = 0; ii < dwCount; ii++)
        {
            OpcVariantClear(&pValueVQTs[ii].vDataValue);
        }
    }

    OpcFree(pServerHandles);
    OpcFree(pClientHandles);
    OpcFree(pMaxAges);
    OpcFree(pValueVQTs);
    OpcFree(pErrors);
    OpcFree(pValues);
    OpcFree(pTimestamps);
    OpcFree(pQualities);

    Init();
}

// SetItemStates
void COpcDaTransaction::SetItemStates(OPCITEMSTATE* pStates)
{
    hMasterQuality = S_OK;

    pClientHandles = OpcArrayAlloc(OPCHANDLE, dwCount);
    pValues        = OpcArrayAlloc(VARIANT, dwCount);
    pTimestamps    = OpcArrayAlloc(FILETIME, dwCount);
    pQualities     = OpcArrayAlloc(WORD, dwCount);

    for (DWORD ii = 0; ii < dwCount; ii++)
    {
        pClientHandles[ii] = pStates[ii].hClient;
        pTimestamps[ii]    = pStates[ii].ftTimeStamp;
        pQualities[ii]     = pStates[ii].wQuality;

        memcpy(&(pValues[ii]), &(pStates[ii].vDataValue), sizeof(VARIANT));
        memset(&(pStates[ii].vDataValue), 0, sizeof(VARIANT));

        if (pQualities[ii] != OPC_QUALITY_GOOD)
        {
            hMasterQuality = S_FALSE;
        }
    }
}

// SetItemErrors
void COpcDaTransaction::SetItemErrors(HRESULT* pNewErrors)
{
    pErrors = OpcArrayAlloc(HRESULT, dwCount);
    memcpy(pErrors, pNewErrors, dwCount*sizeof(HRESULT));

    hMasterError = S_OK;

    for (DWORD ii = 0; ii < dwCount; ii++)
    {        
        if (pErrors[ii] != S_OK)
        {
            hMasterError = S_FALSE;
            break;
        }
    }
}

// ChangeType
void COpcDaTransaction::ChangeType(DWORD dwType)
{
	m_uType = dwType;
}