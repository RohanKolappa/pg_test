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
//	BFC.Crypto.XCBC.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_XCBC_H_
#define _BFC_Crypto_XCBC_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, XCBC )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Cipher.h"
#include "BFC.Crypto.MAC.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL XCBC : public MAC {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new XCBC.

	XCBC(
	);

	/// \brief Initializes this object.
	///
	/// \param pCipher
	///	The cipher you want to use.
	///
	/// \param pKey
	///	The secret key.

	void init(
			CipherPtr	pCipher,
		const	Buffer &	pKey
	);

	/// \brief Processes data through XCBC.
	///
	/// \param pData
	///	The data to send through XCBC.

	virtual void process(
		const	Buffer &	pData
	);

	/// \brief Terminates an XCBC session.
	///
	/// \param pTag
	///	The destination of the XCBC authentication tag.

	virtual void done(
			Buffer &	pTag
	);

	/// \brief Performs XCBC self-test.

	virtual void test(
	);

protected :

private :

	CipherPtr	cipher;
	Uint32		blkSze;
	Buffer		K[ 3 ];
	Buffer		IV;
	Buffer		tmpBuf;

	/// \brief Forbidden copy constructor.

	XCBC(
		const	XCBC &
	);

	/// \brief Forbidden copy operator.

	XCBC & operator = (
		const	XCBC &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_XCBC_H_

// ============================================================================

