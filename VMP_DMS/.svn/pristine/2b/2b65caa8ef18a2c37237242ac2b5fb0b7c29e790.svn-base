// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.TSDecoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_TSDecoderBackend_H_
#define _VMP_M2S_TSDecoderBackend_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSDecoderBackendPtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.GPC.Consumer.h"

#include "VMP.L3.StreamDecoderBackend.h"

#include "VMP.M2S.TSPacketFramerPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL TSDecoderBackend :

	public L3::StreamDecoderBackend,
	public GPC::Consumer {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TSDecoderBackend.

	TSDecoderBackend(
	);

	void init(
		const	BFC::Uint16	pPID
	) {
		pid = pPID;
	}

	virtual const BFC::DL::TypeCPtr & getOutputType(
	) const;

	virtual BFC::Bool canDecode(
			BytesSessionPtr	pIBSession
	);

	virtual SessionPtr initSession(
			BytesSessionPtr	pIBSession
	);

	virtual void exitSession(
	);

	virtual FramePtr getFrame(
	);

	virtual void seekFrame(
		const	BFC::Uint64	pIndex
	);

	virtual void putObject(
			BFC::SPtr	pPESPacket
	);

protected :

private :

	TSPacketFramerPtr		frm;
	BFC::Uint16			pid;
	BFC::Array< PESPacketPtr >	pck;

	/// \brief Forbidden copy constructor.

	TSDecoderBackend(
		const	TSDecoderBackend &
	);

	/// \brief Forbidden copy operator.

	TSDecoderBackend & operator = (
		const	TSDecoderBackend &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSDecoderBackend_H_

// ============================================================================

