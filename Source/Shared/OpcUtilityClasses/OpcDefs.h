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

#ifndef _OpcDefs_H_
#define _OpcDefs_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <crtdbg.h>
#include <malloc.h>

//==============================================================================
// MACRO:   OPC_ASSERT
// PURPOSE: Halts program execution if condition is not met.

#ifdef _DEBUG
extern void OpcAssert(bool bCondition, LPCWSTR szMessage);
#define OPC_ASSERT(x) OpcAssert(x, L#x)
#else
#define OPC_ASSERT(x)
#endif

//==============================================================================
// MACRO:   TRY, LEAVE. FINALLY
// PURPOSE: Macros to facilitate cleanup after error conditions.

#pragma warning(disable: 4102) 

#ifndef OPC_NO_EXCEPTIONS

#define TRY try 
#define CATCH catch (DWORD)
#define CATCH_FINALLY catch (DWORD) {} finally:
#define FINALLY finally:
#define THROW() throw (DWORD)0;
#define THROW_(xResult, xValue) { (xResult) = (xValue); throw (DWORD)0; }
#define GOTOFINALLY() goto finally;

#else

#define TRY {    
#define CATCH } goto finally; onerror:
#define CATCH_FINALLY onerror: finally:
#define FINALLY finally:
#define THROW() goto onerror;
#define THROW_(xResult, xValue) { (xResult) = (xValue); goto onerror; }
#define GOTOFINALLY() goto finally;

#endif

//==============================================================================
// FUNCTION: OpcAlloc
// PURPOSE:  Allocates a block of memory.

OPCUTILS_API void* OpcAlloc(size_t tSize);

//==============================================================================
// FUNCTION: OpcFree
// PURPOSE:  Frees a block of memory.

OPCUTILS_API void OpcFree(void* pBlock);

//==============================================================================
// MACRO:   OpcArrayAlloc
// PURPOSE: Allocates an array of simple data (i.e. not classes).

#define OpcArrayAlloc(xType,xLength) (xType*)OpcAlloc((xLength)*sizeof(xType))

//==============================================================================
// FUNCTION: OpcStrDup
// PURPOSE:  Duplicates a string.

OPCUTILS_API CHAR* OpcStrDup(LPCSTR szValue);
OPCUTILS_API WCHAR* OpcStrDup(LPCWSTR szValue);

//==============================================================================
// FUNCTION: OpcArrayDup
// PURPOSE:  Duplicates an array of types (bitwise copy must be valid).

#define OpcArrayDup(xCopy, xType, xLength, xValue) \
{ \
    UINT uLength = (xLength)*sizeof(xType); \
    xCopy = (xType*)OpcAlloc(uLength); \
 \
    if (xValue != NULL) memcpy(xCopy, xValue, uLength); \
    else memset(xCopy, 0, uLength); \
}

//==============================================================================
// MACRO:   OPC_CLASS_NEW_DELETE
// PURPOSE: Implements a class new and delete operators.

#define OPC_CLASS_NEW_DELETE() \
public: void* operator new(size_t nSize) {return OpcAlloc(nSize);} \
public: void operator delete(void* p) {OpcFree(p);}

#define OPC_CLASS_NEW_DELETE_EX(xCallType) \
xCallType void* operator new(size_t nSize) {return OpcAlloc(nSize);} \
xCallType void operator delete(void* p) {OpcFree(p);}

//==============================================================================
// MACRO:   OPC_CLASS_NEW_DELETE_ARRAY
// PURPOSE: Implements a class new and delete instance and array operators.

#define OPC_CLASS_NEW_DELETE_ARRAY() \
OPC_CLASS_NEW_DELETE() \
public: void* operator new[](size_t nSize) {return OpcAlloc(nSize);} \
public:  void operator delete[](void* p) {OpcFree(p);}

#define OPC_CLASS_NEW_DELETE_ARRAY_EX(xCallType) \
OPC_CLASS_NEW_DELETE_EX(xCallType) \
xCallType void* operator new[](size_t nSize) {return OpcAlloc(nSize);} \
xCallType void operator delete[](void* p) {OpcFree(p);}

//==============================================================================
// FUNCTION: OpcUtcNow
// PURPOSE:  Returns the current UTC time 

OPCUTILS_API FILETIME OpcUtcNow();

//==============================================================================
// FUNCTION: OpcMinDate
// PURPOSE:  Returns the minimum value for a file time.

OPCUTILS_API FILETIME OpcMinDate();

//==============================================================================
// FUNCTION: OpcToInt64
// PURPOSE:  Converts a FILETIME to a 64-bit integer.

OPCUTILS_API LONGLONG OpcToInt64(FILETIME ftTime);

//==============================================================================
// FUNCTION: OpcToFILETIME
// PURPOSE:  Converts a 64-bit integer to a FILETIME.

OPCUTILS_API FILETIME OpcToFILETIME(LONGLONG llTime);

//============================================================================
// FUNCTION: OpcLocalTimeToUtcTime
// PURPOSE:  Converts a local time to a UTC time.

OPCUTILS_API bool OpcLocalTimeToUtcTime(FILETIME& ftLocalTime, FILETIME& ftUtcTime);
OPCUTILS_API bool OpcLocalTimeToUtcTime(SYSTEMTIME& stLocalTime, FILETIME& ftUtcTime);

//============================================================================
// FUNCTION: OpcGetDaysInMonth
// PURPOSE:  Returns the number of days in a month.

OPCUTILS_API UINT OpcGetDaysInMonth(UINT uYear, UINT uMonth);

//==============================================================================
// FUNCTION: operator
// PURPOSE:  Implements various operators for FILETIME structures.

inline bool operator==(const FILETIME& a, const FILETIME& b) 
{
	return (memcmp(&a, &b, sizeof(FILETIME)) == 0); 
}

inline bool operator!=(const FILETIME& a, const FILETIME& b) 
{ 
	return (memcmp(&a, &b, sizeof(FILETIME)) != 0); 
}

inline bool operator<(const FILETIME& a, const FILETIME& b)
{
	return (a.dwHighDateTime != b.dwHighDateTime)?(a.dwHighDateTime < b.dwHighDateTime):(a.dwLowDateTime < b.dwLowDateTime);
}

inline bool operator<=(const FILETIME& a, const FILETIME& b)
{
	return (a.dwHighDateTime != b.dwHighDateTime)?(a.dwHighDateTime <= b.dwHighDateTime):(a.dwLowDateTime <= b.dwLowDateTime);
}

inline bool operator>(const FILETIME& a, const FILETIME& b)
{
	return (a.dwHighDateTime != b.dwHighDateTime)?(a.dwHighDateTime > b.dwHighDateTime):(a.dwLowDateTime > b.dwLowDateTime);
}

inline bool operator>=(const FILETIME& a, const FILETIME& b)
{
	return (a.dwHighDateTime != b.dwHighDateTime)?(a.dwHighDateTime >= b.dwHighDateTime):(a.dwLowDateTime >= b.dwLowDateTime);
}

//==============================================================================
// FUNCTION: operator
// PURPOSE:  Implements various operators for CY structures.

inline bool operator==(const CY& a, const CY& b) 
{
	return (a.int64 == b.int64); 
}

inline bool operator!=(const CY& a, const CY& b) 
{ 
	return (a.int64 != b.int64); 
}

inline bool operator<(const CY& a, const CY& b)
{
	return (a.int64 < b.int64); 
}

inline bool operator<=(const CY& a, const CY& b)
{
	return (a.int64 <= b.int64); 
}

inline bool operator>(const CY& a, const CY& b)
{
	return (a.int64 > b.int64); 
}

inline bool operator>=(const CY& a, const CY& b)
{
	return (a.int64 >= b.int64); 
}

//==============================================================================
// MACRO:    OPC_MASK_XXX
// PURPOSE:  Facilitate manipulation of bit masks.

#define OPC_MASK_ISSET(xValue, xMask) (((xValue) & (xMask)) == (xMask))
#define OPC_MASK_SET(xValue, xMask)   xValue |= (xMask)
#define OPC_MASK_UNSET(xValue, xMask) xValue &= ~(xMask)

#endif // _OpcDefs_H_

