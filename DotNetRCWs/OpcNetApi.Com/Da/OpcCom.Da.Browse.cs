/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

using System;
using System.Xml;
using System.Collections;
using Opc.Da;

namespace OpcCom.Da
{
	/// <summary>
	/// Implements an object that handles multi-step browse operations.
	/// </summary>
	[Serializable]
	internal class BrowsePosition : Opc.Da.BrowsePosition
	{
		/// <summary>
		/// The continuation point for a browse operation.
		/// </summary>
		internal string ContinuationPoint = null;

		/// <summary>
		/// Indicates that elements that meet the filter criteria have not been returned.
		/// </summary>
		internal bool MoreElements = false;

		/// <summary>
		/// Initializes a browse position 
		/// </summary>
		internal BrowsePosition(
			Opc.ItemIdentifier itemID, 
			BrowseFilters      filters, 
			string             continuationPoint)
		:
			base(itemID, filters)
		{
			ContinuationPoint = continuationPoint;
		}
	}	
}
