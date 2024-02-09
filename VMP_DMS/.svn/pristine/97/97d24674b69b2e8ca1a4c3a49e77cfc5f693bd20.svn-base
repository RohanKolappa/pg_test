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
//	BFC.Crypto.RNG.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Crypto_RNG_H_
#define _BFC_Crypto_RNG_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Crypto.RNGPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Crypto_DLL RNG : virtual public SObject {

public :

	static RNGPtr createNew(
	);

	RNG(
	) : valid( false ) {
	}

	/* virtual */ Buffer getBytes(
		const	Uint32		pBytes
	);

	virtual void getBytes(
			Buffer &	pBuffer
	) = 0;

	Bool isValid(
	) {
		return valid;
	}

	BASE_CLASS_GEN_EXCEPTION( "BFC.Crypto.RNG" );

	CLASS_EXCEPTION( NotValid, "Not valid" );

protected :

	void setValid(
	) {
		valid = true;
	}

private :

	Bool	valid;

};

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_RNG_H_

// ============================================================================

