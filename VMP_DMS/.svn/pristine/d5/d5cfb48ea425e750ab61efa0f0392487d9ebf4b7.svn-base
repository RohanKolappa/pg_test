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
//	VMP.RTP.TFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TFrame_H_
#define _VMP_RTP_TFrame_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TFramePtr.h"

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

class VMP_RTP_DLL TFrame : public Frame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TFrame.

	TFrame(
	);

	/// \brief Copy constructor.

	TFrame(
		const	TFrame &	pOther
	);

	virtual BFC::SPtr clone(
	) const;

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

	BFC::Net::SockAddrCPtr	srcSAddr;	///< Source address.
	BFC::Net::SockAddrCPtr	tgtSAddr;	///< Target address.
	BFC::Time::Clock	readTime;	///< When was this packet read ?
	BFC::Bool		M;		///< "m" flag.
	BFC::Uint32		PT;		///< "pt" field.
	BFC::Uint32		SN;		///< "sequence number".
	BFC::Uint32		TS;		///< "timestamp".
	BFC::Uint32		SSRC;		///< "ssrc" field.

	BFC::Buffer		data;		///< Payload.

protected :

private :

	/// \brief Forbidden copy operator.

	TFrame & operator = (
		const	TFrame &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TFrame_H_

// ============================================================================

