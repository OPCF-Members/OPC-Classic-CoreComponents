using System;
using System.Collections.Generic;
using System.Text;

namespace OpcCom
{
#if NETCORE
    /// <summary>
    /// The COM FILETIME structure.
    /// </summary>
    public struct FILETIME
    {
        /// <summary>
        /// The least significant DWORD.
        /// </summary>
        public int dwLowDateTime;

        /// <summary>
        /// The most significant DWORD.
        /// </summary>
        public int dwHighDateTime;
    }
#endif
}
