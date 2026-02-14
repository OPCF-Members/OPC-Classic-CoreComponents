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
