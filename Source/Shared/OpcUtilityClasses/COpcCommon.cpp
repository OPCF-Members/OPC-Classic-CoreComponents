//==============================================================================
// TITLE: COpcCommon.cpp
//
// CONTENTS:
// 
// A class that implements the IOPCCommon interface.
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
// 2002/09/03 RSA   First release.
// 2003/05/07 RSA   Fixed problem with locale ids on non-english systems.
// 2003/06/25 RSA   Fixed return value for invalid/unsupported locale ids.

#include "StdAfx.h"
#include "COpcCommon.h"

//==============================================================================
// Local Declarations

#define MAX_ERROR_STRING 1024

//==============================================================================
// COpcCommon

COpcCommon::COpcCommon()
{
    m_dwLcid      = LOCALE_SYSTEM_DEFAULT;
    m_cClientName = _T("");
	m_cUserName   = _T("");
}

// Destructor 
COpcCommon::~COpcCommon()
{
    // do nothing.
}

// SetLocaleID
HRESULT COpcCommon::SetLocaleID(LCID dwLcid)
{
    // check that the locale is supported.
    switch (dwLcid)
    {
        case LOCALE_ENGLISH_US:
		case LOCALE_ENGLISH_UK:
        case LOCALE_ENGLISH_NEUTRAL:
		case LOCALE_GERMAN_GERMANY:
		case LOCALE_JAPANESE_JAPAN:
        case LOCALE_NEUTRAL:
        case LOCALE_INVARIANT:
        case LOCALE_SYSTEM_DEFAULT:
        case LOCALE_USER_DEFAULT:
        {
            m_dwLcid = dwLcid;
            return S_OK;
        }

        default:
        {
            const LCID* pLcids = GetAvailableLocaleIDs();

            if (pLcids != NULL)
            {
                for (int ii = 0; pLcids[ii] != NULL; ii++)
                {
                    if (pLcids[ii] == dwLcid)
                    {
                        m_dwLcid = dwLcid;
                        return S_OK;
                    }
                }
            }

            break;
        }
    }

    // locale not available - return error.
    return E_INVALIDARG;
}

// GetLocaleID
HRESULT COpcCommon::GetLocaleID(LCID* pdwLcid)
{
    // invalid arguments - return error.
    if (pdwLcid == NULL)
    {
        return E_INVALIDARG;
    }

    *pdwLcid = m_dwLcid;
    return S_OK;
}

// QueryAvailableLocaleIDs
HRESULT COpcCommon::QueryAvailableLocaleIDs(DWORD* pdwCount, LCID** pdwLcid)
{
    // invalid arguments - return error.
    if (pdwCount == NULL || pdwLcid == NULL)
    {
        return E_INVALIDARG;
    }

	// always supports english and neutral cultures.
	*pdwCount = 4;

	// get additional locales.
    const LCID* pLcids = GetAvailableLocaleIDs();

	// count the available locales.
	if (pLcids != NULL)
	{
		for (int ii = 0; pLcids[ii] != NULL; ii++) (*pdwCount)++;
	}

	// allocate array.
	*pdwLcid = (LCID*)CoTaskMemAlloc((*pdwCount)*sizeof(LCID));

	// add default locales.
	(*pdwLcid)[0] = LOCALE_ENGLISH_US;
	(*pdwLcid)[1] = LOCALE_ENGLISH_UK;
	(*pdwLcid)[2] = LOCALE_GERMAN_GERMANY;
	(*pdwLcid)[3] = LOCALE_JAPANESE_JAPAN;

	DWORD temp = GetSystemDefaultLCID();

	// add additional locales.
	if (pLcids != NULL)
	{
		for (int ii = 0; pLcids[ii] != NULL; ii++) (*pdwLcid)[ii+4] = pLcids[ii];
	}

	// everything ok.
    return S_OK;
}

// GetErrorString
HRESULT COpcCommon::GetErrorString(HRESULT dwError, LPWSTR* ppString)
{
    return GetErrorString(dwError, m_dwLcid, ppString);
}

// GetErrorString
COpcString COpcCommon::GetErrorString(
	const COpcString& cModuleName,
    HRESULT           hError
)
{
	COpcString cString;
	LPWSTR pString = NULL;

	HRESULT hResult = COpcCommon::GetErrorString(cModuleName, hError, LOCALE_NEUTRAL, &pString);

	if (SUCCEEDED(hResult))
	{
		cString = pString;
		OpcFree(pString);
	}

	return cString;
}

// FormatMessage
static bool FormatMessage(
	LANGID  langID, 
	DWORD   dwCode, 
	LPCTSTR szModuleName, 
	TCHAR*  tsBuffer, 
	DWORD   dwLength
)
{
	// attempt to load string from module message table.
	DWORD dwResult = FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		::GetModuleHandle(szModuleName), 
		dwCode,
		langID, 
		tsBuffer,
		dwLength-1,
		NULL 
	);

	// attempt to load string from system message table.
	if (dwResult == 0)
	{
		dwResult = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			dwCode,
			langID, 
			tsBuffer,
			dwLength-1,
			NULL 
		);
	}

	return (dwResult != 0);
}

// GetErrorString
HRESULT COpcCommon::GetErrorString( 
	LPCTSTR szModuleName,
    HRESULT dwError,
    LCID    dwLocale,
    LPWSTR* ppString
)
{  
    // check arguments.
    if (ppString == NULL)
    {
        return E_INVALIDARG;
    }

    *ppString = NULL;

	LANGID langID = LANGIDFROMLCID(dwLocale);

	// lookup languages for 'special' locales.
	switch (dwLocale)
	{
		case LOCALE_SYSTEM_DEFAULT:
		{
			langID = GetSystemDefaultLangID();
			break;
		}

		case LOCALE_USER_DEFAULT:
		{
			langID = GetUserDefaultLangID();
			break;
		}

		case LOCALE_INVARIANT:
		{
			langID = LANGIDFROMLCID(LOCALE_NEUTRAL);
			break;
		}
	}

    // initialize buffer for message.
    TCHAR tsMsg[MAX_ERROR_STRING+1];
    memset(tsMsg, 0, sizeof(tsMsg));

	if (!FormatMessage(langID, dwError, szModuleName, tsMsg, MAX_ERROR_STRING+1))
	{
		// check if the locale explicitly requested a particular language.
		if (dwLocale == LOCALE_SYSTEM_DEFAULT || dwLocale == LOCALE_USER_DEFAULT)
		{
			// try to load message using special locales.
			if (!FormatMessage(LANGIDFROMLCID(dwLocale), dwError, szModuleName, tsMsg, MAX_ERROR_STRING+1))
			{
				// use US english if a default locale was requested but language is not supported by server.
				if (!FormatMessage(LANGIDFROMLCID(LOCALE_ENGLISH_US), dwError, szModuleName, tsMsg, MAX_ERROR_STRING+1))
				{
					return E_INVALIDARG;
				}
			}
		}
		else
		{
			// always return english.
			if (!FormatMessage(LANGIDFROMLCID(LOCALE_ENGLISH_US), dwError, szModuleName, tsMsg, MAX_ERROR_STRING+1))
			{
				return E_INVALIDARG;
			}
		}
	}

    COpcString cMsg = tsMsg;

    // remove trailing \r\n.
	if (cMsg.ReverseFind(_T("\r\n")) == (int)(cMsg.GetLength()-2))
	{
		cMsg = cMsg.SubStr(0, cMsg.GetLength()-2);
	}

    // allocate string for return.
    *ppString = OpcStrDup((LPCWSTR)cMsg);

    return S_OK;
}

// SetClientName
HRESULT COpcCommon::SetClientName(LPCWSTR szName)
{
    m_cClientName = szName;
    return S_OK;
}



//=========================================================================
// IOPCSecurityNT

// IsAvailablePriv
HRESULT COpcCommon::IsAvailableNT(BOOL* pbAvailable)
{
	if (pbAvailable == NULL)
	{
		return E_INVALIDARG;
	}

	*pbAvailable = FALSE;
	return S_OK;
}


// QueryMinImpersonationLevel
HRESULT COpcCommon::QueryMinImpersonationLevel(DWORD* pdwMinImpLevel)
{
	if (pdwMinImpLevel == NULL)
	{
		return E_INVALIDARG;
	}

	*pdwMinImpLevel = RPC_C_IMP_LEVEL_ANONYMOUS;
	return S_OK;
}

// ChangeUser
HRESULT COpcCommon::ChangeUser(void)
{
	return E_FAIL;
}

//=========================================================================
// IOPCSecurityPrivate

// IsAvailablePriv
HRESULT COpcCommon::IsAvailablePriv(BOOL* pbAvailable)
{
	if (pbAvailable == NULL)
	{
		return E_INVALIDARG;
	}

	*pbAvailable = TRUE;
	return S_OK;
}

// Logon
HRESULT COpcCommon::Logon(LPCWSTR szUserID, LPCWSTR szPassword)
{
	if (szUserID == NULL || szUserID[0] == 0)
	{
		m_cUserName.Empty();
		return S_OK;
	}

	if (szPassword == NULL || szPassword[0] == 0)
	{
		return E_FAIL;
	}

	m_cUserName = szUserID;
	return S_OK;
}

// Logoff
HRESULT COpcCommon::Logoff(void)
{
	m_cUserName.Empty();
	return S_OK;
}
