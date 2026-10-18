/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Collections.Generic;
using System.Text;

namespace OpcRcw
{
    /// <summary>
    /// Stores the connection information about a COM server.
    /// </summary>
    public class ServerDescription
    {
        /// <summary>
        /// The name of the machine.
        /// </summary>
        public string HostName { get; set; }

        /// <summary>
        /// The class id.
        /// </summary>
        public Guid Clsid { get; set; }

        /// <summary>
        /// The programmatic id.
        /// </summary>
        public string ProgId { get; set; }

        /// <summary>
        /// The version independent programmatic id.
        /// </summary>
        public string VersionIndependentProgId { get; set; }

        /// <summary>
        /// The description.
        /// </summary>
        public string Description { get; set; }
    }
}
