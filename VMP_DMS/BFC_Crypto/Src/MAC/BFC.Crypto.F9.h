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
//	BFC.Crypto.F9.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_F9_H_
#define _BFC_Crypto_F9_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, F9 )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.Cipher.h"
#include "BFC.Crypto.MAC.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL F9 : public MAC {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new F9.

	F9(
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

	/// \brief Processes data through F9.
	///
	/// \param pData
	///	The data to send through F9.

	virtual void process(
		const	Buffer &	pData
	);

	/// \brief Terminates an F9 session.
	///
	/// \param pTag
	///	The destination of the F9 authentication tag.

	virtual void done(
			Buffer &	pTag
	);

	/// \brief Performs F9 self-test.

	virtual void test(
	);

protected :

private :

	CipherPtr	cipher;
	Uint32		blkSze;
	Buffer		key2;
	Buffer		ACC;
	Buffer		IV;
	Buffer		buf;

	/// \brief Forbidden copy constructor.

	F9(
		const	F9 &
	);

	/// \brief Forbidden copy operator.

	F9 & operator = (
		const	F9 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_F9_H_

// ============================================================================

