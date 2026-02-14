//==============================================================================
// TITLE: COpcText.cpp
//
// CONTENTS:
// 
// A class that defines a text element to read from text buffer.
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

#include "StdAfx.h"
#include "COpcText.h"

//==============================================================================
// COpcText

// Constructor
COpcText::COpcText()
{
   Reset();
}

// Reset
void COpcText::Reset()
{
   m_cData.Empty();

   // Search Crtieria
   m_eType = COpcText::NonWhitespace;
   m_cHaltChars.Empty();
   m_uMaxChars = 0;
   m_bNoExtract = false;
   m_cText.Empty();
   m_bSkipLeading = false;
   m_bSkipWhitespace = false;
   m_bIgnoreCase = false;
   m_bEofDelim = false;
   m_bNewLineDelim = false;
   m_cDelims.Empty();
   m_bLeaveDelim = true;
   m_zStart = L'"';
   m_zEnd = L'"';
   m_bAllowEscape = true;

   // Search Results
   m_uStart = 0;
   m_uEnd = 0;
   m_zHaltChar = 0;
   m_uHaltPos = 0;
   m_zDelimChar = 0;
   m_bEof = false;
   m_bNewLine = false;
}

// CopyData
void COpcText::CopyData(LPCWSTR szData, UINT uLength)
{
    m_cData.Empty();

    if (uLength > 0 && szData != NULL)
    {
        LPWSTR wszData = OpcArrayAlloc(WCHAR, uLength+1);
        wcsncpy(wszData, szData, uLength);
        wszData[uLength] = L'\0';
        
        m_cData = wszData;
        OpcFree(wszData);
    }
}

// SetType
void COpcText::SetType(COpcText::Type eType)
{
   Reset();

   m_eType = eType;

   switch (eType)
   {
      case Literal:
      {
         m_cText.Empty();
         m_bSkipLeading = false;
         m_bSkipWhitespace = true;
         m_bIgnoreCase = false;
         break;
      }

      case Whitespace:
      {
         m_bSkipLeading = false;
         m_bEofDelim = true;
         break;
      }

      case NonWhitespace:
      {
         m_bSkipWhitespace = true;
         m_bEofDelim = true;
         break;
      }

      case Delimited:
      {
         m_cDelims.Empty();
         m_bSkipWhitespace = false;
         m_bIgnoreCase = false;
         m_bEofDelim = false;
         m_bNewLineDelim = false;
         m_bLeaveDelim = false;
         break;
      }

      default:
      {
         break;
      }
   }
}
