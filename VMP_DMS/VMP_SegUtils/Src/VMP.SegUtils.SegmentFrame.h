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
//	VMP.SegUtils.SegmentFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_SegUtils_SegmentFrame_H_
#define _VMP_SegUtils_SegmentFrame_H_

// ============================================================================

#include "VMP.SegUtils.DLL.h"

// ============================================================================

#include "VMP.SegUtils.SegmentFramePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Frac64.h"

#include "BFC.Time.Clock.h"

#include "VMP.Frame.h"

// ============================================================================

namespace VMP {
namespace SegUtils {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_SegUtils

class VMP_SegUtils_DLL SegmentFrame : public Frame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SegmentFrame.

	SegmentFrame(
	);

	/// \brief Creates a new SegmentFrame.

	SegmentFrame(
		const	BFC::Uint64	pSegmIndx,
		const	BFC::Frac64 &	pSegmDrtn,
		const	BFC::Buffer &	pFileName
	);

	/// \brief Copy constructor.

	SegmentFrame(
		const	SegmentFrame &	pOther
	);

	BFC::Uint64 getSegmentIndex(
	) const {
		return segmIndx;
	}

	const BFC::Frac64 & getSegmentDuration(
	) const {
		return segmDrtn;
	}

	const BFC::Buffer & getFileName(
	) const {
		return fileName;
	}

	virtual BFC::SPtr clone(
	) const;

	virtual BFC::Uint32 getLength(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	BFC::Uint64	segmIndx;
	BFC::Frac64	segmDrtn;
	BFC::Buffer	fileName;

	/// \brief Forbidden copy operator.

	SegmentFrame & operator = (
		const	SegmentFrame &
	);

};

// ============================================================================

} // namespace SegUtils
} // namespace VMP

// ============================================================================

#endif // _VMP_SegUtils_SegmentFrame_H_

// ============================================================================

