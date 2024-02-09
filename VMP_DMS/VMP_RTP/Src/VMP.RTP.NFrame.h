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
//	VMP.RTP.NFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_NFrame_H_
#define _VMP_RTP_NFrame_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.NFramePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Net.SockAddrPtr.h"

#include "BFC.Time.Clock.h"

#include "VMP.Frame.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL NFrame : public Frame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new NFrame.

	NFrame(
	);

	/// \brief Creates a new NFrame.

	NFrame(
			BFC::Net::SockAddrCPtr
					pSrcSAddr,
			BFC::Net::SockAddrCPtr
					pTgtSAddr,
		const	BFC::Buffer &	pBuffData,
		const	BFC::Time::Clock &
					pBuffTime
	);

	/// \brief Copy constructor.

	NFrame(
		const	NFrame &	pOther
	);

	virtual BFC::SPtr clone(
	) const;

	BFC::Net::SockAddrCPtr getSourceAddress(
	) const {
		return srcSAddr;
	}

	BFC::Net::SockAddrCPtr getTargetAddress(
	) const {
		return tgtSAddr;
	}

	const BFC::Buffer & getData(
	) const {
		return buffData;
	}

	const BFC::Time::Clock & getTime(
	) const {
		return buffTime;
	}

	virtual BFC::Uint32 getLength(
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

	BFC::Net::SockAddrCPtr	srcSAddr;
	BFC::Net::SockAddrCPtr	tgtSAddr;
	BFC::Buffer		buffData;
	BFC::Time::Clock	buffTime;

	/// \brief Forbidden copy operator.

	NFrame & operator = (
		const	NFrame &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_NFrame_H_

// ============================================================================

