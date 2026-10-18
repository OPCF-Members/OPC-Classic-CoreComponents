/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

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
