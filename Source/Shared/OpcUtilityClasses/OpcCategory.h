/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

#ifndef _OpcCategory_H_
#define _OpcCategory_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcComObject.h"
#include "COpcList.h"

//==============================================================================
// FUNCTION: OpcEnumServers
// PURPOSE:  Enumerates servers in the specified category on the host.
 
// OpcEnumServers
OPCUTILS_API HRESULT OpcEnumServersInCategory(
    LPCTSTR          tsHostName,
    const CATID&     tCategory,
    COpcList<CLSID>* pServers 
);

//==============================================================================
// FUNCTION: RegisterClsidInCategory
// PURPOSE:  Registers a CLSID as belonging to a component category. 
 
HRESULT RegisterClsidInCategory(REFCLSID clsid, CATID catid, LPCWSTR szDescription) ;

//==============================================================================
// FUNCTION: UnregisterClsidInCategory
// PURPOSE:  Unregisters a CLSID as belonging to a component category. 
HRESULT UnregisterClsidInCategory(REFCLSID clsid, CATID catid);

//==============================================================================
// STRUCT:  TClassCategories
// PURPOSE: Associates a clsid with a component category. 

struct TClassCategories 
{
    const CLSID* pClsid;
    const CATID* pCategory;
	const TCHAR* szDescription;
};

//==============================================================================
// MACRO:   OPC_BEGIN_CATEGORY_TABLE
// PURPOSE: Begins the module class category table.

#define OPC_BEGIN_CATEGORY_TABLE() static const TClassCategories g_pCategoryTable[] = {

//==============================================================================
// MACRO:   OPC_CATEGORY_TABLE_ENTRY
// PURPOSE: An entry in the module class category table.

#define OPC_CATEGORY_TABLE_ENTRY(xClsid, xCatid, xDescription) {&(__uuidof(xClsid)), &(xCatid), (xDescription)},

//==============================================================================
// MACRO:   OPC_END_CATEGORY_TABLE
// PURPOSE: Ends the module class category table.

#define OPC_END_CATEGORY_TABLE() {NULL, NULL, NULL}};

#endif // _OpcCategory_H_
