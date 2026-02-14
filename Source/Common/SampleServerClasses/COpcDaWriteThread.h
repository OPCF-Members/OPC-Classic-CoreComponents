//============================================================================
// TITLE: COpcDaWriteThread.h
//
// CONTENTS:
// 
// A thread that serializes asynchronous write requests.
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
// 2003/06/20 RSA   Initial implementation.

#ifndef _COpcDaWriteThread_H_
#define _COpcDaWriteThread_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaTransaction.h"

//============================================================================
// CLASS:   COpcDaWriteThread
// PURPOSE: Maintains an in memory cache of DA items.

class COpcDaWriteThread : public COpcSynchObject
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcDaWriteThread();

    // Destructor
    ~COpcDaWriteThread();
    
	//=========================================================================
    // Public Methods

	// Start
	bool Start();
	
	// Stop
	void Stop();
    
	// Run
    void Run();

	// QueueTransaction
	bool QueueTransaction(COpcDaTransaction* pTransaction);

private:

    //========================================================================
    // Private Members

	DWORD  m_dwID;
	HANDLE m_hEvent;
};

#endif // _COpcDaWriteThread_H_