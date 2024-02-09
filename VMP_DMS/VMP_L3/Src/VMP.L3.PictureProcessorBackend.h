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
//	VMP.L3.PictureProcessorBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_L3_PictureProcessorBackend_H_
#define _VMP_L3_PictureProcessorBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, PictureProcessorBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "BFC.Image.Picture.h"

#include "VMP.VideoFrame.h"
#include "VMP.VideoSession.h"

#include "VMP.L3.Backend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all picture transformer backends.
///
/// A PictureProcessorBackend is an object capable of transforming a
/// single BFC::Image::Picture into another BFC::Image::Picture.
///
/// \ingroup VMP_L3

class VMP_L3_DLL PictureProcessorBackend : public Backend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new PictureProcessorBackend.

	PictureProcessorBackend(
		const	BFC::DL::TypeCPtr &	pClassType = getClassType()
	);

public :

	/// \brief Initializes this object, preparing it to transform
	///	BFC::Image::Picture objects described by the \a iVSession
	///	session descriptor.
	///
	/// The default implementation returns the same video session
	/// descriptor.
	///
	/// \throws NotSuitable
	///	If this backend cannot process \a iVSession.

	virtual VideoSessionPtr processSession(
			VideoSessionPtr		iVSession
	);

	/// \brief Transforms picture \a picture.
	///
	/// \throws NotSuitable
	///	If this backend cannot process \a iVFrame.

	virtual VideoFramePtr processFrame(
			VideoFramePtr		iVFrame
	) = 0;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_PictureProcessorBackend_H_

// ============================================================================

