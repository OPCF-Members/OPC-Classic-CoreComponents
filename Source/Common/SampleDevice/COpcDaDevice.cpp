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
#include "COpcDaDevice.h"
#include "COpcDaCache.h"

//============================================================================
// Local Declarations

#define MAX_SAMPLING_RATE 100

#define TAG_SEPARATOR      _T("/")
#define TAG_BROWSE_ELEMENT _T("BrowseElement")
#define TAG_ELEMENT_NAME   _T("ElementName")
#define TAG_ITEM           _T("Item")
#define TAG_CHILD_ELEMENTS _T("Children")

// UpdateThreadCtrl
static void WINAPI UpdateThreadCtrl(void* pDevice, bool bStopThread)
{
    if (!bStopThread)
    {
        ((COpcDaDevice*)pDevice)->Update();
    }
}

//============================================================================
// COpcDaDevice

// Start
bool COpcDaDevice::Start()
{
	// construct configuration file name.
	COpcString cFileName;
	
	cFileName += OpcDaGetModulePath();
	cFileName += _T("\\");
	cFileName += OpcDaGetModuleName();
	cFileName += _T(".device.xml");

	// load the xml file.
	COpcXmlDocument cConfigFile;

	if (!cConfigFile.Load(cFileName))
	{
		return false;
	}

	if (!Start(cConfigFile.GetRoot()))
	{
		return false;
	}

    return true;
}

// Start
bool COpcDaDevice::Start(COpcXmlElement& cElement)
{
	// parse the xml document.
	if (!Read(cElement))
	{
		return false;
	}

	// add items to cache.
	if (!BuildAddressSpace())
	{
		return false;
	}

	// start the simulation thread.
    if (!m_cThread.Start(UpdateThreadCtrl, this, INFINITE, THREAD_PRIORITY_TIME_CRITICAL))
    {
        return false;
    }    

    return true;
}

// Stop
void COpcDaDevice::Stop()
{
	// stop simulation thread.
    m_cThread.Stop();

	// clear items from cache.
	ClearAddressSpace();
}
 
// Update
void COpcDaDevice::Update()
{
	LONGLONG llEnd   = 0;
	LONGLONG llStart = 0;
	LONGLONG llFreq  = 0;

	QueryPerformanceFrequency((LARGE_INTEGER*)&llFreq);

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    LONGLONG uTicks = 0;

    while (!m_cThread.WaitingForStop())
    {     
        COpcLock cLock(*this);

        // update each group.
        OPC_POS pos = m_cItems.GetStartPosition();

        while (pos != NULL)
        {
            COpcString cItemID;
            COpcDaDeviceItem* pItem = NULL;
            m_cItems.GetNextAssoc(pos, cItemID, pItem);

            pItem->Update(uTicks, MAX_SAMPLING_RATE);
        }

        cLock.Unlock();

		QueryPerformanceCounter((LARGE_INTEGER*)&llEnd);

		UINT uWaitTime = MAX_SAMPLING_RATE;

		if (llStart != 0)
		{
			double delay = (((double)(llEnd - llStart))/((double)llFreq))*1000 - MAX_SAMPLING_RATE;

			if (delay > 0)
			{
				uWaitTime = (delay < uWaitTime)?(uWaitTime - (UINT)delay):0;
			}
		}

		QueryPerformanceCounter((LARGE_INTEGER*)&llStart);

		{
			LONGLONG llEnd   = 0;
			LONGLONG llStart = 0;

			QueryPerformanceCounter((LARGE_INTEGER*)&llStart);

			Sleep(uWaitTime);

			QueryPerformanceCounter((LARGE_INTEGER*)&llEnd);

			double delay = (((double)(llEnd - llStart))/((double)llFreq))*1000;

			if (delay > uWaitTime)
			{
				delay = 0;
			}
		}
			
        uTicks++;
    }

    CoUninitialize();
}

//========================================================================
// IOpcDaDevice

// BuildAddressSpace
bool COpcDaDevice::BuildAddressSpace()
{
    COpcLock cLock(*this);

    // items to address space.
    OPC_POS pos = m_cItemList.GetHeadPosition();

    while (pos != NULL)
    {
		COpcString cItemID = m_cItemList.GetNext(pos);

		COpcDaDeviceItem* pItem = NULL;

		if (m_cItems.Lookup(cItemID, pItem))
		{
			if (!pItem->BuildAddressSpace())
			{
				return false;
			}
		}
    }

    return true;
}

// ClearAddressSpace
void COpcDaDevice::ClearAddressSpace()
{
    COpcLock cLock(*this);

    // items to address space.
    OPC_POS pos = m_cItemList.GetHeadPosition();

    while (pos != NULL)
    {
		COpcString cItemID = m_cItemList.GetNext(pos);

		COpcDaDeviceItem* pItem = NULL;

		if (m_cItems.Lookup(cItemID, pItem))
		{
			pItem->ClearAddressSpace();
		}
    }
}

// ParseItemID
COpcDaDeviceItem* COpcDaDevice::ParseItemID(const COpcString& cItemID, COpcString& cItemPath)
{
	cItemPath   = (LPCWSTR)NULL;

	COpcString cRootID = cItemID;

	int iIndex = cRootID.Find(CPX_DATABASE_ROOT);

	if (iIndex != -1)
	{
		cItemPath = cRootID.SubStr(iIndex+_tcslen(CPX_DATABASE_ROOT)+1);
		cRootID   = cRootID.SubStr(0, iIndex-1);
	}

	COpcDaDeviceItem* pItem = NULL;

	if (!m_cItems.Lookup(cRootID, pItem))
	{
		return NULL;
	}

	return pItem;
}

// IsKnownItem
bool COpcDaDevice::IsKnownItem(const COpcString& cItemID)
{
    COpcLock cLock(*this);

	COpcString cItemPath;
	return (ParseItemID(cItemID, cItemPath) != NULL);
}

// GetAvailableProperties
HRESULT COpcDaDevice::GetAvailableProperties(
	const COpcString&   cItemID, 
	bool                bReturnValues,
	COpcDaPropertyList& cProperties
)
{
    COpcLock cLock(*this);

	COpcString cItemPath;
	COpcDaDeviceItem* pItem = ParseItemID(cItemID, cItemPath);

	if (pItem == NULL)
    {
        return OPC_E_UNKNOWNITEMID;
    }
	
	return pItem->GetAvailableProperties(cItemPath, bReturnValues, cProperties);
}

// GetAvailableProperties
HRESULT COpcDaDevice::GetAvailableProperties(
	const COpcString&      cItemID, 
	const COpcList<DWORD>& cIDs,
	bool                   bReturnValues,
	COpcDaPropertyList&    cProperties
)
{
    COpcLock cLock(*this);

	COpcString cItemPath;
	COpcDaDeviceItem* pItem = ParseItemID(cItemID, cItemPath);

	if (pItem == NULL)
    {
        return OPC_E_UNKNOWNITEMID;
    }

    return pItem->GetAvailableProperties(cItemPath, cIDs, bReturnValues, cProperties);
}

// Read
HRESULT COpcDaDevice::Read(
	const COpcString& cItemID, 
    DWORD             dwPropertyID,
    VARIANT&          cValue, 
    FILETIME*         pftTimestamp,
    WORD*             pwQuality
)
{
    COpcLock cLock(*this);

	COpcString cItemPath;
	COpcDaDeviceItem* pItem = ParseItemID(cItemID, cItemPath);

	if (pItem == NULL)
    {
        return OPC_E_UNKNOWNITEMID;
    }

	// read value.
	return pItem->Read(cItemPath, dwPropertyID, cValue, pftTimestamp, pwQuality);
}

// Write
HRESULT COpcDaDevice::Write(
	const COpcString& cItemID, 
	DWORD             dwPropertyID,
	const VARIANT&    cValue, 
	FILETIME*         pftTimestamp,
	WORD*             pwQuality
)
{
    COpcLock cLock(*this);

	COpcString cItemPath;
	COpcDaDeviceItem* pItem = ParseItemID(cItemID, cItemPath);

	if (pItem == NULL)
    {
        return OPC_E_UNKNOWNITEMID;
    }

	// write value.
	return pItem->Write(cItemPath, false, dwPropertyID, cValue, pftTimestamp, pwQuality);
}

// GetDeviceItem
COpcDaDeviceItem* COpcDaDevice::GetDeviceItem(const COpcString& cItemID)
{
    COpcLock cLock(*this);

    COpcDaDeviceItem* pItem = NULL;

    if (!m_cItems.Lookup(cItemID, pItem))
    {
        return NULL;
    }

	return pItem;
}

//========================================================================
// IOpcXmlSerialize

// Init
void COpcDaDevice::Init()
{
    m_cItems.RemoveAll();
	m_cItemList.RemoveAll();
    m_cBranches.RemoveAll();
}

// Clear
void COpcDaDevice::Clear()
{
    OPC_POS pos = NULL;

    // clear all items.
    pos = m_cItems.GetStartPosition();

    while (pos != NULL)
    {
        COpcString cItemID;
        COpcDaDeviceItem* pItem = NULL;
        m_cItems.GetNextAssoc(pos, cItemID, pItem);

        delete pItem;
    }

    m_cItems.RemoveAll();
	m_cItemList.RemoveAll();

    // clear all branches.
    pos = m_cBranches.GetStartPosition();

    while (pos != NULL)
    {
        COpcString cItemID;
        COpcStringList* pBranch = NULL;
        m_cBranches.GetNextAssoc(pos, cItemID, pBranch);

        delete pBranch;
    }

    m_cBranches.RemoveAll();

	// clear dictionaries.
	pos = m_cDictionaries.GetStartPosition();

    while (pos != NULL)
    {
        COpcString cDictionaryID;
        COpcDaTypeDictionary* pDictionary = NULL;
        m_cDictionaries.GetNextAssoc(pos, cDictionaryID, pDictionary);

        delete pDictionary;
    }

    m_cDictionaries.RemoveAll();

    Init();
}

// Read
bool COpcDaDevice::Read(COpcXmlElement& cElement)
{
    COpcLock cLock(*this);

    // read top level items.
    COpcXmlElementList cItems;

    UINT uCount = cElement.GetChildren(cItems);

    for (UINT ii = 0; ii < uCount; ii++)
    {
		Read((LPTSTR)NULL, cItems[ii]);
    }

	return true;
}

// Read
bool COpcDaDevice::Read(const COpcString& cElementPath, COpcXmlElement& cElement)
{
    // read element name.
    COpcString cElementName;

	READ_ATTRIBUTE(TAG_ELEMENT_NAME, cElementName);

    if (cElementName.IsEmpty())
    {
        return false;
    }

    // index element by path
	COpcStringList* pBranch = NULL;

	if (!m_cBranches.Lookup(cElementPath, pBranch))
	{
		m_cBranches[cElementPath] = pBranch = new COpcStringList();
	}

	pBranch->AddTail(cElementName);

	// build item id.
	COpcString cItemID;

	cItemID += cElementPath;
    cItemID += (cElementPath.IsEmpty())?_T(""):TAG_SEPARATOR;
	cItemID += cElementName;

	// check if current element is an item.
	COpcXmlElement cItem = cElement.GetChild(TAG_ITEM);

	if (cItem != NULL)
	{
        // check for duplicate item ids.
        if (m_cItems.Lookup(cItemID))
        {
			return false;
        }

		COpcDaDeviceItem* pItem = new COpcDaDeviceItem(cItemID);

		// initialize item.
		if (!pItem->Read(cItem))
		{
			delete pItem;
			return false;
		}

        // index item by item id.
        m_cItems[cItemID] = pItem;
		m_cItemList.AddTail(cItemID);
	}

    // read child elements.
	COpcXmlElement cChildren = cElement.GetChild(TAG_CHILD_ELEMENTS);

	if (cChildren != NULL)
	{
		COpcXmlElementList cItems;

		UINT uCount = cChildren.GetChildren(cItems);

		for (UINT ii = 0; ii < uCount; ii++)
		{
			Read(cItemID, cItems[ii]);
		}
	}

    return true;
}

// Write
bool COpcDaDevice::Write(COpcXmlElement& cElement)
{
	OPC_ASSERT(false);  
	
	// not implemented.
	
	return false;
}
