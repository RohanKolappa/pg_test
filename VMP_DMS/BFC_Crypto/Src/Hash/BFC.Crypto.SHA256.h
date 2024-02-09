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
//	BFC.Crypto.SHA256.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_SHA256_H_
#define _BFC_Crypto_SHA256_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, SHA256 )

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

class BFC_Crypto_DLL SHA256 : public Hash {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SHA256.

	SHA256(
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

	static Uint32 Ch(
		const	Uint32		x,
		const	Uint32		y,
		const	Uint32		z
	) {
		return (z ^ (x & (y ^ z)));
	}

	static Uint32 Maj(
		const	Uint32		x,
		const	Uint32		y,
		const	Uint32		z
	) {
		return (((x | y) & z) | (x & y));
	}

	static Uint32 R(
		const	Uint32		x,
		const	Uint32		n
	) {
		return ( x >> n );
	}

	static Uint32 S(
		const	Uint32		x,
		const	Uint32		n
	) {
		return ROR( x, n );
	}

	static Uint32 Sigma0(
		const	Uint32		x
	) {
		return (S(x, 2) ^ S(x, 13) ^ S(x, 22));
	}

	static Uint32 Sigma1(
		const	Uint32		x
	) {
		return (S(x, 6) ^ S(x, 11) ^ S(x, 25));
	}

	static Uint32 Gamma0(
		const	Uint32		x
	) {
		return (S(x, 7) ^ S(x, 18) ^ R(x, 3));
	}

	static Uint32 Gamma1(
		const	Uint32		x
	) {
		return (S(x, 17) ^ S(x, 19) ^ R(x, 10));
	}

private :

	friend class SHA224;

	SHA256(
		const	DL::TypeCPtr &	pType
	);

	static const Uint32 K[64];

	Buffer	msgBuf;
	Uint64	msgLen;
	Uint32	state[8];

	/// \brief Forbidden copy constructor.

	SHA256(
		const	SHA256 &
	);

	/// \brief Forbidden copy operator.

	SHA256 & operator = (
		const	SHA256 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_SHA256_H_

// ============================================================================

