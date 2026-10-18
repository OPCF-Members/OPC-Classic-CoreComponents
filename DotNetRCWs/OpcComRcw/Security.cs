/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

#pragma warning disable 1591

namespace OpcRcw.Security
{       
    /// <exclude />
	[ComImport]
	[GuidAttribute("7AA83A01-6C77-11d3-84F9-00008630A38B")]
	[InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)] 
    public interface IOPCSecurityNT
    {
	    void IsAvailableNT(
		    [Out][MarshalAs(UnmanagedType.I4)]
		    out int pbAvailable);

	    void QueryMinImpersonationLevel(
		    [Out][MarshalAs(UnmanagedType.I4)]
		    out int pdwMinImpLevel);

	    void ChangeUser();
    };

    /// <exclude />
	[ComImport]
	[GuidAttribute("7AA83A02-6C77-11d3-84F9-00008630A38B")]
	[InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)] 
    public interface IOPCSecurityPrivate
    {
        void IsAvailablePriv(
		    [Out][MarshalAs(UnmanagedType.I4)]
		    out int pbAvailable);

        void Logon(
			[MarshalAs(UnmanagedType.LPWStr)]
		    string szUserID, 
			[MarshalAs(UnmanagedType.LPWStr)]
		    string szPassword);

        void Logoff();
    };
}
