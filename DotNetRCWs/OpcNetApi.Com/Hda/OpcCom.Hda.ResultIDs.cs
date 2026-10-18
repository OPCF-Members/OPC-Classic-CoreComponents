/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Xml;
using System.Collections;
using System.Reflection;
using System.Runtime.InteropServices;

namespace OpcCom
{
	namespace Hda
	{
		/// <summary>
		/// Defines all well known COM HDA HRESULT codes.
		/// </summary>
		public struct ResultIDs
		{	
			/// <remarks/>
			public const int E_MAXEXCEEDED      = -0X3FFBEFFF; // 0xC0041001
			/// <remarks/>
			public const int S_NODATA           = +0x40041002; // 0x40041002
			/// <remarks/>
			public const int S_MOREDATA         = +0x40041003; // 0x40041003
			/// <remarks/>
			public const int E_INVALIDAGGREGATE = -0X3FFBEFFC; // 0xC0041004
			/// <remarks/>
			public const int S_CURRENTVALUE     = +0x40041005; // 0x40041005
			/// <remarks/>
			public const int S_EXTRADATA        = +0x40041006; // 0x40041006
			/// <remarks/>
			public const int W_NOFILTER         = -0x7FFBEFF9; // 0x80041007
			/// <remarks/>
			public const int E_UNKNOWNATTRID    = -0x3FFBEFF8; // 0xC0041008
			/// <remarks/>
			public const int E_NOT_AVAIL        = -0x3FFBEFF7; // 0xC0041009
			/// <remarks/>
			public const int E_INVALIDDATATYPE  = -0x3FFBEFF6; // 0xC004100A
			/// <remarks/>
			public const int E_DATAEXISTS       = -0x3FFBEFF5; // 0xC004100B
			/// <remarks/>
			public const int E_INVALIDATTRID    = -0x3FFBEFF4; // 0xC004100C
			/// <remarks/>
			public const int E_NODATAEXISTS     = -0x3FFBEFF3; // 0xC004100D
			/// <remarks/>
			public const int S_INSERTED         = +0x4004100E; // 0x4004100E
			/// <remarks/>
			public const int S_REPLACED         = +0x4004100F; // 0x4004100F
		}
	}
}
