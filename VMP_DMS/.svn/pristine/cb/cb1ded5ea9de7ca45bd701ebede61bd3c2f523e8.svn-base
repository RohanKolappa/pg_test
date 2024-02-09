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
//	VMP.AudioSession.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AudioSession_H_
#define _VMP_AudioSession_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.AudioSessionPtr.h"

// ============================================================================

#include "VMP.AudioStream.h"
#include "VMP.Session.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL AudioSession : public Session {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new AudioSession.

	AudioSession(
		const	Flags		pFlags = Pausable
	);

	/// \brief Creates a new AudioSession.

	AudioSession(
			AudioStreamPtr	pStream,
		const	Flags		pFlags = Pausable
	);

	/// \brief Copy constructor.

	AudioSession(
		const	AudioSession &	pOther
	);

	virtual SessionPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			SessionCPtr	pSession
	) const;

	void setContent(
			AudioStreamPtr	p
	) {
		setStream( p );
	}

	BFC::Bool hasContent(
	) const {
		return hasStream();
	}

	AudioStreamPtr getContent(
	) {
		return getStream();
	}

	AudioStreamCPtr getContent(
	) const {
		return getStream();
	}

	virtual BFC::Buffer toBuffer(
	) const;

protected :

private :

	/// \brief Forbidden copy operator.

	AudioSession & operator = (
		const	AudioSession &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_AudioSession_H_

// ============================================================================

