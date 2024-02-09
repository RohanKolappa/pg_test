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
//	VMP.L3.PicturesConsumerBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_PicturesConsumerBackend_H_
#define _VMP_L3_PicturesConsumerBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, PicturesConsumerBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.VideoFrame.h"
#include "VMP.VideoSession.h"

#include "VMP.L3.Backend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Backend converting N input pictures into 1 output Frame.
///
/// \ingroup VMP_L3

class VMP_L3_DLL PicturesConsumerBackend : public Backend {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	PicturesConsumerBackend(
		const	BFC::DL::TypeCPtr &		pClassType,
		const	BFC::Uint32			pNbrInputs,
		const	BFC::DL::TypeCPtr &		pOutputType
	);

public :

	BFC::Uint32 getNbrInputs(
	) const {
		return nbrInputs;
	}

	BFC::DL::TypeCPtr getOutputType(
	) const {
		return outputType;
	}

	/// \brief Computes the output session, based on the input video sessions.
	///
	/// \throws NotSuitable
	///	If this backend cannot consume \a pSessions.

	virtual SessionPtr consumeSessions(
		const	VideoSessionArray &		pSessions
	) = 0;

	/// \brief Computes an output frame, based on the input pictures.
	///
	/// \throws NotSuitable
	///	If this backend cannot process \a pPictures.

	virtual FramePtr consumePictures(
		const	BFC::Image::PictureArray &	pPictures
	) = 0;

private :

	BFC::Uint32		nbrInputs;
	BFC::DL::TypeCPtr	outputType;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_PicturesConsumerBackend_H_

// ============================================================================

