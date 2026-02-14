//==============================================================================
// TITLE: COpcTextReader.h
//
// CONTENTS:
// 
// A class that parses a stream of text.
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

#ifndef _COpcTextReader_H
#define _COpcTextReader_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpcDefs.h"
#include "COpcText.h"

//==============================================================================
// CLASS:   COpcTextReader
// PURPOSE: Extracts tokens from a stream.

class OPCUTILS_API COpcTextReader
{
    OPC_CLASS_NEW_DELETE();

public:

    //==========================================================================
    // Operators

    // Constructor
    COpcTextReader(const COpcString& cBuffer);  
    COpcTextReader(LPCSTR szBuffer, UINT uLength = -1);  
    COpcTextReader(LPCWSTR szBuffer, UINT uLength = -1);  
 
    // Destructor
    ~COpcTextReader(); 

    //==========================================================================
    // Public Methods
  
    // GetNext
    bool GetNext(COpcText& cText);

    // GetBuf
    LPCWSTR GetBuf() const { return m_szBuf; }

private:

    //==========================================================================
    // Private Methods

    // ReadData
    bool ReadData();

    // FindToken
    bool FindToken(COpcText& cText);

    // FindLiteral
    bool FindLiteral(COpcText& cText);

    // FindNonWhitespace
    bool FindNonWhitespace(COpcText& cText);

    // FindWhitespace
    bool FindWhitespace(COpcText& cText);
    
    // FindDelimited
    bool FindDelimited(COpcText& cText);

    // FindEnclosed
    bool FindEnclosed(COpcText& cText);

    // CheckForHalt
    bool CheckForHalt(COpcText& cText, UINT uIndex);
    
    // CheckForDelim
    bool CheckForDelim(COpcText& cText, UINT uIndex);

    // SkipWhitespace
    UINT SkipWhitespace(COpcText& cText);

    // CopyData
    void CopyData(COpcText& cText, UINT uStart, UINT uEnd);

    //==========================================================================
    // Private Members

    LPWSTR m_szBuf;
    UINT   m_uLength;
    UINT   m_uEndOfData;
};

#endif //ndef _COpcTextReader_H
