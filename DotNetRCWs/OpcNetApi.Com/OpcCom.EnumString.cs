/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Collections;
using System.Runtime.InteropServices;
using OpcRcw.Comn;

namespace OpcCom
{
	/// <summary>
	/// A wrapper for the COM IEnumString interface.
	/// </summary>
	public class EnumString : IDisposable
	{	
		/// <summary>
		/// A reference to the remote COM object.
		/// </summary>
		private IEnumString m_enumerator = null;

		/// <summary>
		/// Initializes the object with an enumerator.
		/// </summary>
		public EnumString(object enumerator)
		{
			m_enumerator = (IEnumString)enumerator;
		}
		
		/// <summary>
		/// Releases the remote COM object.
		/// </summary>
		public void Dispose()
		{
			OpcCom.Interop.ReleaseServer(m_enumerator);
			m_enumerator = null;
		}

		//=====================================================================
		// IEnumString

		/// <summary>
		/// Fetches the next group of strings. 
		/// </summary>
		public string[] Next(int count)
		{
			try
			{
				// create buffer.
                IntPtr buffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(IntPtr))*count);

                try
                {
                    // fetch next group of strings.
                    int fetched = 0;

                    m_enumerator.RemoteNext(
                        count,
                        buffer,
                        out fetched);

                    // return empty array if at end of list.
                    if (fetched == 0)
                    {
                        return new string[0];
                    }

                    return Interop.GetUnicodeStrings(ref buffer, fetched, true);
                }
                finally
                {
                    Marshal.FreeCoTaskMem(buffer);
                }
			}

			// return null on any error.
			catch (Exception)
			{
				return null;
			}
		}

		/// <summary>
		/// Skips a number of strings.
		/// </summary>
		public void Skip(int count)
		{
			m_enumerator.Skip(count);
		}

		/// <summary>
		/// Sets pointer to the start of the list.
		/// </summary>
		public void Reset()
		{
			m_enumerator.Reset();
		}

		/// <summary>
		/// Clones the enumerator.
		/// </summary>
		public EnumString Clone()
		{
			IEnumString enumerator = null;
			m_enumerator.Clone(out enumerator);
			return new EnumString(enumerator);
		}
	}
}
