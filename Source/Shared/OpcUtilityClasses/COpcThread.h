//==============================================================================
// TITLE: COpcThread.h
//
// CONTENTS:
// 
// A class that manages startup and shutdown of a thread.
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

#ifndef _COpcThread_H_
#define _COpcThread_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcString.h"

//==============================================================================
// TYPEDEF: PfnOpcThreadControl
// PURPOSE: Pointer to a function that controls a thread.

typedef void (WINAPI *FnOpcThreadControl)(void* pData, bool bStopThread);
typedef FnOpcThreadControl PfnOpcThreadControl;

//==============================================================================
// CLASS:   COpcThread
// PURPOSE: Manages startup and shutdown of a thread.

class OPCUTILS_API COpcThread
{
    OPC_CLASS_NEW_DELETE()

public:

    //==========================================================================
    // Public Operators

    // Constructor
    COpcThread();

    // Destructor
    ~COpcThread();

    //==========================================================================
    // Public Methods

    // Start
    bool Start(
        PfnOpcThreadControl pfnStartProc, 
        void*               pData, 
        DWORD               dwTimeout = INFINITE,
		int                 iPriority = THREAD_PRIORITY_NORMAL);

    // Stop
    void Stop(DWORD dwTimeout = INFINITE);

    // WaitingForStop
    bool WaitingForStop() { return m_bWaitingForStop; }

    // Run
    DWORD Run();

    // PostMessage
    bool PostMessage(UINT uMsgID, WPARAM wParam, LPARAM lParam);

private:

    //==========================================================================
    // Private Members

    DWORD               m_dwID;
    HANDLE              m_hThread;
    HANDLE              m_hEvent;
    bool                m_bWaitingForStop;

    PfnOpcThreadControl m_pfnControl;
    void*               m_pData;
};

#endif // _COpcThread_H_