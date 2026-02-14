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
