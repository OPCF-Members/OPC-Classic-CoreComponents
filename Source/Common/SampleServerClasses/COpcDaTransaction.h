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

#ifndef _COpcDaTransaction_H_
#define _COpcDaTransaction_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//============================================================================
// MACROS:  OPC_TRANSACTION_XXX
// PURPOSE: Defines unique ids for transaction types.

#define OPC_TRANSACTION_READ           WM_APP+0x01
#define OPC_TRANSACTION_WRITE          WM_APP+0x02
#define OPC_TRANSACTION_WRITE_COMPLETE WM_APP+0x03
#define OPC_TRANSACTION_REFRESH        WM_APP+0x04
#define OPC_TRANSACTION_UPDATE         WM_APP+0x05
#define OPC_TRANSACTION_CANCEL         WM_APP+0x06

//============================================================================
// CLASS:   COpcDaTransaction
// PURPOSE: Stores details for an asynchronous transaction.

class COpcDaTransaction : public COpcMessage
{
    OPC_CLASS_NEW_DELETE()

public:

    //=========================================================================
    // Public Properties

    COpcString    cGroupName;
    DWORD         dwClientID;
    OPCDATASOURCE dwSource;
    HRESULT       hMasterError;
    HRESULT       hMasterQuality;
    
    DWORD         dwCount;
    
    OPCHANDLE*    pServerHandles;
    OPCHANDLE*    pClientHandles;

    DWORD*        pMaxAges;
    OPCITEMVQT*   pValueVQTs;

    VARIANT*      pValues;
    FILETIME*     pTimestamps;
    WORD*         pQualities;
    HRESULT*      pErrors;
    
    //=========================================================================
    // Public Operators

    // Constructor
	COpcDaTransaction(
		DWORD                dwType, 
		IOpcMessageCallback* ipCallback,
		const COpcString&    cName, 
		DWORD                dwTransactionID
	);

    // Destructor 
    ~COpcDaTransaction() { Clear(); }
    
    //=========================================================================
    // Public Methods

	// Init
	void Init();

	// Clear()
	void Clear();

    // SetItemStates
    void SetItemStates(OPCITEMSTATE* pStates);

    // SetItemErrors
    void SetItemErrors(HRESULT* pErrors);
	
    // ChangeType
    void ChangeType(DWORD dwType);
};

//============================================================================
// TYPE:    COpcDaTransactionQueue
// PURPOSE: A queue of transactions.

typedef COpcList<COpcDaTransaction*> COpcDaTransactionQueue;

#endif // _COpcDaTransaction_H_
