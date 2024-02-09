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
//	BFC.Crypto.Yarrow.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_Yarrow_H_
#define _BFC_Crypto_Yarrow_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, Yarrow )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Base.FastCS.h"

#include "BFC.Crypto.Cipher.h"
#include "BFC.Crypto.Hash.h"
#include "BFC.Crypto.Mode.h"
#include "BFC.Crypto.PRNG.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL Yarrow : public PRNG {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Yarrow.

	Yarrow(
	);

	virtual void start(
	);

	virtual void add_entropy(
		const	Buffer &	pData
	);

	virtual void ready(
	);

	virtual void done(
	);

	virtual Uint32 read(
			Buffer &	pBuffer
	);

	virtual Buffer exportState(
	);

	virtual void importState(
		const	Buffer &	pState
	);

	virtual void test(
	);

protected :

private :

	FastCS			cs;
	CipherPtr		cipher;
	HashPtr			hash;
	Buffer			pool;
	ModePtr			ctr;

	/// \brief Forbidden copy constructor.

	Yarrow(
		const	Yarrow &
	);

	/// \brief Forbidden copy operator.

	Yarrow & operator = (
		const	Yarrow &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Yarrow_H_

// ============================================================================

