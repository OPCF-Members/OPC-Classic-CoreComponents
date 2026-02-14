//==============================================================================
// TITLE: COpcCriticalSection.h
//
// CONTENTS:
// 
// A wrapper for a critical section.
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
//

#ifndef _COpcCriticalSection_H_
#define _COpcCriticalSection_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"

//==============================================================================
// TITLE:   COpcCriticalSection.h
// PURPOSE: Implements a wrapper for a critical section.
// NOTES:

class OPCUTILS_API COpcCriticalSection
{
    OPC_CLASS_NEW_DELETE();

public:

    //==========================================================================
    // Operators

    // Constructor
    inline COpcCriticalSection()
    {
        m_ulLocks  = 0;
        m_dwThread = -1;

        InitializeCriticalSection(&m_csLock);
    }

    // Destructor
    inline ~COpcCriticalSection()
    {
        DeleteCriticalSection(&m_csLock);
    }

    //==========================================================================
    // Public Methods

    // Lock
    inline void Lock()
    {
        EnterCriticalSection(&m_csLock);

        if (m_dwThread == -1)
        {
            m_dwThread = GetCurrentThreadId();
        }

        OPC_ASSERT(m_dwThread == GetCurrentThreadId());

        m_ulLocks++;
    }

    // Unlock
    inline void Unlock()
    {
        OPC_ASSERT(m_dwThread == GetCurrentThreadId());
        OPC_ASSERT(m_ulLocks > 0);

        m_ulLocks--;

        if (m_ulLocks == 0)
        {
            m_dwThread = -1;
        }

        LeaveCriticalSection(&m_csLock);
    }

    // HasLock
    inline bool HasLock()
	{
		return (m_dwThread == GetCurrentThreadId());
	}

private: 
   
   //===========================================================================
   // Private Members

   CRITICAL_SECTION m_csLock;
   DWORD            m_dwThread;
   ULONG            m_ulLocks;
};

//==============================================================================
// TITLE:   COpcLock.h
// PURPOSE: Implements a class that leaves a critical section when destroyed.
// NOTES:

class COpcLock
{
public:

    //==========================================================================
    // Operators

    // Constructor
    inline COpcLock(const COpcCriticalSection& cLock)
    :
        m_pLock(NULL)
    {
        m_pLock = (COpcCriticalSection*)&cLock;
        m_pLock->Lock();
        m_uLocks = 1;
    }

    // Destructor
    inline ~COpcLock()
    {
        while (m_uLocks > 0) Unlock();
    }

    //==========================================================================
    // Public Methods

    inline void Unlock()
    {
        OPC_ASSERT(m_uLocks > 0);

        m_uLocks--;

        if (m_uLocks == 0)
        {
            m_pLock->Unlock();
        }
    }

    inline void Lock()
    {
        if (m_uLocks == 0)
        {
            m_pLock->Lock();
        }

        m_uLocks++;
    }

private:

    UINT                 m_uLocks;
    COpcCriticalSection* m_pLock;       
};

//==============================================================================
// TITLE:   COpcSynchObject.h
// PURPOSE: A base class that adds a critical section to a class.

class COpcSynchObject
{

public:

    // Cast
    operator COpcCriticalSection&() { return m_cLock; }
    operator const COpcCriticalSection&() const { return m_cLock; }

	// HasLock
	bool HasLock() { return m_cLock.HasLock(); }

private:

    COpcCriticalSection m_cLock;
};

#endif // _COpcCriticalSection_H_