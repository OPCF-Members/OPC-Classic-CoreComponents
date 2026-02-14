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
