/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Runtime.InteropServices;

namespace OpcCom.Ae
{
	/// <summary>
	/// Defines all well known COM AE HRESULT codes.
	/// </summary>
	public struct ResultIDs
	{		
		/// <remarks/>
		public const int S_ALREADYACKED         = +0x00040200; // 0x00040200
		/// <remarks/>
		public const int S_INVALIDBUFFERTIME    = +0x00040201; // 0x00040201
		/// <remarks/>
		public const int S_INVALIDMAXSIZE       = +0x00040202; // 0x00040202
		/// <remarks/>
		public const int S_INVALIDKEEPALIVETIME = +0x00040203; // 0x00040203
		/// <remarks/>
		public const int E_INVALIDBRANCHNAME    = -0x3FFBFDFD; // 0xC0040203
		/// <remarks/>
		public const int E_INVALIDTIME          = -0x3FFBFDFC; // 0xC0040204
		/// <remarks/>
		public const int E_BUSY                 = -0x3FFBFDFB; // 0xC0040205
		/// <remarks/>
		public const int E_NOINFO               = -0x3FFBFDFA; // 0xC0040206
	}
}
