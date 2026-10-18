/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Runtime.InteropServices;

namespace OpcCom
{
	/// <summary>
	/// An interface that is invoked when the wrapper loads/unloads the server.
	/// </summary>
	[ComImport]
	[GuidAttribute("50E8496C-FA60-46a4-AF72-512494C664C6")]
	[InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)] 
	public interface IOPCWrappedServer
	{
		/// <summary>
		/// Called when the object is loaded by the COM wrapper process.
		/// </summary>
		void Load([MarshalAs(UnmanagedType.LPStruct)] Guid clsid);

		/// <summary>
		/// Called when the object is unloaded by the COM wrapper process.
		/// </summary>
		void Unload();
	}
}
