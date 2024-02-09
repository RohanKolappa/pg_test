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
//	VMP.VideoSession.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_VideoSession_H_
#define _VMP_VideoSession_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.VideoSessionPtr.h"

// ============================================================================

#include "VMP.Session.h"
#include "VMP.VideoStream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL VideoSession : public Session {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new VideoSession.

	VideoSession(
		const	Flags		pFlags = Pausable // Seekable
	);

	VideoSession(
		const	VideoSession&	pOther
	);

	virtual SessionPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			SessionCPtr	pSession
	) const;

	void setContent(
			VideoStreamPtr	p
	) {
		setStream( p );
	}

	/// \brief Returns true iff the content has been set.

	BFC::Bool hasContent(
	) const {
		return hasStream();
	}

	VideoStreamPtr getContent(
	) {
		return getStream();
	}

	VideoStreamCPtr getContent(
	) const {
		return getStream();
	}

	virtual BFC::Buffer toBuffer(
	) const;

protected :

private :

	/// \brief Forbidden copy operator.

	VideoSession & operator = (
		const	VideoSession &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_VideoSession_H_

// ============================================================================

