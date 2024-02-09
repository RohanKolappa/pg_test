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
//	BFC.Crypto.RIPEMD320.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_RIPEMD320_H_
#define _BFC_Crypto_RIPEMD320_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, RIPEMD320 )

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

class BFC_Crypto_DLL RIPEMD320 : public Hash {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new RIPEMD320.

	RIPEMD320(
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

private :

	Uint32	state[ 10 ];
	Buffer	msgBuf;
	Uint64	msgLen;

	/// \brief Forbidden copy constructor.

	RIPEMD320(
		const	RIPEMD320 &
	);

	/// \brief Forbidden copy operator.

	RIPEMD320 & operator = (
		const	RIPEMD320 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_RIPEMD320_H_

// ============================================================================

