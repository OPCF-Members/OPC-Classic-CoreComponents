//============================================================================
// TITLE: COpcDaTransaction.h
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