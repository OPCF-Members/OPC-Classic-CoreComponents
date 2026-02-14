//==============================================================================
// TITLE: OpcUtils.h
//
// CONTENTS:
// 
// Main header for OPC Common library.
//
// (c) Copyright 2002-2003 The OPC Foundation
// ALL RIGHTS RESERVED.
//
// DISCLAIMER:
//  This code is provided by the OPC Foundation solely to assist in 
//  understanding and use of the appropriate OPC Specification(s) and may be 
//  used as set forth in the License Grant section of the OPC Specification.
//  This code is provided as-is and without warranty or support of any sort
//  and is subject to the Warranty and Liability Disclaimers which appear
//  in the printed OPC Specification.
//
// MODIFICATION LOG:
//
// Date       By    Notes
// ---------- ---   -----
// 2002/09/03 RSA   First release.
//

#ifndef _OpcUtils_H_
#define _OpcUtils_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "opccomn.h"

#define OPCUTILS_API

#include "OpcDefs.h"
#include "COpcString.h"
#include "COpcFile.h"
#include "OpcMatch.h"
#include "COpcCriticalSection.h"
#include "COpcArray.h"
#include "COpcList.h"
#include "COpcMap.h"
#include "COpcSortedArray.h"
#include "COpcText.h"
#include "COpcTextReader.h"
#include "COpcThread.h"
#include "OpcCategory.h"
#include "OpcRegistry.h"
#include "OpcXmlType.h"
#include "COpcXmlAnyType.h"
#include "COpcXmlElement.h"
#include "COpcXmlDocument.h"
#include "COpcVariant.h"
#include "COpcComObject.h"
#include "COpcClassFactory.h"
#include "COpcComModule.h"
#include "COpcCommon.h"
#include "COpcConnectionPoint.h"
#include "COpcCPContainer.h"
#include "COpcEnumCPs.h"
#include "COpcEnumString.h"
#include "COpcEnumUnknown.h"
#include "COpcSecurity.h"
#include "COpcThreadPool.h"
#include "COpcBrowseElement.h"

#endif // _OpcUtils_H_

