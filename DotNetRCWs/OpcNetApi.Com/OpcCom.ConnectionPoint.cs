/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Runtime.InteropServices;
using OpcRcw.Comn;

namespace OpcCom
{
	/// <summary>
	/// Adds and removes a connection point to a server.
	/// </summary>
	public class ConnectionPoint : IDisposable
	{
		/// <summary>
		/// The COM server that supports connection points.
		/// </summary>
		private IConnectionPoint m_server = null;

		/// <summary>
		/// The id assigned to the connection by the COM server.
		/// </summary>
		private int m_cookie = 0;
		
		/// <summary>
		/// The number of times Advise() has been called without a matching Unadvise(). 
		/// </summary>
		private int m_refs = 0;
		
		/// <summary>
		/// Initializes the object by finding the specified connection point.
		/// </summary>
		public ConnectionPoint(object server, Guid iid)
		{
			((IConnectionPointContainer)server).FindConnectionPoint(ref iid, out m_server);
		}

		/// <summary>
		/// Releases the COM server.
		/// </summary>
		public void Dispose()
		{
			if (m_server != null)
			{
				while (Unadvise() > 0);				
				OpcCom.Interop.ReleaseServer(m_server);
				m_server = null;
			}
		}

        /// <summary> 
        /// The cookie returned in the advise call. 
        /// </summary> 
        public int Cookie 
        { 
            get { return m_cookie; } 
        }
		
        //=====================================================================
		// IConnectionPoint

		/// <summary>
		/// Establishes a connection, if necessary and increments the reference count.
		/// </summary>
		public int Advise(object callback)
		{
			if (m_refs++ == 0) m_server.Advise(callback, out m_cookie);
			return m_refs;
		}

		/// <summary>
		/// Decrements the reference count and closes the connection if no more references.
		/// </summary>
		public int Unadvise()
		{
			if (--m_refs == 0) m_server.Unadvise(m_cookie);
			return m_refs;
		}
	}
}
