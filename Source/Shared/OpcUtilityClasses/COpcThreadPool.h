//==============================================================================
// TITLE: COpcThreadPool.h
//
// CONTENTS:
// 
// Manages a pool of threads that process queued messages.
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
// 2002/12/02 RSA   First release.
//

#ifndef _COpcThreadPool_H_
#define _COpcThreadPool_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcList.h"
#include "COpcCriticalSection.h"

class COpcMessage;

//==============================================================================
// INTERFACE: IOpcMessageCallback
// PURPOSE:   A interface to an object that processes messages.

interface IOpcMessageCallback : public IUnknown
{
	// ProcessMessage
	virtual void ProcessMessage(COpcMessage& cMsg) = 0;
};

//==============================================================================
// CLASS:   COpcMessage
// PURPOSE: A base class for a message.

class OPCUTILS_API COpcMessage
{
    OPC_CLASS_NEW_DELETE();

public:

    //==========================================================================
    // Public Operators

    // Constructor
    COpcMessage(UINT uType, IOpcMessageCallback* ipCallback);

    // Copy Constructor
    COpcMessage(const COpcMessage& cMessage);

	// Destructor
    virtual ~COpcMessage();

    //==========================================================================
    // Public Methods

	// Process
	virtual void Process()
	{
		if (m_ipCallback != NULL)
		{
			m_ipCallback->ProcessMessage(*this);
		}
	}

	// GetID
	UINT GetID() { return m_uID; }

	// GetType
	UINT GetType() { return m_uType; }

protected:

    //==========================================================================
    // Protected Operators

	UINT                 m_uID;
	UINT                 m_uType;
	IOpcMessageCallback* m_ipCallback;
};

//==============================================================================
// CLASS:   COpcThreadPool
// PURPOSE: Manages a pool of threads that process queued messages.

class OPCUTILS_API COpcThreadPool : public COpcSynchObject
{
    OPC_CLASS_NEW_DELETE();

public:

    //==========================================================================
    // Public Operators

    // Constructor
    COpcThreadPool();

	// Destructor
    ~COpcThreadPool();

    //==========================================================================
    // Public Methods
     
	// Start
	bool Start();

	// Stop
	void Stop();

	// Run
	void Run();

    // QueueMessage
	bool QueueMessage(COpcMessage* pMsg);

	// SetSize
	void SetSize(UINT uMinThreads, UINT uMaxThreads);

private:

    //==========================================================================
    // Private Members

	HANDLE                  m_hEvent;
	COpcList<COpcMessage*>  m_cQueue;

	UINT                    m_uTotalThreads;
	UINT                    m_uWaitingThreads;
	UINT                    m_uMinThreads;
	UINT                    m_uMaxThreads;
};

#endif // _COpcThreadPool_H_