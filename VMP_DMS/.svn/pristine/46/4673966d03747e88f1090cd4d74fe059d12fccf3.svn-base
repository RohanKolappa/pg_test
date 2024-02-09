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
//	BFC.Crypto.Whirl.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_Whirl_H_
#define _BFC_Crypto_Whirl_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Whirl )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Functions.h"
#include "BFC.Crypto.Hash.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Whirl : public Hash {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Whirl.

	Whirl(
	);

	virtual void init(
	);

	virtual void process(
		const	Buffer &	pData
	);

	virtual void done(
			Buffer &	out
	);

	virtual void test(
	);

protected :

	void compress(
		const	Uchar *		pData
	);

	static Uint32 GB(
		const	Uint64 *	a,
		const	Uint32		i,
		const	Uint32		j
	) {
		return ( ( a[ i & 7 ] >> ( 8 * j ) ) & 255 );
	}


	static Uint64 theta_pi_gamma(
		const	Uint64 *	a,
		const	Uint32		i
	) {
		return sbox0[ GB( a, i-0, 7 ) ]
		     ^ sbox1[ GB( a, i-1, 6 ) ]
		     ^ sbox2[ GB( a, i-2, 5 ) ]
		     ^ sbox3[ GB( a, i-3, 4 ) ]
		     ^ sbox4[ GB( a, i-4, 3 ) ]
		     ^ sbox5[ GB( a, i-5, 2 ) ]
		     ^ sbox6[ GB( a, i-6, 1 ) ]
		     ^ sbox7[ GB( a, i-7, 0 ) ];
	}

private :

	static const Uint64 cont[];
	static const Uint64 sbox0[];
	static const Uint64 sbox1[];
	static const Uint64 sbox2[];
	static const Uint64 sbox3[];
	static const Uint64 sbox4[];
	static const Uint64 sbox5[];
	static const Uint64 sbox6[];
	static const Uint64 sbox7[];

	Uint64	state[ 8 ];
	Buffer	msgBuf;
	Uint64	msgLen;

	/// \brief Forbidden copy constructor.

	Whirl(
		const	Whirl &
	);

	/// \brief Forbidden copy operator.

	Whirl & operator = (
		const	Whirl &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Whirl_H_

// ============================================================================

