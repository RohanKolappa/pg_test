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
//	BFC.Crypto.SHA224.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_SHA224_H_
#define _BFC_Crypto_SHA224_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Crypto {

BFC_PTR_DECL( BFC_Crypto_DLL, SHA224 )

} // namespace Crypto
} // namespace BFC

// ============================================================================

#include "BFC.Crypto.SHA256.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief SHA-224 new NIST standard based off of SHA-256 truncated to 224 bits.
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL SHA224 : public SHA256 {

public :

	/// \brief Returns the identity of this class.

	static const DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SHA224.

	SHA224(
	);

	virtual void init(
	);

	virtual void test(
	);

protected :

private :

	/// \brief Forbidden copy constructor.

	SHA224(
		const	SHA224 &
	);

	/// \brief Forbidden copy operator.

	SHA224 & operator = (
		const	SHA224 &
	);

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_SHA224_H_

// ============================================================================

