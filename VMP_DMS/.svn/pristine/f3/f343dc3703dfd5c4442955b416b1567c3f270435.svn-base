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
//	VMP.L3.PicturesProducerBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_PicturesProducerBackend_H_
#define _VMP_L3_PicturesProducerBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, PicturesProducerBackend )

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

/// \brief Base class of all backend transforming input pictures into one or
///	more output pictures.
///
/// \ingroup VMP_L3

class VMP_L3_DLL PicturesProducerBackend : public Backend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new PicturesProducerBackend.

	PicturesProducerBackend(
		const	BFC::Uint32		nbrOutputs = 1
	);

	BFC::Uint32 getNbrOutputs(
	) const {
		return nbrOutputs;
	}

	/// \brief Processes one input session.
	///
	/// \throws NotSuitable
	///	If this object is not capable of processing the video session
	///	described by \a iVSession.

	virtual VideoSessionArray processSession(
			VideoSessionPtr		iVSession
	);

	/// \brief Processes one input picture.
	///
	/// \throws EndOfStream
	///
	/// \throws BrokenPipe

	virtual VideoFrameArray processFrame(
			VideoFramePtr		iVFrame
	) = 0;

protected :

private :

	BFC::Uint32		nbrOutputs;

	/// \brief Forbidden copy constructor.

	PicturesProducerBackend(
		const	PicturesProducerBackend &
	);

	/// \brief Forbidden copy operator.

	PicturesProducerBackend & operator = (
		const	PicturesProducerBackend &
	);

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_PicturesProducerBackend_H_

// ============================================================================

