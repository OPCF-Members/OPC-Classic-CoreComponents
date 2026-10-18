/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _COpcDaEngine_H_
#define _COpcDaEngine_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "COpcDaCache.h"
#include "COpcDaDevice.h"

//==============================================================================
// CLASS:   COpcDaCache
// PURPOSE: A concrete instance of a OPC Data Server cache.

class COpcDa20Cache : public COpcDaCache
{
    OPC_CLASS_NEW_DELETE()

public:

    //==========================================================================
    // Public Operators

    // Constructor
	COpcDa20Cache(COpcDaDevice& cDevice) : m_cDevice(cDevice) {}

    // Destructor 
	~COpcDa20Cache() {}

	//=========================================================================
    // Public Methods

	// Start
	virtual bool Start();
	
	// Stop
	virtual void Stop();

	// Load
	virtual bool Load(COpcXmlElement& cRoot);

private:

    //==========================================================================
    // Private Members

	COpcDaDevice& m_cDevice;
};


//==============================================================================
// Global Functions

// Initialize
extern bool Initialize();

// Uninitialize
extern void Uninitialize();

#endif // _COpcDaEngine_H_
