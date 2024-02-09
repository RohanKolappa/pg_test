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
//	BFC.Base64.Decoder.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base64_Decoder_H_
#define _BFC_Base64_Decoder_H_

// ============================================================================

#include "BFC.Base64.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Base64 {

BFC_PTR_DECL( BFC_Base64_DLL, Decoder )

} // namespace Base64
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Base64 {

// ============================================================================

/// \brief Base64 decoder.
///
/// \ingroup BFC_Base64

class BFC_Base64_DLL Decoder : virtual public SObject {

public :

	/// \brief Creates a new Decoder.

	Decoder(
	);

	Buffer decode(
		const	Buffer &	in
	) const;

protected :

	/// \brief Conversion table: decoding: 8 bits -> 6 bits.

	static const Uchar decode_table[];

private :

	/// \brief Forbidden copy constructor.

	Decoder(
		const	Decoder&
	);

	/// \brief Forbidden copy operator.

	Decoder& operator = (
		const	Decoder&
	);

};

// ============================================================================

} // namespace Base64
} // namespace BFC

// ============================================================================

#endif // _BFC_Base64_Decoder_H_

// ============================================================================

