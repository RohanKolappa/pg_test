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
//	BFC.Crypto.PKCS5.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_PKCS5_H_
#define _BFC_Crypto_PKCS5_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, PKCS5 )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL PKCS5 : virtual public SObject {

public :

	/// \brief Creates a new PKCS5.

	PKCS5(
	);

	/// \brief Destroys this object.

	virtual ~PKCS5(
	);

	/// \brief Executes PKCS #5 v2.
	///
	/// \param password			 The input password (or key)
	/// \param password_len		The length of the password (octets)
	/// \param salt				  The salt (or nonce)
	/// \param salt_len			 The length of the salt (octets)
	/// \param iteration_count	# of iterations desired for PKCS #5 v2 [read specs for more]
	/// \param hash_idx			 The index of the hash desired
	/// \param out					[out] The destination for this algorithm
	/// \param outlen				[in/out] The max size and resulting size of the algorithm output

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Crypto.PKCS5" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

	/// \brief Forbidden copy constructor.

	PKCS5(
		const	PKCS5 &
	);

	/// \brief Forbidden copy operator.

	PKCS5 & operator = (
		const	PKCS5 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_PKCS5_H_

// ============================================================================

