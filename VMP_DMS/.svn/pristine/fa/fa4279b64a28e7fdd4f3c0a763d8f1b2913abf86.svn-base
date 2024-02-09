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
//	BFC.Crypto.SHA512.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_SHA512_H_
#define _BFC_Crypto_SHA512_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, SHA512 )

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

class BFC_Crypto_DLL SHA512 : public Hash {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SHA512.

	SHA512(
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
	);

	static Uint64 Ch(
		const	Uint64		x,
		const	Uint64		y,
		const	Uint64		z
	) {
		return (z ^ (x & (y ^ z)));
	}

	static Uint64 Maj(
		const	Uint64		x,
		const	Uint64		y,
		const	Uint64		z
	) {
		return (((x | y) & z) | (x & y));
	}

	static Uint64 R(
		const	Uint64		x,
		const	Uint32		n
	) {
		return ( x >> n );
	}

	static Uint64 S(
		const	Uint64		x,
		const	Uint32		n
	) {
		return ROR64( x, n );
	}

	static Uint64 Sigma0(
		const	Uint64		x
	) {
		return (S(x, 28) ^ S(x, 34) ^ S(x, 39));
	}

	static Uint64 Sigma1(
		const	Uint64		x
	) {
		return (S(x, 14) ^ S(x, 18) ^ S(x, 41));
	}

	static Uint64 Gamma0(
		const	Uint64		x
	) {
		return (S(x, 1) ^ S(x, 8) ^ R(x, 7));
	}

	static Uint64 Gamma1(
		const	Uint64		x
	) {
		return (S(x, 19) ^ S(x, 61) ^ R(x, 6));
	}

private :

	friend class SHA384;

	SHA512(
		const	DL::TypeCPtr &	pType
	);

	static const Uint64 K[80];

	Buffer	msgBuf;
	Uint64	msgLen;
	Uint64	state[ 8 ];

	/// \brief Forbidden copy constructor.

	SHA512(
		const	SHA512 &
	);

	/// \brief Forbidden copy operator.

	SHA512 & operator = (
		const	SHA512 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_SHA512_H_

// ============================================================================

