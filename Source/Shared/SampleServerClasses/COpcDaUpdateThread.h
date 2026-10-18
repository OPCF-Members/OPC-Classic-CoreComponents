/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcDaUpdateThread_H_
#define _COpcDaUpdateThread_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class COpcDaGroup;

#define MAX_UPDATE_RATE 100

//============================================================================
// CLASS:   COpcDaUpdateThread
// PURPOSE: Maintains an in memory cache of DA items.

class COpcDaUpdateThread : public COpcSynchObject
{
    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Public Operators

    // Constructor
    COpcDaUpdateThread();

    // Destructor
    ~COpcDaUpdateThread();
    
	//=========================================================================
    // Public Methods

	// Start
	bool Start();
	
	// Stop
	void Stop();
    
	// Update
    void Run();

	// Register
	void Register(COpcDaGroup* pGroup);

	// Unregister
	void Unregister(COpcDaGroup* pGroup);

private:

    //========================================================================
    // Private Members

	DWORD                  m_dwID;
	HANDLE                 m_hEvent;
	COpcList<COpcDaGroup*> m_cGroups;
};

#endif // _COpcDaUpdateThread_H_
