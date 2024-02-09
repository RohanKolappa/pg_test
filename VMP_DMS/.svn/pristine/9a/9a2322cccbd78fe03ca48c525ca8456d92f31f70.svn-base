// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.BlockDecoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_L3_BlockDecoderBackend_H_
#define _VMP_L3_BlockDecoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, BlockDecoderBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.BytesFrame.h"
#include "VMP.Session.h"

#include "VMP.L3.DecoderBackend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all block-based decoder backends.
///
/// A BlockDecoderBackend is an object capable of decoding a BytesSession
/// into some Session, where the content of the input BytesSession is provided
/// in a single block of data (a single Frame object).
///
/// \ingroup VMP_L3

class VMP_L3_DLL BlockDecoderBackend : public DecoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	BlockDecoderBackend(
		const	BFC::DL::TypeCPtr &	pClassType
	) :
		DecoderBackend( pClassType ) {
	}

public :

	/// \brief Returns true iff this decoder can decode the encoded content
	///	stored in \a pEncoded.

	virtual BFC::Bool canDecodeFrame(
			BytesFramePtr		pEncoded
	) = 0;

	/// \brief Decodes the encoded content stored in \a pEncoded.
	///
	/// \param pEncoded
	///	The encoded content.
	///
	/// \returns
	///	The decoded content.
	///
	/// \throws NotSuitable
	///	In case of failure.

	virtual FramePtr decodeFrame(
			BytesFramePtr		pEncoded
	) = 0;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_BlockDecoderBackend_H_

// ============================================================================

