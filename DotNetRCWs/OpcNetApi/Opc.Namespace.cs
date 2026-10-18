/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Net;

namespace Opc
{
	/// <summary>
	/// Declares constants for common XML Schema and OPC namespaces.
	/// </summary>
	public class Namespace
	{
		/// <remarks/>
		public const string XML_SCHEMA                  = "http://www.w3.org/2001/XMLSchema";
		/// <remarks/>
		public const string XML_SCHEMA_INSTANCE         = "http://www.w3.org/2001/XMLSchema-instance";
		/// <remarks/>
		public const string OPC                         = "http://opcfoundation.org/OPC/";
		/// <remarks/>
		public const string OPC_SAMPLE                  = "http://opcfoundation.org/Samples/";
		/// <remarks/>
		public const string OPC_ALARM_AND_EVENTS        = "http://opcfoundation.org/AlarmAndEvents/";
		/// <remarks/>
		public const string OPC_COMPLEX_DATA            = "http://opcfoundation.org/ComplexData/";
		/// <remarks/>
		public const string OPC_DATA_EXCHANGE           = "http://opcfoundation.org/DataExchange/";
		/// <remarks/>
		public const string OPC_DATA_ACCESS             = "http://opcfoundation.org/DataAccess/";
		/// <remarks/>
		public const string OPC_HISTORICAL_DATA_ACCESS  = "http://opcfoundation.org/HistoricalDataAccess/";
		/// <remarks/>
		public const string OPC_DATA_ACCESS_XML10       = "http://opcfoundation.org/webservices/XMLDA/1.0/";
		/// <remarks/>
		public const string OPC_BINARY                  = "http://opcfoundation.org/OPCBinary/1.0/";
		/// <remarks/>
		public const string OPC_UA10                    = "http://opcfoundation.org/webservices/UA/1.0/";
	}
}
