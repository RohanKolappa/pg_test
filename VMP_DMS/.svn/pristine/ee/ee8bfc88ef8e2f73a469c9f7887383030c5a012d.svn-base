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
//	VMP.AVC.Session.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_Session_H_
#define _VMP_AVC_Session_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.SessionPtr.h"

// ============================================================================

#include "VMP.BytesSession.h"

#include "VMP.AVC.PacketMode.h"
#include "VMP.AVC.StreamMode.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL Session : public BytesSession {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Session.

	Session(
	);

	/// \brief Creates a new Session.

	Session(
		const	Flags		pFlags,
		const	PacketMode	pPcktMode,
		const	StreamMode	pStrmMode
	);

	/// \brief Creates a new Session.

	Session(
		const	BytesSession &	pOther,
		const	PacketMode	pPcktMode,
		const	StreamMode	pStrmMode
	);

	/// \brief Copy constructor.

	Session(
		const	Session &	pOther
	);

	PacketMode getPacketMode(
	) const {
		return pcktMode;
	}

	void setPacketMode(
		const	PacketMode	pPcktMode
	) {
		pcktMode = pPcktMode;
	}

	StreamMode getStreamMode(
	) const {
		return strmMode;
	}

	void setStreamMode(
		const	StreamMode	pStrmMode
	) {
		strmMode = pStrmMode;
	}

	virtual VMP::SessionPtr clone(
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

	PacketMode	pcktMode;
	StreamMode	strmMode;

	/// \brief Forbidden copy operator.

	Session & operator = (
		const	Session &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_Session_H_

// ============================================================================

