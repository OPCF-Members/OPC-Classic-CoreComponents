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

#ifndef _COpcDaEnumGroup_H_
#define _COpcDaEnumGroup_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//============================================================================
// CLASS:   COpcDaEnumGroup
// PURPOSE: A class to implement the IEnumString interface.
// NOTES:

class COpcDaEnumGroup 
:
    public COpcComObject,
    public IEnumOPCItemAttributes
{     
    OPC_BEGIN_INTERFACE_TABLE(COpcDaEnumGroup)
        OPC_INTERFACE_ENTRY(IEnumOPCItemAttributes)
    OPC_END_INTERFACE_TABLE()

    OPC_CLASS_NEW_DELETE()

public:

    //========================================================================
    // Operators

    // Constructor
    COpcDaEnumGroup();

    // Constructor
    COpcDaEnumGroup(UINT uCount, OPCITEMATTRIBUTES* pItems);

    // Destructor 
    ~COpcDaEnumGroup();

    //========================================================================
    // IEnumOPCItemAttributes

    // Next
	STDMETHODIMP Next( 
		ULONG               celt,
		OPCITEMATTRIBUTES** ppItemArray,
		ULONG*              pceltFetched 
	);

    // Skip
	STDMETHODIMP Skip(ULONG celt);

    // Reset
	STDMETHODIMP Reset();

    // Clone
	STDMETHODIMP Clone(IEnumOPCItemAttributes** ppEnumGroupAttributes);

private:

	//=========================================================================
    // Private Methods

	// Init
	void Init(OPCITEMATTRIBUTES& cAttributes);

	// Clear
	void Clear(OPCITEMATTRIBUTES& cAttributes);

	// Copy
	void Copy(OPCITEMATTRIBUTES& cDst, OPCITEMATTRIBUTES& cSrc);

    //=========================================================================
    // Private Members

    UINT			   m_uIndex;
    UINT			   m_uCount;
    OPCITEMATTRIBUTES* m_pItems;
};

#endif // _COpcDaEnumGroup_H_
