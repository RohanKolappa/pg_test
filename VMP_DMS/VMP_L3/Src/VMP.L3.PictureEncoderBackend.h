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
//	VMP.L3.PictureEncoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_L3_PictureEncoderBackend_H_
#define _VMP_L3_PictureEncoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, PictureEncoderBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "BFC.Image.Picture.h"

#include "VMP.BytesFrame.h"
#include "VMP.BytesSession.h"
#include "VMP.VideoFrame.h"
#include "VMP.VideoSession.h"

#include "VMP.L3.BlockEncoderBackend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all picture encoder backends.
///
/// \ingroup VMP_L3

class VMP_L3_DLL PictureEncoderBackend : public BlockEncoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Returns the output format of this encoder.
	///
	/// The default is to return VideoCompression::Unknown.

	virtual VideoCompression getVideoCompression(
	) const;

protected :

	/// \brief Creates a new PictureEncoderBackend.

	PictureEncoderBackend(
		const	BFC::DL::TypeCPtr &	pClassType
	) :
		BlockEncoderBackend( pClassType ) {
	}

public :

	virtual BFC::Bool canEncodeFrame(
			FramePtr		pDecoded
	);

	virtual BytesFramePtr encodeFrame(
			FramePtr		pDecoded,
			SessionPtr		pSession
	);

	/// \brief Returns true iff this encoder can encode the decoded picture
	///	stored in \a pDecoded.
	///
	/// The default is to return true, whatever \a pDecoded may contain.

	virtual BFC::Bool canEncode(
		const	BFC::Image::Picture &	pDecoded
	);

	/// \brief Encodes the decoded picture stored in \a pDecoded.
	///
	/// \returns
	///	The encoded picture.
	///
	/// \throws NotSuitable
	///	In case of failure.

	virtual BFC::Buffer encode(
		const	BFC::Image::Picture &	pDecoded
	) = 0;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_PictureEncoderBackend_H_

// ============================================================================

