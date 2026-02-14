//==============================================================================
// TITLE: CXmlDaSourceCache.cpp
//
// CONTENTS:
// 
// A concrete instance of a OPC Data Server cache.
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
// 2002/11/16 RSA   First release.
//

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

