/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

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
