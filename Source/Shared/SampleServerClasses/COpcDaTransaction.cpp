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
