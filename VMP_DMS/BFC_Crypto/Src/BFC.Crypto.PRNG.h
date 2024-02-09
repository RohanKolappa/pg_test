// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Crypto.PRNG.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_PRNG_H_
#define _BFC_Crypto_PRNG_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Crypto.PRNGPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "BFC.Crypto.PRNGDescriptor.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL PRNG : public DL::Object {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new PRNG.

	PRNG(
		const	DL::TypeCPtr &	pType
	) :
		DL::Object	( pType ),
		descr		( pType->getData() ) {
	}

public :

	void rng_make_prng(
		const	Uint32		bits
	);

	/// \brief Starts this PRNG.

	virtual void start(
	) = 0;

	/// \brief Adds entropy to this PRNG.

	virtual void add_entropy(
		const	Buffer &	pData
	) = 0;

	/// \brief Ready this PRNG state to read from.

	virtual void ready(
	) = 0;

	/** Read from the PRNG
		@param out	[out] Where to store the data
		@param outlen  Length of data desired (octets)
		@param prng	The PRNG state to read from
		@return Number of octets read
	*/

	virtual Buffer read(
		const	Uint32		pLength
	);

	virtual Uint32 read(
			Buffer &	pBuffer
	) = 0;

	/** Terminate a PRNG state
		@param prng	The PRNG state to terminate
		@return CRYPT_OK if successful
	*/

	virtual void done(
	);

	/** Export a PRNG state  
		@param out	[out] The destination for the state
		@param outlen  [in/out] The max size and resulting size of the PRNG state
		@param prng	The PRNG to export
		@return CRYPT_OK if successful
	*/

	virtual Buffer exportState(
	) = 0;

	/** Import a PRNG state
		@param in		The data to import
		@param inlen	The length of the data to import (octets)
		@param prng	The PRNG to initialize/import
		@return CRYPT_OK if successful
	*/

	virtual void importState(
		const	Buffer &	pState
	) = 0;

	/// \brief Self-test.

	virtual void test(
	) = 0;

private :

	PRNGDescriptorCPtr	descr;

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_PRNG_H_

// ============================================================================

