//==============================================================================
// TITLE: COpcCommon.h
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
//

#ifndef _COpcCommon_H_
#define _COpcCommon_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "opccomn.h"
#include "opcsec.h"

#include "OpcDefs.h"
#include "COpcString.h"

//==============================================================================
// FUNCTION: OPCXX_MESSAGE_MODULE_NAME
// PURPOSE:  Names for modules that contain text for standard OPC messages.

#define OPC_MESSAGE_MODULE_NAME_AE    _T("opc_aeps")
#define OPC_MESSAGE_MODULE_NAME_BATCH _T("opcbc_ps")
#define OPC_MESSAGE_MODULE_NAME_DA    _T("opcproxy")
#define OPC_MESSAGE_MODULE_NAME_DX    _T("opcdxps")
#define OPC_MESSAGE_MODULE_NAME_HDA   _T("opchda_ps")
#define OPC_MESSAGE_MODULE_NAME_SEC   _T("opcsec_ps")
#define OPC_MESSAGE_MODULE_NAME_CMD   _T("opccmdps")

//==============================================================================
// CLASS:   COpcCommon
// PURPOSE: Implements the IOPCCommon interface.
// NOTES:

class OPCUTILS_API COpcCommon :
	public IOPCCommon,
	public IOPCSecurityNT,
	public IOPCSecurityPrivate
{
public:

    //==========================================================================
    // Operators

    // Constructor
    COpcCommon();

    // Destructor 
    ~COpcCommon();

	//==========================================================================
    // Public Methods
    
	// GetErrorString
    static COpcString GetErrorString(
		const COpcString& cModuleName,
        HRESULT           hResult
    );

    // GetErrorString
    static STDMETHODIMP GetErrorString( 
		LPCTSTR szModuleName,
        HRESULT dwError,
        LCID    dwLocale,
        LPWSTR* ppString
    );

    // GetUserName
	LPCWSTR GetUserName() const { return (LPCWSTR)m_cUserName; }

	//==========================================================================
    // IOPCCommon

    // SetLocaleID
    STDMETHODIMP SetLocaleID(LCID dwLcid);

    // GetLocaleID
    STDMETHODIMP GetLocaleID(LCID *pdwLcid);

    // QueryAvailableLocaleIDs
    STDMETHODIMP QueryAvailableLocaleIDs(DWORD* pdwCount, LCID** pdwLcid);

    // GetErrorString
    STDMETHODIMP GetErrorString(HRESULT dwError, LPWSTR* ppString);

    // SetClientName
    STDMETHODIMP SetClientName(LPCWSTR szName);

    // GetLocaleID
    LCID GetLocaleID() const { return m_dwLcid; }

	//=========================================================================
    // IOPCSecurityNT

	// IsAvailablePriv
    STDMETHODIMP IsAvailableNT(BOOL* pbAvailable);
	
	// QueryMinImpersonationLevel
    STDMETHODIMP QueryMinImpersonationLevel(DWORD* pdwMinImpLevel);
	
	// ChangeUser
    STDMETHODIMP ChangeUser(void);

    //=========================================================================
    // IOPCSecurityPrivate

	// IsAvailablePriv
    STDMETHODIMP IsAvailablePriv(BOOL* pbAvailable);

	// Logon
    STDMETHODIMP Logon(LPCWSTR szUserID, LPCWSTR szPassword);
    
	// Logoff
    STDMETHODIMP Logoff(void);

protected:

    //==========================================================================
    // Protected Methods

    // GetClientName
    const COpcString& GetClientName() const { return m_cClientName; }

    // GetAvailableLocaleIDs
    virtual const LCID* GetAvailableLocaleIDs() { return NULL; }
	
	// GetErrorString
    virtual STDMETHODIMP GetErrorString(HRESULT dwError, LCID dwLocale, LPWSTR* ppString) = 0;

private:

    //==========================================================================
    // Private Members

    LCID       m_dwLcid;
    COpcString m_cClientName;
    COpcString m_cUserName;
};

#define LOCALE_ENGLISH_US      MAKELCID(MAKELANGID(LANG_ENGLISH,  SUBLANG_ENGLISH_US), SORT_DEFAULT)
#define LOCALE_ENGLISH_UK      MAKELCID(MAKELANGID(LANG_ENGLISH,  SUBLANG_ENGLISH_UK), SORT_DEFAULT)
#define LOCALE_ENGLISH_NEUTRAL MAKELCID(MAKELANGID(LANG_ENGLISH,  SUBLANG_NEUTRAL),    SORT_DEFAULT)
#define LOCALE_GERMAN_GERMANY  MAKELCID(MAKELANGID(LANG_GERMAN,   SUBLANG_DEFAULT),    SORT_DEFAULT)
#define LOCALE_JAPANESE_JAPAN  MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT),    SORT_DEFAULT)

#endif // _COpcCommon_H_