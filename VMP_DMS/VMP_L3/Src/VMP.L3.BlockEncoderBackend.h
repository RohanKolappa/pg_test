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
//	VMP.L3.BlockEncoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_L3_BlockEncoderBackend_H_
#define _VMP_L3_BlockEncoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, BlockEncoderBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.BytesFrame.h"
#include "VMP.Session.h"

#include "VMP.L3.EncoderBackend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all block-based encoder backends.
///
/// A BlockEncoderBackend is an object capable of encoding some "uncompressed"
/// or "unencoded" session into a BytesSession, where the content of the
/// Session is provided in a single block of data (a single Frame object).
///
/// \ingroup VMP_L3

class VMP_L3_DLL BlockEncoderBackend : public EncoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new BlockEncoderBackend.

	BlockEncoderBackend(
		const	BFC::DL::TypeCPtr &	pClassType
	) :
		EncoderBackend( pClassType ) {
	}

public :

	/// \brief Returns true iff this encoder can encode the unencoded
	///	data contained in \a pDecoded.
	///
	/// \param pDecoded
	///	The content that should be encoded.

	virtual BFC::Bool canEncodeFrame(
			FramePtr		pDecoded
	) = 0;

	/// \brief Encodes the decoded content stored in \a pDecoded.
	///
	/// \param pDecoded
	///	The unencoded content to encode.
	///
	/// \param pSession
	///	The Session this Frame belongs to.
	///
	/// \returns
	///	The encoded content.
	///
	/// \throws NotSuitable
	///	In case of failure.

	virtual BytesFramePtr encodeFrame(
			FramePtr		pDecoded,
			SessionPtr		pSession
	) = 0;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_BlockEncoderBackend_H_

// ============================================================================

