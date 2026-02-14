/* ========================================================================
 * Copyright (c) 2002-2026 OPC Foundation. All rights reserved.
 *
 * OPC Foundation MIT License 1.00
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * The complete license agreement can be found here:
 * http://opcfoundation.org/License/MIT/1.00/
 * ======================================================================*/

#ifndef _COpcDaBuffer_H_
#define _COpcDaBuffer_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//============================================================================
// MACROS:  OPC_MAX_BUF_SIZE
// PURPOSE: Defines the maximum buffer size,

#define OPC_MAX_BUF_SIZE 16

//============================================================================
// TYPE:    COpcDaBuffer
// PURPOSE: Contains a rotating buffer of item values.

class COpcDaBuffer
{
    OPC_CLASS_NEW_DELETE()

public:

    // Constructor
    COpcDaBuffer(DWORD dwSize = 1)
    {
        m_dwSize    = 0;
        m_dwFirst   = 0;
        m_dwLast    = 0;
        m_pValues   = NULL;
        m_pErrors   = NULL;
        m_bOverflow = false;

        Alloc(dwSize);
    }

    // Destructor
    ~COpcDaBuffer()
    {
        Alloc(0);
    }

    // GetOverflow
    bool GetOverflow() const { return m_bOverflow; }

    // Clear
    void Clear()
    {
        for (DWORD ii = 0; ii < m_dwSize; ii++)
        {
            OpcVariantClear(&m_pValues[ii].vDataValue);
        }

        memset(m_pValues, 0, m_dwSize*sizeof(OPCITEMSTATE));
        memset(m_pErrors, 0, m_dwSize*sizeof(HRESULT));

        m_dwFirst   = 0;
        m_dwLast    = 0;
        m_bOverflow = false;
    }

    // Alloc
    void Alloc(DWORD dwSize)
    {
        Clear();

        OpcFree(m_pValues);
        OpcFree(m_pErrors);
        
        m_dwSize  = dwSize;
        m_pValues = NULL;
        m_pErrors = NULL;

        if (m_dwSize > 0)
        {
            m_pValues = OpcArrayAlloc(OPCITEMSTATE, m_dwSize); 
            m_pErrors = OpcArrayAlloc(HRESULT, m_dwSize); 

            memset(m_pValues, 0, m_dwSize*sizeof(OPCITEMSTATE));
            memset(m_pErrors, 0, m_dwSize*sizeof(HRESULT));
        }
    }

    // Grow
    void Grow(DWORD dwSize)
    {
        if (m_dwSize - GetCount() >= dwSize)
        {
            return;
        }

        dwSize = dwSize + GetCount();

        OPCITEMSTATE* pValues = OpcArrayAlloc(OPCITEMSTATE, dwSize); 
        HRESULT*      pErrors = OpcArrayAlloc(HRESULT, dwSize); 

        memset(pValues, 0, dwSize*sizeof(OPCITEMSTATE));
        memset(pErrors, 0, dwSize*sizeof(HRESULT));

        for (DWORD ii = 0; ii < GetCount(); ii++)
        {
            memcpy(&(pValues[ii]), &((*this)[ii]), sizeof(OPCITEMSTATE));
            pErrors[ii] = Error(ii);
        }       
        
        OpcFree(m_pValues);
        OpcFree(m_pErrors);

        m_dwSize  = dwSize;
        m_pValues = pValues;
        m_pErrors = pErrors;
    }

    // Append
    void Append(OPCITEMSTATE& cValue, HRESULT hResult)
    {
        // check for buffer overflow.
        if (m_dwLast == m_dwSize)
        {
            m_bOverflow = true;        
            m_dwLast      = 0;
        }

        // remove oldest value.
        if (m_bOverflow)
        {
            OpcVariantClear(&m_pValues[m_dwFirst++].vDataValue);
            
            if (m_dwFirst == m_dwSize)
            {
                m_dwFirst = 0;
            }
        }

        // take ownship of item value.
        memcpy(&m_pValues[m_dwLast], &cValue, sizeof(OPCITEMSTATE));
        memset(&cValue, 0, sizeof(OPCITEMSTATE));

        m_pErrors[m_dwLast] = hResult;
        m_dwLast++;
    }

    // Append
    void Append(COpcDaBuffer& cBuffer)
    {
        DWORD dwCount = cBuffer.GetCount();

        if (dwCount == 0)
        {
            return;
        }

        Grow(dwCount);

        for (DWORD ii = 0; ii < dwCount; ii++)
        {
            Append(cBuffer[ii], cBuffer.Error(ii));
        }     

        cBuffer.Clear();
    }

    // GetCount
    DWORD GetCount() const 
    {
        if (m_dwLast > m_dwFirst)
        {
            return (m_dwLast - m_dwFirst);
        }

        if (m_dwLast == m_dwFirst)
        {
            return (m_bOverflow)?m_dwSize:0;
        }

        return m_dwSize - (m_dwFirst - m_dwLast); 
    }

    // Pop
    void Pop() 
    {
        OpcVariantClear(&m_pValues[m_dwFirst++].vDataValue);
        
		// check if buffer is now empty.
		if (m_dwFirst == m_dwLast)
		{
			m_dwFirst = 0;
			m_dwLast  = 0;
		}

		// check for wrap around.
        else if (m_dwFirst == m_dwSize)
        {
            m_dwFirst = 0;
        }
    }

    // Index Operator
    OPCITEMSTATE& operator[](DWORD dwIndex) 
    {
        OPC_ASSERT(dwIndex < GetCount());

        if (dwIndex < (m_dwSize - m_dwFirst))
        {
            return m_pValues[dwIndex + m_dwFirst];
        }

        return m_pValues[dwIndex - (m_dwSize - m_dwFirst)];
    }

    // Indexer for the errors.
    HRESULT& Error(DWORD dwIndex) 
    {
        OPC_ASSERT(dwIndex < GetCount());

        if (dwIndex < (m_dwSize - m_dwFirst))
        {
            return m_pErrors[dwIndex + m_dwFirst];
        }

        return m_pErrors[dwIndex - (m_dwSize - m_dwFirst)];
    }

private:
  
    DWORD         m_dwSize;
    DWORD         m_dwFirst;
    DWORD         m_dwLast;
    OPCITEMSTATE* m_pValues;
    HRESULT*      m_pErrors;
    bool          m_bOverflow;
};

#endif // _COpcDaBuffer_H_
