//==============================================================================
// TITLE: COpcFile.h
//
// CONTENTS:
// 
// A class that provides basic file I/O functions.
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

#ifndef _COpcFile_H_
#define _COpcFile_H_

#include "COpcString.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//==============================================================================
// CLASS:   COpcFile
// PURPOSE  Facilitiates manipulation of XML Elements,

class COpcFile
{
    OPC_CLASS_NEW_DELETE();

public:

    //==========================================================================
    // Public Operators

    // Constructor
    COpcFile();
            
    // Destructor
    ~COpcFile();

    //==========================================================================
    // Public Methods

	// Create
	bool Create(const COpcString& cFileName);

	// Open
	bool Open(const COpcString& cFileName, bool bReadOnly = true);

	// Close
	void Close();

	// Read
	UINT Read(BYTE* pBuffer, UINT uSize);

	// Write
	UINT Write(BYTE* pBuffer, UINT uSize);
	
	// GetFileSize
	UINT GetFileSize();

	// GetLastModified
	FILETIME GetLastModified();

	// GetMemoryMapping
	BYTE* GetMemoryMapping();

private:

    //==========================================================================
    // Private Members

	HANDLE m_hFile;
	HANDLE m_hMapping;
	BYTE*  m_pView;
	bool   m_bReadOnly;
};

#endif // _COpcFile_H_ 
