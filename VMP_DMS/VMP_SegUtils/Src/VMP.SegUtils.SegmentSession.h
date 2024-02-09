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
//	VMP.SegUtils.SegmentSession.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_SegUtils_SegmentSession_H_
#define _VMP_SegUtils_SegmentSession_H_

// ============================================================================

#include "VMP.SegUtils.DLL.h"

// ============================================================================

#include "VMP.SegUtils.SegmentSessionPtr.h"

// ============================================================================

#include "BFC.Base.Frac64.h"

#include "VMP.Session.h"

// ============================================================================

namespace VMP {
namespace SegUtils {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_SegUtils

class VMP_SegUtils_DLL SegmentSession : public Session {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SegmentSession.

	SegmentSession(
		const	Flags		pSessFlgs = Seekable
	);

	/// \brief Creates a new SegmentSession.

	SegmentSession(
			BytesSessionPtr	pByteSess,
		const	BFC::Frac64 &	pSegmDrtn,
		const	Flags		pSessFlgs = Seekable
	);

	/// \brief Copy constructor.

	SegmentSession(
		const	SegmentSession &
					pOther
	);

	BytesSessionPtr getBytesSession(
	) const {
		return byteSess;
	}

	const BFC::Frac64 & getSegmentDuration(
	) const {
		return segmDrtn;
	}

	virtual SessionPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			SessionCPtr	pSession
	) const;

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	BytesSessionPtr	byteSess;
	BFC::Frac64	segmDrtn;

	/// \brief Forbidden copy operator.

	SegmentSession & operator = (
		const	SegmentSession &
	);

};

// ============================================================================

} // namespace SegUtils
} // namespace VMP

// ============================================================================

#endif // _VMP_SegUtils_SegmentSession_H_

// ============================================================================

