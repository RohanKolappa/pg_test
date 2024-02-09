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
//	VMP.RTP.TEStreamCheckerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TEStreamCheckerEngine_H_
#define _VMP_RTP_TEStreamCheckerEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TEStreamCheckerEnginePtr.h"

// ============================================================================

#include "BFC.Time.Clock.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TEStreamCheckerEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TEStreamCheckerEngine.

	TEStreamCheckerEngine(
	);

	void setAllowMultipleSSRC(
		const	BFC::Bool	pMultSSRC
	) {
		multSSRC = pMultSSRC;
	}

	virtual void putSessionCallback(
			SessionPtr	pInptSess
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pInptFrme
	);

protected :

private :

	static const BFC::Uint64 Mod_2_32	= ( CU64( 1 ) << 32 );

	BFC::Bool		multSSRC;
	BFC::Bool		gotFirst;
	BFC::Uint16		nextSqNb;
	BFC::Uint32		currSSRC;
	BFC::Uint32		currType;	///< Payload type.
	BFC::Time::Clock	lastRead;	///< Last wall clock.
	BFC::Uint32		timeStmp;	///< Last packet TS.

	/// \brief Forbidden copy constructor.

	TEStreamCheckerEngine(
		const	TEStreamCheckerEngine &
	);

	/// \brief Forbidden copy operator.

	TEStreamCheckerEngine & operator = (
		const	TEStreamCheckerEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TEStreamCheckerEngine_H_

// ============================================================================

