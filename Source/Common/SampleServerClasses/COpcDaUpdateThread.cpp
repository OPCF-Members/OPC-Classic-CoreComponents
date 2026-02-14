//============================================================================
// TITLE: COpcDaUpdateThread.cpp
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

#include "StdAfx.h"
#include "COpcDaGroup.h"

//==============================================================================
// Local Functions

static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    ((COpcDaUpdateThread*)lpParameter)->Run();
	return 0;
}

//============================================================================
// COpcDaUpdateThread

// Constructor
COpcDaUpdateThread::COpcDaUpdateThread()
{
	m_dwID   = NULL;
	m_hEvent = NULL;
}

// Destructor
COpcDaUpdateThread::~COpcDaUpdateThread()
{
	OPC_ASSERT(m_cGroups.GetCount() == 0);
}

// Start
bool COpcDaUpdateThread::Start()
{
	COpcLock cLock(*this);

	// check that the thread is not already running.
	if (m_dwID != NULL)
	{
		return false;
	}

	// start thread.
	HANDLE hThread = CreateThread(
		NULL,
		NULL,
		ThreadProc,
		(void*)this,
		NULL,
		&m_dwID);

	if (hThread == NULL)
	{
		return false;
	}

	// close thread handle.
	CloseHandle(hThread);
	return true;
}

// Stop
void COpcDaUpdateThread::Stop()
{
	COpcLock cLock(*this);

	// remove all groups.
	while (m_cGroups.GetCount() > 0)
	{
		COpcDaGroup* pGroup = m_cGroups.RemoveHead();
		((IOPCItemMgt*)pGroup)->Release();
	}

	if (m_dwID != NULL)
	{
		// create event to wait for shutdown.
		m_hEvent = CreateEvent(
			NULL,
			TRUE,
			FALSE,
			NULL
		);

		// clear thread id to signal shutdown.
		m_dwID = NULL;

		cLock.Unlock();

		// wait for update thread to shutdown.
		WaitForSingleObject(m_hEvent, INFINITE);

		cLock.Lock();
		
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

// Update
void COpcDaUpdateThread::Run()
{
	LONGLONG llEnd   = 0;
	LONGLONG llStart = 0;
	LONGLONG llFreq  = 0;
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&llFreq);

	LONGLONG uTicks = 0;

	do 
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&llEnd);

		UINT uWaitTime = MAX_UPDATE_RATE;

		if (llStart != 0)
		{
			double delay = (((double)(llEnd - llStart))/((double)llFreq))*1000 - MAX_UPDATE_RATE;

			if (delay > 0)
			{
				uWaitTime = (delay < uWaitTime)?(uWaitTime - (UINT)delay):0;
			}
		}

		QueryPerformanceCounter((LARGE_INTEGER*)&llStart);

		// wait until next update.
		Sleep(uWaitTime);

		COpcLock cLock(*this);

		// check for shutdown.
		if (m_dwID != GetCurrentThreadId())
		{
			SetEvent(m_hEvent);
			break;
		}

		// call update on all groups.
		OPC_POS pos = m_cGroups.GetHeadPosition();

		while (pos != NULL)
		{
			COpcDaGroup* pGroup = m_cGroups.GetNext(pos);
			pGroup->Update(uTicks, MAX_UPDATE_RATE);
		}
	
		// increment counter.
		uTicks++;

		cLock.Unlock();
	}
	while (true);
}

// Register
void COpcDaUpdateThread::Register(COpcDaGroup* pGroup)
{
	COpcLock cLock(*this);

	// check for duplicate group.
	OPC_POS pos = m_cGroups.GetHeadPosition();

	while (pos != NULL)
	{
		COpcDaGroup* pCurrent = m_cGroups.GetNext(pos);

		if (pGroup == pCurrent)
		{
			return;
		}
	}

	// add reference to group.
	m_cGroups.AddTail(pGroup);
	((IOPCItemMgt*)pGroup)->AddRef();
}

// Unregister
void COpcDaUpdateThread::Unregister(COpcDaGroup* pGroup)
{
	COpcLock cLock(*this);

	// find group in list.
	OPC_POS pos = m_cGroups.GetHeadPosition();

	while (pos != NULL)
	{
		COpcDaGroup* pCurrent = m_cGroups[pos];

		if (pGroup == pCurrent)
		{
			// remove reference to group.
			m_cGroups.RemoveAt(pos);
			((IOPCItemMgt*)pCurrent)->Release();
			return;
		}

		m_cGroups.GetNext(pos);
	}
}