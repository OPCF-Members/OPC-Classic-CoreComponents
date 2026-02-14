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

// ----------------------------------------------------------------------------
// 
// This file is part of the Microsoft COM+ Samples.
// 
// Copyright (C) 1995-2000 Microsoft Corporation. All rights reserved.
// 
// This source code is intended only as a supplement to Microsoft
// Development Tools and/or on-line documentation. See these other
// materials for detailed information regarding Microsoft code samples.
// 
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
// 
// ----------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "ntsecapi.h"
#include "dcomperm.h"

/*---------------------------------------------------------------------------*\
 * NAME: CreateNewSD 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Creates a new security descriptor.
\*---------------------------------------------------------------------------*/
DWORD CreateNewSD (
    SECURITY_DESCRIPTOR **ppSecurityDesc
    )
{
    PACL    pAcl          = NULL;
    DWORD   cbSid         = 0;
    PSID    pSid          = NULL;
    PSID    psidGroup     = NULL;
    PSID    psidOwner     = NULL;
    DWORD   dwReturnValue = ERROR_SUCCESS;

    if(!ppSecurityDesc) return ERROR_BAD_ARGUMENTS;
    
    *ppSecurityDesc = NULL;

    dwReturnValue = GetCurrentUserSID (&pSid);
    
    if (dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

    cbSid = GetLengthSid (pSid);

    *ppSecurityDesc = (SECURITY_DESCRIPTOR *) malloc (
         sizeof (ACL) +  (2 * cbSid) + sizeof (SECURITY_DESCRIPTOR));

    if(!*ppSecurityDesc)
    {
        dwReturnValue = ERROR_OUTOFMEMORY;
        goto CLEANUP;
    }

    psidGroup = (SID *) (*ppSecurityDesc + 1);
    psidOwner = (SID *) (((BYTE *) psidGroup) + cbSid);
    pAcl = (ACL *) (((BYTE *) psidOwner) + cbSid);

    if (!InitializeSecurityDescriptor (*ppSecurityDesc, SECURITY_DESCRIPTOR_REVISION))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    if (!InitializeAcl (pAcl,
                        sizeof (ACL)+sizeof (ACCESS_ALLOWED_ACE)+cbSid,
                        ACL_REVISION2))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    if (!SetSecurityDescriptorDacl (*ppSecurityDesc, TRUE, pAcl, FALSE))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    memcpy (psidGroup, pSid, cbSid);
    if (!SetSecurityDescriptorGroup (*ppSecurityDesc, psidGroup, FALSE))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    memcpy (psidOwner, pSid, cbSid);
    if (!SetSecurityDescriptorOwner (*ppSecurityDesc, psidOwner, FALSE))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

CLEANUP:

    if(dwReturnValue != ERROR_SUCCESS)
    {
        if(*ppSecurityDesc) free (*ppSecurityDesc);
    }

    if(pSid) free (pSid);

    return dwReturnValue;
}

/*---------------------------------------------------------------------------*\
 * NAME: SetLegacyAclDefaults
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Adds the default ACEs to the supplied ACL as confgured in 
 * the legacy COM security model.
\*---------------------------------------------------------------------------*/
DWORD SetLegacyAclDefaults(
    PACL pDacl, 
    DWORD dwSDType
    ) 
{
    DWORD dwReturnValue = ERROR_BAD_ARGUMENTS;

    switch (dwSDType)
    {

        case SDTYPE_DEFAULT_LAUNCH:
        case SDTYPE_APPLICATION_LAUNCH:

        dwReturnValue = AddAccessAllowedACEToACL (&pDacl, 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("SYSTEM"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (&pDacl, 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Administrators"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (&pDacl, 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Interactive"));
        break;
        
        case SDTYPE_DEFAULT_ACCESS:
        case SDTYPE_APPLICATION_ACCESS:

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (&pDacl, 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("SYSTEM"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (&pDacl, 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("SELF"));
        break;
        
        default:

            _tprintf(_T("WARNING: SetLegacyAclDefaults- Invalid security descriptor type.\n"));
        
        break;

    }

CLEANUP:

    return dwReturnValue;
}


/*---------------------------------------------------------------------------*\
 * NAME: SetAclDefaults
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Sets the default ACEs in a ACL for the enhanced COM 
 * security model.
\*---------------------------------------------------------------------------*/
DWORD SetAclDefaults(
    PACL *ppDacl, 
    DWORD dwSDType
    ) 
{
    DWORD dwReturnValue = ERROR_BAD_ARGUMENTS;

    switch (dwSDType)
    {

        case SDTYPE_MACHINE_LAUNCH:

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_ACTIVATE_REMOTE |
                                                  COM_RIGHTS_EXECUTE_REMOTE | 
                                                  COM_RIGHTS_ACTIVATE_LOCAL |
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Administrators"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;
        
        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_ACTIVATE_REMOTE |
                                                  COM_RIGHTS_EXECUTE_REMOTE | 
                                                  COM_RIGHTS_ACTIVATE_LOCAL |
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Offer Remote Assistance Helpers"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_ACTIVATE_LOCAL |
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Everyone"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        break;
        
        case SDTYPE_MACHINE_ACCESS:

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_EXECUTE_REMOTE | 
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Everyone"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Everyone"));


        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        break;
        
        case SDTYPE_DEFAULT_LAUNCH:
        case SDTYPE_APPLICATION_LAUNCH:

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_ACTIVATE_REMOTE |
                                                  COM_RIGHTS_EXECUTE_REMOTE | 
                                                  COM_RIGHTS_ACTIVATE_LOCAL |
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("SYSTEM"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_ACTIVATE_REMOTE |
                                                  COM_RIGHTS_EXECUTE_REMOTE | 
                                                  COM_RIGHTS_ACTIVATE_LOCAL |
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Administrators"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_ACTIVATE_REMOTE |
                                                  COM_RIGHTS_EXECUTE_REMOTE | 
                                                  COM_RIGHTS_ACTIVATE_LOCAL |
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("Interactive"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        break;
        
        case SDTYPE_DEFAULT_ACCESS:
        case SDTYPE_APPLICATION_ACCESS:


        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("SYSTEM"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        dwReturnValue = AddAccessAllowedACEToACL (ppDacl, 
                                                  COM_RIGHTS_EXECUTE_REMOTE | 
                                                  COM_RIGHTS_EXECUTE_LOCAL | 
                                                  COM_RIGHTS_EXECUTE, 
                                                  _T("SELF"));

        if(dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

        break;
        
        default:

            _tprintf(_T("WARNING: SetAclDefaults- Invalid security descriptor type.\n"));
        
        break;

    }

CLEANUP:

    return dwReturnValue;
}


/*---------------------------------------------------------------------------*\
 * NAME: MakeSDAbsolute 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Takes a self-relative security descriptor and returns a
 * newly created absolute security descriptor.
\*---------------------------------------------------------------------------*/
DWORD MakeSDAbsolute (
    PSECURITY_DESCRIPTOR psidOld,
    PSECURITY_DESCRIPTOR *psidNew
    )
{
    PSECURITY_DESCRIPTOR  pSid           = NULL;
    DWORD                 cbDescriptor   = 0;
    DWORD                 cbDacl         = 0;
    DWORD                 cbSacl         = 0;
    DWORD                 cbOwnerSID     = 0;
    DWORD                 cbGroupSID     = 0;
    PACL                  pDacl          = NULL;
    PACL                  pSacl          = NULL;
    PSID                  psidOwner      = NULL;
    PSID                  psidGroup      = NULL;
    BOOL                  fPresent       = FALSE;
    BOOL                  fSystemDefault = FALSE;
    DWORD                 dwReturnValue  = ERROR_SUCCESS;

    // Get SACL
    if (!GetSecurityDescriptorSacl (psidOld, &fPresent, &pSacl, &fSystemDefault))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    if (pSacl && fPresent)
    {
        cbSacl = pSacl->AclSize;
    } 

    // Get DACL
    if (!GetSecurityDescriptorDacl (psidOld, &fPresent, &pDacl, &fSystemDefault))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    if (pDacl && fPresent)
    {
        cbDacl = pDacl->AclSize;
    } 

    // Get Owner
    if (!GetSecurityDescriptorOwner (psidOld, &psidOwner, &fSystemDefault))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    cbOwnerSID = GetLengthSid (psidOwner);

    // Get Group
    if (!GetSecurityDescriptorGroup (psidOld, &psidGroup, &fSystemDefault))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    cbGroupSID = GetLengthSid (psidGroup);

    // Do the conversion
    cbDescriptor = 0;

    MakeAbsoluteSD (psidOld, pSid, &cbDescriptor, pDacl, &cbDacl, pSacl,
                    &cbSacl, psidOwner, &cbOwnerSID, psidGroup,
                    &cbGroupSID);

    pSid = (PSECURITY_DESCRIPTOR) malloc(cbDescriptor);
    if(!pSid)
    {
        dwReturnValue = ERROR_OUTOFMEMORY;
        goto CLEANUP;
    }

    ZeroMemory(pSid, cbDescriptor);
    
    if (!InitializeSecurityDescriptor (pSid, SECURITY_DESCRIPTOR_REVISION))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    if (!MakeAbsoluteSD (psidOld, pSid, &cbDescriptor, pDacl, &cbDacl, pSacl,
                         &cbSacl, psidOwner, &cbOwnerSID, psidGroup,
                         &cbGroupSID))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

CLEANUP:

    if(dwReturnValue != ERROR_SUCCESS && pSid)
    {
        free(pSid);
        pSid = NULL;
    }

    *psidNew = pSid;

    return dwReturnValue;
}

/*---------------------------------------------------------------------------*\
 * NAME: SetNamedValueSD 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Copies the supplied security descriptor to the registry.
\*---------------------------------------------------------------------------*/
DWORD SetNamedValueSD (
    HKEY hkeyRoot,
    LPTSTR tszKeyName,
    LPTSTR tszValueName,
    SECURITY_DESCRIPTOR *pSecurityDesc
    )
{
    DWORD   dwReturnValue = ERROR_SUCCESS;
    DWORD   dwDisposition = 0;
    HKEY    hkeyReg       = NULL;

    // Create new key or open existing key
    dwReturnValue = RegCreateKeyEx (hkeyRoot, tszKeyName, 0, _T(""), 0, KEY_ALL_ACCESS, NULL, &hkeyReg, &dwDisposition);
    if (dwReturnValue != ERROR_SUCCESS)
    {
        goto CLEANUP;
    }

    // Write the security descriptor
    dwReturnValue = RegSetValueEx (hkeyReg, tszValueName, 0, REG_BINARY, (LPBYTE) pSecurityDesc, GetSecurityDescriptorLength (pSecurityDesc));
    if (dwReturnValue != ERROR_SUCCESS)
    {
        goto CLEANUP;
    }

CLEANUP:

    if(hkeyReg) RegCloseKey (hkeyReg);

    return dwReturnValue;
}

/*---------------------------------------------------------------------------*\
 * NAME: GetNamedValueSD 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Retrieves a designated security descriptor from the 
 * registry.
\*---------------------------------------------------------------------------*/
DWORD GetNamedValueSD (
    HKEY hkeyRoot,
    LPTSTR tszKeyName,
    LPTSTR tszValueName,
    SECURITY_DESCRIPTOR **ppSecurityDesc,
    BOOL *psdNew
    )
{
    DWORD  dwReturnValue = ERROR_SUCCESS;
    HKEY   hkeyReg       = NULL;
    DWORD  dwType        = 0;
    DWORD  cbSize        = 0;

    if(!ppSecurityDesc) return ERROR_BAD_ARGUMENTS;

    if(psdNew) *psdNew = FALSE;
    *ppSecurityDesc = NULL;

    // Get the security descriptor from the named value. If it doesn't
    // exist, create a fresh one.
    dwReturnValue = RegOpenKeyEx (hkeyRoot, tszKeyName, 0, KEY_ALL_ACCESS, &hkeyReg);

    if (dwReturnValue != ERROR_SUCCESS)
    {
        if(dwReturnValue == ERROR_FILE_NOT_FOUND)
        {
            goto CLEANUP;
        }
        
        return dwReturnValue;
    }
    
    dwReturnValue = RegQueryValueEx (hkeyReg, tszValueName, NULL, &dwType, NULL, &cbSize);

    if (dwReturnValue != ERROR_SUCCESS && dwReturnValue != ERROR_INSUFFICIENT_BUFFER)
    {
        goto CLEANUP;
    } 

    *ppSecurityDesc = (SECURITY_DESCRIPTOR *) malloc (cbSize);

    dwReturnValue = RegQueryValueEx (hkeyReg, tszValueName, NULL, &dwType, (LPBYTE) *ppSecurityDesc, &cbSize);


CLEANUP:

    if(dwReturnValue != ERROR_SUCCESS)
    {
        if(*ppSecurityDesc) free(*ppSecurityDesc);
    
        *ppSecurityDesc = NULL;

        if(psdNew)
        {
            dwReturnValue = CreateNewSD (ppSecurityDesc);
            if (dwReturnValue == ERROR_SUCCESS)
            {
                *psdNew = TRUE;
            }
        }
    }

    if(hkeyReg) RegCloseKey (hkeyReg);

    return dwReturnValue;
}

/*---------------------------------------------------------------------------*\
 * NAME: ListNamedValueSD 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Displays the designated security descriptor.
\*---------------------------------------------------------------------------*/
DWORD ListNamedValueSD (
    HKEY hkeyRoot,
    LPTSTR tszKeyName,
    LPTSTR tszValueName,
    DWORD dwSDType
    )
{
    DWORD               dwReturnValue = ERROR_SUCCESS;
    SECURITY_DESCRIPTOR *pSD          = NULL;
    BOOL                fPresent      = FALSE;
    BOOL                fDefaultDACL  = FALSE;
    PACL                dacl          = NULL;

    dwReturnValue = GetNamedValueSD (hkeyRoot, tszKeyName, tszValueName, &pSD, NULL);

    if (dwReturnValue != ERROR_SUCCESS)
    {
        _tprintf (_T("<Using Default Permissions>\n"));
        goto CLEANUP;
    }

    if (!GetSecurityDescriptorDacl (pSD, &fPresent, &dacl, &fDefaultDACL))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    if (!fPresent)
    {
        _tprintf (_T("<Access is denied to everyone>\n"));
        goto CLEANUP;
    }

    ListACL (dacl, dwSDType);

CLEANUP:

    if(pSD) free (pSD);

    return dwReturnValue;
}

/*---------------------------------------------------------------------------*\
 * NAME: AddPrincipalToNamedValueSD 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Retrieves the designated security descriptor from the
 * registry and adds an ACE for the designated principal.
\*---------------------------------------------------------------------------*/
DWORD AddPrincipalToNamedValueSD (
    HKEY hkeyRoot,
    LPTSTR tszKeyName,
    LPTSTR tszValueName,
    LPTSTR tszPrincipal,
    BOOL fPermit,
    DWORD dwAccessMask,
    DWORD dwSDType
    )
{
    DWORD               dwReturnValue    = ERROR_SUCCESS;
    SECURITY_DESCRIPTOR *pSD             = NULL;
    SECURITY_DESCRIPTOR *psdSelfRelative = NULL;
    SECURITY_DESCRIPTOR *psdAbsolute     = NULL;
    DWORD               cbSecurityDesc   = 0;
    BOOL                fPresent         = FALSE;
    BOOL                fDefaultDACL     = FALSE;
    PACL                pDacl            = NULL;
    BOOL                fNewSD           = FALSE;

    dwReturnValue = GetNamedValueSD (hkeyRoot, tszKeyName, tszValueName, &pSD, &fNewSD);

    // Get security descriptor from registry or create a new one
    if (dwReturnValue != ERROR_SUCCESS)  goto CLEANUP;

    if (!GetSecurityDescriptorDacl (pSD, &fPresent, &pDacl, &fDefaultDACL))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    if (fNewSD)
    {
        dwReturnValue = SetAclDefaults(&pDacl, dwSDType);
        if (dwReturnValue != ERROR_SUCCESS)  goto CLEANUP;
    }

    // Add the tszPrincipal that the caller wants added
    if (fPermit)
    {
        dwReturnValue = AddAccessAllowedACEToACL (&pDacl, dwAccessMask, tszPrincipal);
    }
    else
    {
        dwReturnValue = AddAccessDeniedACEToACL (&pDacl, dwAccessMask, tszPrincipal);
    }

    if (dwReturnValue != ERROR_SUCCESS) goto CLEANUP;

    // Make the security descriptor absolute if it isn't new
    if (!fNewSD)
    {
        dwReturnValue = MakeSDAbsolute ((PSECURITY_DESCRIPTOR) pSD, (PSECURITY_DESCRIPTOR *) &psdAbsolute);
        if (dwReturnValue != ERROR_SUCCESS) goto CLEANUP;
    }
    else
    {
        psdAbsolute = pSD;
    }

    // Set the discretionary ACL on the security descriptor
    if (!SetSecurityDescriptorDacl (psdAbsolute, TRUE, pDacl, FALSE))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
     }

    // Make the security descriptor self-relative so that we can
    // store it in the registry
    cbSecurityDesc = 0;

    MakeSelfRelativeSD (psdAbsolute, psdSelfRelative, &cbSecurityDesc);

    psdSelfRelative = (SECURITY_DESCRIPTOR *) malloc (cbSecurityDesc);

    if(!psdSelfRelative)
    {
        dwReturnValue = ERROR_OUTOFMEMORY;
        goto CLEANUP;
    }

    if (!MakeSelfRelativeSD (psdAbsolute, psdSelfRelative, &cbSecurityDesc))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Store the security descriptor in the registry
    SetNamedValueSD (hkeyRoot, tszKeyName, tszValueName, psdSelfRelative);

CLEANUP:

    if(pSD) free (pSD);
    if(psdSelfRelative) free (psdSelfRelative);
    if(psdAbsolute && pSD != psdAbsolute) free (psdAbsolute);

    return dwReturnValue;
}


/*---------------------------------------------------------------------------*\
 * NAME: UpdatePrincipalInNamedValueSD 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Retrieves the designated security descriptor from the
 * registry and updates all ACLs that belong to the named principal.
\*---------------------------------------------------------------------------*/
DWORD UpdatePrincipalInNamedValueSD (
    HKEY hkeyRoot,
    LPTSTR tszKeyName,
    LPTSTR tszValueName,
    LPTSTR tszPrincipal,
    DWORD dwAccessMask,
    BOOL fRemove,
    DWORD fAceType
    )
{
    DWORD               dwReturnValue    = ERROR_SUCCESS;
    SECURITY_DESCRIPTOR *pSD             = NULL;
    SECURITY_DESCRIPTOR *psdSelfRelative = NULL;
    SECURITY_DESCRIPTOR *psdAbsolute     = NULL;
    DWORD               cbSecurityDesc   = 0;
    BOOL                fPresent         = FALSE;
    BOOL                fDefaultDACL     = FALSE;
    PACL                pDacl            = NULL;


    // Get security descriptor from registry
    dwReturnValue = GetNamedValueSD (hkeyRoot, tszKeyName, tszValueName, &pSD, NULL);


    if (dwReturnValue != ERROR_SUCCESS)
    {
        if(dwReturnValue == ERROR_FILE_NOT_FOUND && fRemove)
        {
            dwReturnValue = ERROR_SUCCESS;
        }
        goto CLEANUP;
    }

    if (!GetSecurityDescriptorDacl (pSD, &fPresent, &pDacl, &fDefaultDACL))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Update the tszPrincipal that the caller wants to change
    dwReturnValue = UpdatePrincipalInACL (pDacl, tszPrincipal, dwAccessMask, fRemove, fAceType);
    if(dwReturnValue == ERROR_FILE_NOT_FOUND)
    {
        dwReturnValue = ERROR_SUCCESS;
        goto CLEANUP;
    }
    else if (dwReturnValue != ERROR_SUCCESS)
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Make the security descriptor absolute 
    dwReturnValue = MakeSDAbsolute ((PSECURITY_DESCRIPTOR) pSD, (PSECURITY_DESCRIPTOR *) &psdAbsolute); 
    if (dwReturnValue != ERROR_SUCCESS)  goto CLEANUP;


    // Set the discretionary ACL on the security descriptor
    if (!SetSecurityDescriptorDacl (psdAbsolute, TRUE, pDacl, FALSE))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Make the security descriptor self-relative so that we can
    // store it in the registry
    cbSecurityDesc = 0;
    MakeSelfRelativeSD (psdAbsolute, psdSelfRelative, &cbSecurityDesc);

    psdSelfRelative = (SECURITY_DESCRIPTOR *) malloc (cbSecurityDesc);


    if (!MakeSelfRelativeSD (psdAbsolute, psdSelfRelative, &cbSecurityDesc))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Store the security descriptor in the registry
    dwReturnValue = SetNamedValueSD (hkeyRoot, tszKeyName, tszValueName, psdSelfRelative);

CLEANUP:

    if(pSD) free (pSD);
    if(psdSelfRelative) free (psdSelfRelative);
    if(psdAbsolute) free (psdAbsolute);

    return dwReturnValue;
}

/*---------------------------------------------------------------------------*\
 * NAME: RemovePrincipalFromNamedValueSD 
 * --------------------------------------------------------------------------*
 * DESCRIPTION: Retrieves the designated security descriptor from the
 * registry and removes all ACLs that belong to the named principal.
\*---------------------------------------------------------------------------*/
DWORD RemovePrincipalFromNamedValueSD (
    HKEY hkeyRoot,
    LPTSTR tszKeyName,
    LPTSTR tszValueName,
    LPTSTR tszPrincipal,
    DWORD fAceType
    )
{
    DWORD               dwReturnValue    = ERROR_SUCCESS;
    SECURITY_DESCRIPTOR *pSD             = NULL;
    SECURITY_DESCRIPTOR *psdSelfRelative = NULL;
    SECURITY_DESCRIPTOR *psdAbsolute     = NULL;
    DWORD               cbSecurityDesc   = 0;
    BOOL                fPresent         = FALSE;
    BOOL                fDefaultDACL     = FALSE;
    PACL                pDacl            = NULL;

    dwReturnValue = GetNamedValueSD (hkeyRoot, tszKeyName, tszValueName, &pSD, NULL);

    // Get security descriptor from registry or create a new one
    if (dwReturnValue != ERROR_SUCCESS)
    {
        if(dwReturnValue == ERROR_FILE_NOT_FOUND)
        {
            dwReturnValue = ERROR_SUCCESS;
        }
        goto CLEANUP;
    }

    if (!GetSecurityDescriptorDacl (pSD, &fPresent, &pDacl, &fDefaultDACL))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Remove the tszPrincipal that the caller wants removed
    dwReturnValue = RemovePrincipalFromACL (pDacl, tszPrincipal, fAceType);
    if(dwReturnValue == ERROR_FILE_NOT_FOUND)
    {
        dwReturnValue = ERROR_SUCCESS;
        goto CLEANUP;
    }
    else if (dwReturnValue != ERROR_SUCCESS)
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Make the security descriptor absolute if it isn't new
    dwReturnValue = MakeSDAbsolute ((PSECURITY_DESCRIPTOR) pSD, (PSECURITY_DESCRIPTOR *) &psdAbsolute); 
    if (dwReturnValue != ERROR_SUCCESS)  goto CLEANUP;

    // Set the discretionary ACL on the security descriptor
    if (!SetSecurityDescriptorDacl (psdAbsolute, TRUE, pDacl, FALSE))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Make the security descriptor self-relative so that we can
    // store it in the registry
    cbSecurityDesc = 0;
    MakeSelfRelativeSD (psdAbsolute, psdSelfRelative, &cbSecurityDesc);

    psdSelfRelative = (SECURITY_DESCRIPTOR *) malloc (cbSecurityDesc);


    if (!MakeSelfRelativeSD (psdAbsolute, psdSelfRelative, &cbSecurityDesc))
    {
        dwReturnValue = GetLastError();
        goto CLEANUP;
    }

    // Store the security descriptor in the registry
    dwReturnValue = SetNamedValueSD (hkeyRoot, tszKeyName, tszValueName, psdSelfRelative);

CLEANUP:

    if(pSD) free (pSD);
    if(psdSelfRelative) free (psdSelfRelative);
    if(psdAbsolute) free (psdAbsolute);

    return dwReturnValue;
}
