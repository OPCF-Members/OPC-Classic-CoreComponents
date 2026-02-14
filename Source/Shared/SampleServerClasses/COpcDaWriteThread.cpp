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
#include "COpcDaWriteThread.h"

#include "COpcDaCache.h"

//==============================================================================
// Local Functions

static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    ((COpcDaWriteThread*)lpParameter)->Run();
	return 0;
}

//============================================================================
// COpcDaWriteThread

// Constructor
COpcDaWriteThread::COpcDaWriteThread()
{
	m_dwID   = NULL;
	m_hEvent = NULL;
}

// Destructor
COpcDaWriteThread::~COpcDaWriteThread()
{
}

// Start
bool COpcDaWriteThread::Start()
{
	COpcLock cLock(*this);

	// check that the thread is not already running.
	if (m_dwID != NULL)
	{
		return false;
	}

	// create event to wait for thread.
	m_hEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL
	);

	if (m_hEvent == NULL)
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

	cLock.Unlock();

	// wait for update thread to start.
	WaitForSingleObject(m_hEvent, INFINITE);

	cLock.Lock();
	ResetEvent(m_hEvent);

	return true;
}

// Stop
void COpcDaWriteThread::Stop()
{
	COpcLock cLock(*this);

	if (m_dwID != NULL)
	{
		// post a quit message.
		PostThreadMessage(m_dwID, WM_QUIT, NULL, NULL);

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

// Run
void COpcDaWriteThread::Run()
{	
	COpcLock cLock(*this);

	// create the message queue.
	MSG cMsg; memset(&cMsg, 0, sizeof(MSG));
	PeekMessage(&cMsg, NULL, NULL, NULL, PM_NOREMOVE);

	// signal startup.
	SetEvent(m_hEvent);

	cLock.Unlock();

	// enter the message loop.
	while (GetMessage(&cMsg, NULL, NULL, NULL))
	{
		// process message.
		if (cMsg.message == OPC_TRANSACTION_WRITE)
		{
			// process write message.
			COpcDaTransaction* pTransaction = (COpcDaTransaction*)cMsg.wParam;
			pTransaction->Process();

			// queue response to client.
			pTransaction->ChangeType(OPC_TRANSACTION_WRITE_COMPLETE);
			::GetCache().QueueMessage(pTransaction);
		}
	}

	cLock.Lock();

	// signal shutdown.
	SetEvent(m_hEvent);
}

// QueueTransaction
bool COpcDaWriteThread::QueueTransaction(COpcDaTransaction* pTransaction)
{
	COpcLock cLock(*this);

	if (!PostThreadMessage(m_dwID, OPC_TRANSACTION_WRITE, (WPARAM)pTransaction, NULL))
	{
		return false;
	}

	return true;
}
