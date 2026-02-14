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
#include "COpcDaProperty.h"
#include "COpcVariant.h"

static const LPCWSTR OPC_PROPERTY_DESC_SIMULATION_NAME      = L"Simulation Name";
static const LPCWSTR OPC_PROPERTY_DESC_SIMULATION_COUNT     = L"Simulation Count";
static const LPCWSTR OPC_PROPERTY_DESC_SIMULATION_TIMESTAMP = L"Simulation Timestamp";

//==============================================================================
// Local Declarations

struct OpcDaPropertyDesc
{
    DWORD   dwID;
    VARTYPE vtDataType;
    LPCWSTR szDescription;
};

OpcDaPropertyDesc g_PropertyTable[] = 
{
    { OPC_PROPERTY_DATATYPE,                VT_I2,              OPC_PROPERTY_DESC_DATATYPE                },
    { OPC_PROPERTY_VALUE,                   VT_VARIANT,         OPC_PROPERTY_DESC_VALUE                   },
    { OPC_PROPERTY_QUALITY,                 VT_I2,              OPC_PROPERTY_DESC_QUALITY                 },
    { OPC_PROPERTY_TIMESTAMP,               VT_DATE,            OPC_PROPERTY_DESC_TIMESTAMP               },
	{ OPC_PROPERTY_ACCESS_RIGHTS,           VT_I4,              OPC_PROPERTY_DESC_ACCESS_RIGHTS           },
    { OPC_PROPERTY_SCAN_RATE,               VT_R4,              OPC_PROPERTY_DESC_SCAN_RATE               },
    { OPC_PROPERTY_EU_TYPE,                 VT_I4,              OPC_PROPERTY_DESC_EU_TYPE                 },
    { OPC_PROPERTY_EU_INFO,                 VT_BSTR | VT_ARRAY, OPC_PROPERTY_DESC_EU_INFO                 },
    { OPC_PROPERTY_EU_UNITS,                VT_BSTR,            OPC_PROPERTY_DESC_EU_UNITS                },
    { OPC_PROPERTY_DESCRIPTION,             VT_BSTR,            OPC_PROPERTY_DESC_DESCRIPTION             },
    { OPC_PROPERTY_HIGH_EU,                 VT_R8,              OPC_PROPERTY_DESC_HIGH_EU                 },
    { OPC_PROPERTY_LOW_EU,                  VT_R8,              OPC_PROPERTY_DESC_LOW_EU                  },
    { OPC_PROPERTY_HIGH_IR,                 VT_R8,              OPC_PROPERTY_DESC_HIGH_IR                 },
    { OPC_PROPERTY_LOW_IR,                  VT_R8,              OPC_PROPERTY_DESC_LOW_IR                  },
    { OPC_PROPERTY_CLOSE_LABEL,             VT_BSTR,            OPC_PROPERTY_DESC_CLOSE_LABEL             },
    { OPC_PROPERTY_OPEN_LABEL,              VT_BSTR,            OPC_PROPERTY_DESC_OPEN_LABEL              },
    { OPC_PROPERTY_TIMEZONE,                VT_I4,              OPC_PROPERTY_DESC_TIMEZONE                },
    { OPC_PROPERTY_CONDITION_STATUS,        VT_BSTR,            OPC_PROPERTY_DESC_CONDITION_STATUS        },
    { OPC_PROPERTY_ALARM_QUICK_HELP,        VT_BSTR,            OPC_PROPERTY_DESC_ALARM_QUICK_HELP        },
    { OPC_PROPERTY_ALARM_AREA_LIST,         VT_BSTR | VT_ARRAY, OPC_PROPERTY_DESC_ALARM_AREA_LIST         },
    { OPC_PROPERTY_PRIMARY_ALARM_AREA,      VT_BSTR,            OPC_PROPERTY_DESC_PRIMARY_ALARM_AREA      },
    { OPC_PROPERTY_CONDITION_LOGIC,         VT_BSTR,            OPC_PROPERTY_DESC_CONDITION_LOGIC         },
    { OPC_PROPERTY_LIMIT_EXCEEDED,          VT_BSTR,            OPC_PROPERTY_DESC_LIMIT_EXCEEDED          },
    { OPC_PROPERTY_DEADBAND,                VT_R8,              OPC_PROPERTY_DESC_DEADBAND                },
    { OPC_PROPERTY_HIHI_LIMIT,              VT_R8,              OPC_PROPERTY_DESC_HIHI_LIMIT              },
    { OPC_PROPERTY_HI_LIMIT,                VT_R8,              OPC_PROPERTY_DESC_HI_LIMIT                },
    { OPC_PROPERTY_LO_LIMIT,                VT_R8,              OPC_PROPERTY_DESC_LO_LIMIT                },
    { OPC_PROPERTY_LOLO_LIMIT,              VT_R8,              OPC_PROPERTY_DESC_LOLO_LIMIT              },
    { OPC_PROPERTY_CHANGE_RATE_LIMIT,       VT_R8,              OPC_PROPERTY_DESC_CHANGE_RATE_LIMIT       },
    { OPC_PROPERTY_DEVIATION_LIMIT,         VT_R8,              OPC_PROPERTY_DESC_DEVIATION_LIMIT         },
    { OPC_PROPERTY_SOUND_FILE,              VT_BSTR,            OPC_PROPERTY_DESC_SOUND_FILE              },
    { OPC_PROPERTY_TYPE_SYSTEM_ID,          VT_BSTR,            OPC_PROPERTY_DESC_TYPE_SYSTEM_ID          },
    { OPC_PROPERTY_DICTIONARY_ID,           VT_BSTR,            OPC_PROPERTY_DESC_DICTIONARY_ID           },
    { OPC_PROPERTY_DICTIONARY,              VT_VARIANT,         OPC_PROPERTY_DESC_DICTIONARY              },
    { OPC_PROPERTY_TYPE_ID,                 VT_BSTR,            OPC_PROPERTY_DESC_TYPE_ID                 },
    { OPC_PROPERTY_TYPE_DESCRIPTION,        VT_VARIANT,         OPC_PROPERTY_DESC_TYPE_DESCRIPTION        },
    { OPC_PROPERTY_CONSISTENCY_WINDOW,      VT_BSTR,            OPC_PROPERTY_DESC_CONSISTENCY_WINDOW      },
    { OPC_PROPERTY_WRITE_BEHAVIOR,          VT_BSTR,            OPC_PROPERTY_DESC_WRITE_BEHAVIOR          },
    { OPC_PROPERTY_UNCONVERTED_ITEM_ID,     VT_BSTR,            OPC_PROPERTY_DESC_UNCONVERTED_ITEM_ID     },
    { OPC_PROPERTY_UNFILTERED_ITEM_ID,      VT_BSTR,            OPC_PROPERTY_DESC_UNFILTERED_ITEM_ID      },
    { OPC_PROPERTY_DATA_FILTER_VALUE,       VT_BSTR,            OPC_PROPERTY_DESC_DATA_FILTER_VALUE       },
    { OPC_PROPERTY_SIMULATION_NAME,         VT_BSTR,            OPC_PROPERTY_DESC_SIMULATION_NAME         },
    { OPC_PROPERTY_SIMULATION_COUNT,        VT_UI4,             OPC_PROPERTY_DESC_SIMULATION_COUNT        },
    { OPC_PROPERTY_SIMULATION_TIMESTAMP,    VT_DATE,            OPC_PROPERTY_DESC_SIMULATION_TIMESTAMP    },
    { 0, VT_EMPTY, NULL }
};

// OpcGetPropertyType
VARTYPE OpcGetPropertyType(DWORD dwID)
{
    for (DWORD ii = 0; g_PropertyTable[ii].dwID != 0; ii++)
    {
        if (g_PropertyTable[ii].dwID == dwID)
        {
            return g_PropertyTable[ii].vtDataType;
        }
    }

    return VT_EMPTY;
}

// OpcGetPropertyDesc
LPCWSTR OpcGetPropertyDesc(DWORD dwID)
{
    for (DWORD ii = 0; g_PropertyTable[ii].dwID != 0; ii++)
    {
        if (g_PropertyTable[ii].dwID == dwID)
        {
            return g_PropertyTable[ii].szDescription;
        }
    }

    return NULL;
}

// OpcParseItemID
bool OpcParseItemID(COpcString& cItemID, DWORD& dwPropertyID)
{
	// check for empty string.
	if (cItemID.IsEmpty())
	{
		return false;
	}

	// check for property id qualifier.
    int iIndex = cItemID.ReverseFind(_T(":"));

	// extract the property id.
    if (iIndex != -1)
    {
        if (!OpcXml::Read(cItemID.SubStr(iIndex+1), dwPropertyID))
        {
            return false;
        }

        cItemID = cItemID.SubStr(0, iIndex);
    }

	// item id is valid.
    return true;
}

// OpcConstructItemID
COpcString OpcConstructItemID(LPCWSTR szItemID, DWORD dwPropertyID)
{
	// append the property id to the item id.
    COpcString cItemID;

    if (!OpcXml::Write(dwPropertyID, cItemID))
    {      
		return (LPCWSTR)NULL;
    }

    cItemID = (COpcString)szItemID + _T(":") + cItemID;
	
	// allocate the memory and return.
	return cItemID;
}

//============================================================================
// COpcDaProperty

// Constructor
COpcDaProperty::COpcDaProperty(DWORD dwID)
{
	m_dwID         = dwID;
	m_cDescription = OpcGetPropertyDesc(dwID);
	m_vtDataType   = OpcGetPropertyType(dwID);
	m_hError       = S_OK;
}

// Copy Constructor
COpcDaProperty::COpcDaProperty(const COpcDaProperty& cProperty)
{
	*this = cProperty;
}

// Destructor
COpcDaProperty::~COpcDaProperty()
{
}

// Assignment.
COpcDaProperty& COpcDaProperty::operator=(const COpcDaProperty& cProperty)
{
	m_dwID         = cProperty.m_dwID;
	m_cItemID      = cProperty.m_cItemID;
	m_cDescription = cProperty.m_cDescription;
	m_vtDataType   = cProperty.m_vtDataType;
	m_cValue       = cProperty.m_cValue;
	m_hError       = cProperty.m_hError;

	return *this;
}

//==========================================================================
// Static Methods

// COpcDaProperty::LocalizeText
LPWSTR COpcDaProperty::LocalizeText(LPCWSTR szString, DWORD dwLocaleId, LPCWSTR  szUserName)
{ 
	if (szString == NULL)
	{
		return NULL;
	}

	if ((dwLocaleId == 0 || dwLocaleId == LOCALE_SYSTEM_DEFAULT || dwLocaleId == LOCALE_ENGLISH_US) && (szUserName == NULL || szUserName[0] == 0))
	{
		return ::OpcStrDup(szString);
	}

	WCHAR szBuffer[4096];
	
	if (szUserName == NULL || szUserName[0] == 0)
	{
		swprintf(szBuffer, 4096, L"%s [%04X]", szString, dwLocaleId);
	}
	else
	{
		swprintf(szBuffer, 4096, L"%s [%04X][%s]", szString, dwLocaleId, szUserName);
	}

	return ::OpcStrDup(szBuffer);
}

// COpcDaProperty::DeLocalizeText
LPWSTR COpcDaProperty::DeLocalizeText(LPCWSTR szString, DWORD dwLocaleId, LPCWSTR  szUserName)
{ 
	if (szString == NULL)
	{
		return NULL;
	}

	if ((dwLocaleId == LOCALE_SYSTEM_DEFAULT || dwLocaleId == LOCALE_ENGLISH_US) && (szUserName == NULL || szUserName[0] == 0))
	{
		return ::OpcStrDup(szString);
	}

	WCHAR szBuffer[4096];
	swprintf(szBuffer, 4096, L" [%04X]", dwLocaleId);

	for (int ii = wcslen(szString)-1; ii >= 0; ii--)
	{
		if (wcsncmp(szString+ii, szBuffer, 7) == 0)
		{
			wcsncpy(szBuffer, szString, ii);
			szBuffer[ii] = 0;
			return ::OpcStrDup(szBuffer);
		}
	}

	return ::OpcStrDup(szString);
}

// LocalizeVARIANT
void COpcDaProperty::LocalizeVARIANT( 
	DWORD    dwLocaleID,
	LPCWSTR  szUserName,
    VARIANT* pvData)
{
	if (pvData != NULL && pvData->vt == VT_BSTR)
	{
		LPWSTR szName = COpcDaProperty::LocalizeText(pvData->bstrVal, dwLocaleID, szUserName);
		SysFreeString(pvData->bstrVal);
		pvData->bstrVal = SysAllocString(szName);
		OpcFree(szName);
	}
}

// LocalizeVARIANTs
void COpcDaProperty::LocalizeVARIANTs( 
	DWORD    dwLocaleID,
	LPCWSTR  szUserName,
	DWORD    dwCount,
    VARIANT* pvData)
{
	for (DWORD ii = 0; ii < dwCount; ii++)
	{
		LocalizeVARIANT(dwLocaleID, szUserName, &(pvData[ii]));
	}
}

// LocalizeOPCITEMSTATE
void COpcDaProperty::LocalizeOPCITEMSTATE( 
	DWORD         dwLocaleID,
	LPCWSTR       szUserName,
	DWORD         dwCount,
	OPCITEMSTATE* pValues)
{
	for (DWORD ii = 0; ii < dwCount; ii++)
	{
		LocalizeVARIANT(dwLocaleID, szUserName, &(pValues[ii].vDataValue));
	}
}

// LocalizeOPCBROWSEELEMENT
void COpcDaProperty::LocalizeOPCBROWSEELEMENT(
	DWORD			  dwLocaleID,
	LPCWSTR           szUserName,
	DWORD	          dwCount,
	OPCBROWSEELEMENT* pBrowseElements)
{
	for (DWORD ii = 0; ii < dwCount; ii++)
	{
		LPWSTR szName = COpcDaProperty::LocalizeText(pBrowseElements[ii].szName, dwLocaleID, szUserName);
		OpcFree(pBrowseElements[ii].szName);
		pBrowseElements[ii].szName = szName;
		LocalizeOPCITEMPROPERTIES(dwLocaleID, szUserName, 1, &pBrowseElements->ItemProperties);
	}
}

// LocalizeOPCITEMPROPERTIES
void COpcDaProperty::LocalizeOPCITEMPROPERTIES( 
	DWORD			   dwLocaleID,
	LPCWSTR            szUserName,
    DWORD		       dwPropertyCount,
    OPCITEMPROPERTIES* pItemProperties)
{
	for (DWORD ii = 0; ii < dwPropertyCount; ii++)
	{
		if (FAILED(pItemProperties[ii].hrErrorID))
		{
			continue;
		}

		for (DWORD jj = 0; jj < pItemProperties[ii].dwNumProperties; jj++)
		{
			if (FAILED(pItemProperties[ii].pItemProperties[jj].hrErrorID))
			{
				continue;
			}

			VARIANT vValue = pItemProperties[ii].pItemProperties[jj].vValue;

			if (vValue.vt == VT_BSTR)
			{
				LPWSTR szName = COpcDaProperty::LocalizeText(vValue.bstrVal, dwLocaleID, szUserName);
				SysFreeString(vValue.bstrVal);
				pItemProperties[ii].pItemProperties[jj].vValue.bstrVal = SysAllocString(szName);
				OpcFree(szName);
			}
		}
	}
}

// COpcDaProperty::Create
void COpcDaProperty::Create(
	const COpcList<DWORD>& cIDs, 
	COpcDaPropertyList&    cProperties
)
{
	COpcDaProperty::Free(cProperties);

	cProperties.SetSize(cIDs.GetCount());

	OPC_POS pos = cIDs.GetHeadPosition();

	for (DWORD ii = 0; pos != NULL; ii++)
	{
		cProperties[ii] = new COpcDaProperty(cIDs.GetNext(pos));
	}
}

// COpcDaProperty::Copy
void COpcDaProperty::Copy(
	const COpcDaPropertyList& cProperties,
    DWORD&                    dwCount,
    DWORD*&                   pPropertyIDs,
    LPWSTR*&                  pDescriptions,
    VARTYPE*&                 pvtDataTypes
)
{
	dwCount = cProperties.GetSize();

	if (dwCount == 0)
	{
		pPropertyIDs  = NULL;
		pDescriptions = NULL;
		pvtDataTypes  = NULL;
		return;
	}

	pPropertyIDs  = OpcArrayAlloc(DWORD, dwCount);
	pDescriptions = OpcArrayAlloc(LPWSTR, dwCount);
	pvtDataTypes  = OpcArrayAlloc(VARTYPE, dwCount);

	memset(pPropertyIDs, 0, sizeof(DWORD)*dwCount);
	memset(pDescriptions, 0, sizeof(LPWSTR)*dwCount);
	memset(pvtDataTypes, 0, sizeof(VARTYPE)*dwCount);

	for (DWORD ii = 0; ii < dwCount; ii++)
	{
		pPropertyIDs[ii]  = cProperties[ii]->GetID();
		pDescriptions[ii] = OpcStrDup((LPCWSTR)cProperties[ii]->GetDescription());
		pvtDataTypes[ii]  = cProperties[ii]->GetDataType();
	}
}

// COpcDaProperty::Copy
void COpcDaProperty::Copy(
	const COpcDaPropertyList& cProperties,
    DWORD&                    dwCount,
    VARIANT*&                 pValues,
	HRESULT*&                 pErrors
)
{
	dwCount = cProperties.GetSize();

	if (dwCount == 0)
	{
		pValues = NULL;
		pErrors = NULL;
		return;
	}

	pValues = OpcArrayAlloc(VARIANT, dwCount);
	pErrors = OpcArrayAlloc(HRESULT, dwCount);

	memset(pValues, 0, sizeof(VARIANT)*dwCount);
	memset(pErrors, 0, sizeof(HRESULT)*dwCount);

	for (DWORD ii = 0; ii < dwCount; ii++)
	{
		OpcVariantCopy(&(pValues[ii]), &(cProperties[ii]->GetValue()));
		pErrors[ii] = cProperties[ii]->GetError();	
	}
}

// COpcDaProperty::Copy
void COpcDaProperty::Copy(
	const COpcDaPropertyList& cProperties,
    DWORD&                    dwCount,
    LPWSTR*&                  pItemIDs,
	HRESULT*&                 pErrors
)
{
	dwCount = cProperties.GetSize();

	if (dwCount == 0)
	{
		pItemIDs = NULL;
		pErrors  = NULL;
		return;
	}

	pItemIDs = OpcArrayAlloc(LPWSTR, dwCount);
	pErrors  = OpcArrayAlloc(HRESULT, dwCount);

	memset(pItemIDs, 0, sizeof(LPWSTR)*dwCount);
	memset(pErrors, 0, sizeof(HRESULT)*dwCount);

	for (DWORD ii = 0; ii < dwCount; ii++)
	{
		pItemIDs[ii] = OpcStrDup((LPCWSTR)cProperties[ii]->GetItemID());
		pErrors[ii]  = cProperties[ii]->GetError();
	}
}

// COpcDaProperty::Copy
void COpcDaProperty::Copy(
	const COpcDaPropertyList& cProperties,
	bool                      bReturnValues,
	DWORD&                    dwCount,
	OPCITEMPROPERTY*&         pProperties
)
{
	dwCount = cProperties.GetSize();

	if (dwCount == 0)
	{
		pProperties = NULL;
		return;
	}

	pProperties = OpcArrayAlloc(OPCITEMPROPERTY, dwCount);

	memset(pProperties, 0, sizeof(OPCITEMPROPERTY)*dwCount);

	for (DWORD ii = 0; ii < dwCount; ii++)
	{
		pProperties[ii].dwPropertyID  = cProperties[ii]->GetID();
		pProperties[ii].szDescription = OpcStrDup((LPCWSTR)cProperties[ii]->GetDescription());
		pProperties[ii].vtDataType    = cProperties[ii]->GetDataType();
		pProperties[ii].szItemID      = OpcStrDup((LPCWSTR)cProperties[ii]->GetItemID());
		pProperties[ii].hrErrorID     = cProperties[ii]->GetError();

		if (bReturnValues)
		{
			OpcVariantCopy(&(pProperties[ii].vValue), &(cProperties[ii]->GetValue()));
		}
	}
}

// COpcDaProperty::Free
void COpcDaProperty::Free(COpcDaPropertyList& cProperties)
{	
	for (DWORD ii = 0; ii < cProperties.GetSize(); ii++)
	{
		delete cProperties[ii];
	}

	cProperties.RemoveAll();
}
