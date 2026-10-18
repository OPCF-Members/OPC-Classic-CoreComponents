/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Xml;
using System.Net;
using System.Collections;
using System.Globalization;
using System.Runtime.Serialization;
using Opc;

namespace Opc.Ae
{
	#region Category Class
	/// <summary>
	/// The description of an event category supported by the server.
	/// </summary>
	[Serializable]
	public class Category : ICloneable
	{
		#region Public Interface
		/// <summary>
		/// A unique identifier for the category.
		/// </summary>
		public int ID
		{
			get { return m_id;  } 
			set { m_id = value; } 
		}

		/// <summary>
		/// The unique name for the category.
		/// </summary>
		public string Name
		{
			get { return m_name;  } 
			set { m_name = value; } 
		}

		/// <summary>
		/// Returns a string that represents the current object.
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			return Name;
		}
		#endregion

		#region ICloneable Members
		/// <summary>
		/// Creates a shallow copy of the object.
		/// </summary>
		public virtual object Clone() { return MemberwiseClone(); }
		#endregion
		
		#region Private Members
		private int    m_id   = 0;
		private string m_name = null;
		#endregion
	}
	#endregion
}
