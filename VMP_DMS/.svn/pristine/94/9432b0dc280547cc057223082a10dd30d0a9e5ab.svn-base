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
//	BFC.Crypto.MD5.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_MD5_H_
#define _BFC_Crypto_MD5_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, MD5 )

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

class BFC_Crypto_DLL MD5 : public Hash {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new MD5.

	MD5(
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

	static Uint32 F( const Uint32 x, const Uint32 y, const Uint32 z) { return (z ^ (x & (y ^ z))); }
	static Uint32 G( const Uint32 x, const Uint32 y, const Uint32 z) { return (y ^ (z & (y ^ x))); }
	static Uint32 H( const Uint32 x, const Uint32 y, const Uint32 z) { return (x^y^z); }
	static Uint32 I( const Uint32 x, const Uint32 y, const Uint32 z) { return (y^(x|(~z))); }

	static void FF( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += F(b,c,d) + M + t; a = ROL(a, s) + b; }
	static void GG( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += G(b,c,d) + M + t; a = ROL(a, s) + b; }
	static void HH( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += H(b,c,d) + M + t; a = ROL(a, s) + b; }
	static void II( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += I(b,c,d) + M + t; a = ROL(a, s) + b; }

private :

	Buffer	msgBuf;
	Uint32	state[4];
	Uint64	msgLen;

	/// \brief Forbidden copy constructor.

	MD5(
		const	MD5 &
	);

	/// \brief Forbidden copy operator.

	MD5 & operator = (
		const	MD5 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_MD5_H_

// ============================================================================

