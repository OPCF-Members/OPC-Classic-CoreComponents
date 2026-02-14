//============================================================================
// TITLE: COpcDaUpdateThread.h
//
// CONTENTS:
// 
// Periodically updates all registered (a.k.a active) groups.
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
// 2002/12/03 RSA   First release.
//

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