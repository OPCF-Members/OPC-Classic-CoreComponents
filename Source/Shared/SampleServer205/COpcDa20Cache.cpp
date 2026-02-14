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
#include "COpcDa20Cache.h"

//==============================================================================
// Static Data

static COpcCriticalSection g_cLock;
static UINT                g_uRefs   = 0;
static COpcDa20Cache*      g_pCache  = NULL;
static COpcDaDevice*       g_pDevice = NULL;

//==============================================================================
// Static Functions

// GetCache
COpcDaCache& GetCache()
{
    return *g_pCache;
}

// GetDevice
IOpcDaDevice* GetDevice(const COpcString& cItemID)
{
    return (IOpcDaDevice*)g_pDevice;
}

// Initialize
bool Initialize()
{
    COpcLock cLock(g_cLock);
    
    if (g_uRefs > 0)
    {
        g_uRefs++;
        return true;
    }

    bool bResult = true;

    TRY
    {
		g_pDevice = new COpcDaDevice();

		g_pCache = new COpcDa20Cache(*g_pDevice);

        if (!g_pCache->Start())
        {
            THROW_(bResult, false);
        }
        
        g_uRefs++;
    }
    CATCH
    {
		if (g_pCache != NULL)
		{
			g_pCache->Stop();
			delete g_pCache;
			g_pCache = NULL;
		}

		if (g_pDevice != NULL)
		{
			delete g_pDevice;
			g_pDevice = NULL;
		}
    }

    return bResult;
}

// Uninitialize
void Uninitialize()
{
    COpcLock cLock(g_cLock);

    g_uRefs--;
    
    if (g_uRefs == 0)
    {
		if (g_pCache != NULL)
		{
			g_pCache->Stop();
			delete g_pCache;
			g_pCache = NULL;
		}
	
		if (g_pDevice != NULL)
		{
			delete g_pDevice;
			g_pDevice = NULL;
		}

        cLock.Unlock();
        COpcComModule::ExitProcess(S_OK);
    }
}

//==============================================================================
// Local Declarations

// Start
bool COpcDa20Cache::Start()
{
    COpcLock cLock(*this);

    bool bResult = true;

    TRY
    {
		// check if server is running.
		if (GetState() != OPC_STATUS_SUSPENDED)
		{
			THROW_(bResult, false);
		}

		// start the update thread.
		if (!COpcDaCache::Start())
        {
            SetState(OPC_STATUS_FAILED);
			GOTOFINALLY();
        }

		// construct configuration file name.
		COpcString cFileName;
		
		cFileName += OpcDaGetModulePath();
		cFileName += _T("\\");
		cFileName += OpcDaGetModuleName();
		cFileName += _T(".config.xml");

		// load the configuration file.
		if (!COpcDaCache::Load(cFileName))
		{
            SetState(OPC_STATUS_NOCONFIG);
			GOTOFINALLY();
		}

		// startup complete.
		SetState(OPC_STATUS_RUNNING);
    }
    CATCH_FINALLY
    {
    }

    return bResult;
}

// Stop
void COpcDa20Cache::Stop()
{
    COpcLock cLock(*this);

    // stop device.
    m_cDevice.Stop();

	// stop update thread.
	COpcDaCache::Stop();

	// place in suspended state.
	SetState(OPC_STATUS_SUSPENDED);
}

// Load
bool COpcDa20Cache::Load(COpcXmlElement& cRoot)
{
    COpcLock cLock(*this);

	// initialize device.
	if (!m_cDevice.Start(cRoot))
	{
		return false;
	}

	return true;
}

