/* ========================================================================
 * SPDX-FileCopyrightText: Copyright (c) 2005-2026, OPC Federation AISBL
 * SPDX-License-Identifier: MIT
 * ======================================================================*/

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
