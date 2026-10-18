/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace OpcRcw
{
    /// <summary>
    /// Enumerates the OPC server on a host.
    /// </summary>
    public class ServerEnumerator : IDisposable
    {
        #region Constructors
        /// <summary>
        /// Initializes an empty instance.
        /// </summary>
        public ServerEnumerator()
        {
            Initialize();
        }

        /// <summary>
        /// Sets private members to default values.
        /// </summary>
        private void Initialize()
        {
            m_server = null;
            m_host = null;
        }
        #endregion

        #region IDisposable Members
        /// <summary>
        /// The finializer implementation.
        /// </summary>
        ~ServerEnumerator()
        {
            Dispose(false);
        }

        /// <summary>
        /// Frees any unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// An overrideable version of the Dispose.
        /// </summary>
        protected virtual void Dispose(bool disposing)
        {
            if (m_server != null)
            {
                Utils.ReleaseServer(m_server);
                m_server = null;
            }
        }
        #endregion

        #region Public Methods
        /// <summary>
        /// Connects to OPCEnum on the local machine.
        /// </summary>
        public void Connect()
        {
            Connect(null, null, null, null);
        }

        /// <summary>
        /// Connects to OPCEnum on the specified machine.
        /// </summary>
        public void Connect(string host, string username, string password, string domain, bool useConnectSecurity = false)
        {
            // disconnect from current server.
            Disconnect();

            // create in the instance.
            object unknown = null;

            try
            {
                unknown = Utils.CreateInstance(OPCEnumCLSID, host, username, password, domain, useConnectSecurity);
            }
            catch (Exception e)
            {
                throw Utils.CreateComException(e);
            }

            m_server = unknown as OpcRcw.Comn.IOPCServerList2;

            if (m_server == null)
            {
                Utils.ReleaseServer(unknown);

                StringBuilder error = new StringBuilder();

                error.Append("Server does not support IOPCServerList2. ");
                error.Append("The OPC proxy/stubs may not be installed properly or the client or server machine. ");
                error.Append("The also could be a problem with DCOM security configuration.");

                throw Utils.CreateComException(ResultIds.E_NOINTERFACE, error.ToString());
            }

            m_host = host;

            if (String.IsNullOrEmpty(m_host))
            {
                m_host = "localhost";
            }
        }

        /// <summary>
        /// Releases the active server.
        /// </summary>
        public void Disconnect()
        {
            try
            {
                if (m_server != null)
                {
                    Utils.ReleaseServer(m_server);
                    m_server = null;
                }
            }
            catch (Exception e)
            {
                throw Utils.CreateComException(e, ResultIds.E_FAIL, "Could not release OPCEnum server.");
            }
        }

        /// <summary>
        /// Returns a list of servers that support the specified specification.
        /// </summary>
        public ServerDescription[] GetAvailableServers(params Guid[] catids)
        {
            // enumerate servers on specified machine.
            try
            {
                // get list of servers in the specified specification.
                OpcRcw.Comn.IOPCEnumGUID enumerator = null;

                m_server.EnumClassesOfCategories(
                    catids.Length,
                    catids,
                    0,
                    null,
                    out enumerator);

                // read clsids.
                List<Guid> clsids = ReadClasses(enumerator);

                // release enumerator object.					
                Utils.ReleaseServer(enumerator);
                enumerator = null;

                // fetch class descriptions.
                ServerDescription[] servers = new ServerDescription[clsids.Count];

                for (int ii = 0; ii < servers.Length; ii++)
                {
                    servers[ii] = ReadServerDetails(clsids[ii]);
                }

                return servers;
            }
            catch (Exception e)
            {
                throw Utils.CreateComException(e, ResultIds.E_FAIL, "Could not enumerate COM servers.");
            }
        }

        /// <summary>
        /// Looks up the CLSID for the specified prog id on a remote host.
        /// </summary>
        public Guid CLSIDFromProgID(string progID)
        {
            // lookup prog id.
            Guid clsid;

            try
            {
                m_server.CLSIDFromProgID(progID, out clsid);
            }
            catch
            {
                clsid = Guid.Empty;
            }

            // return empty guid if prog id not found.
            return clsid;
        }
        #endregion

        #region Private Methods
        /// <summary>
        /// Reads the guids from the enumerator.
        /// </summary>
        private List<Guid> ReadClasses(OpcRcw.Comn.IOPCEnumGUID enumerator)
        {
            List<Guid> guids = new List<Guid>();

            int fetched = 0;
            Guid[] buffer = new Guid[10];

            do
            {
                try
                {
                    IntPtr pGuids = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(Guid)) * buffer.Length);

                    try
                    {
                        enumerator.Next(buffer.Length, pGuids, out fetched);

                        if (fetched > 0)
                        {
                            IntPtr pos = pGuids;

                            for (int ii = 0; ii < fetched; ii++)
                            {
                                buffer[ii] = (Guid)Marshal.PtrToStructure(pos, typeof(Guid));
                                pos = (IntPtr)(pos.ToInt64() + Marshal.SizeOf(typeof(Guid)));
                                guids.Add(buffer[ii]);
                            }
                        }
                    }
                    finally
                    {
                        Marshal.FreeCoTaskMem(pGuids);
                    }
                }
                catch
                {
                    break;
                }
            }
            while (fetched > 0);

            return guids;
        }

        /// <summary>
        /// Reads the server details from the enumerator.
        /// </summary>
        private ServerDescription ReadServerDetails(Guid clsid)
        {
            // initialize the server uri.
            ServerDescription server = new ServerDescription();
            server.HostName = m_host;
            server.Clsid = clsid;

            string progID = null;

            try
            {
                // fetch class details from the enumerator.
                string description = null;
                string verIndProgID = null;

                m_server.GetClassDetails(
                    ref clsid,
                    out progID,
                    out description,
                    out verIndProgID);

                // use version independent prog id if available.
                if (!String.IsNullOrEmpty(verIndProgID))
                {
                    progID = verIndProgID;
                }

                server.Description = description;
                server.VersionIndependentProgId = verIndProgID;
            }
            catch
            {
                // cannot get prog id.
                progID = null;
            }

            server.ProgId = progID;

            // return the server.
            return server;
        }
        #endregion

        #region Private Members
        private OpcRcw.Comn.IOPCServerList2 m_server = null;
        private string m_host = null;
        #endregion

        #region Private Constants
        private static readonly Guid OPCEnumCLSID = new Guid("13486D51-4821-11D2-A494-3CB306C10000");
        #endregion
    }
}
