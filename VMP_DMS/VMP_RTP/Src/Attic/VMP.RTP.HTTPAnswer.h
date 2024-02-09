// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP".
// 
// "VMP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.HTTPAnswer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_HTTPAnswer_H_
#define _VMP_RTP_HTTPAnswer_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

BFC_PTR_DECL( VMP_RTP_DLL, HTTPAnswer )

} // namespace RTP
} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Debug.DObject.h"
#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL HTTPAnswer : virtual public BFC::DObject {

public :

	/// \brief Creates a new HTTPAnswer.

	HTTPAnswer(
	);

	/// \brief Destroys this object.

	virtual ~HTTPAnswer(
	);

	BFC::Bool consume(
		const	BFC::Buffer &	payload
	);

	const BFC::Buffer & getProtocol(
	) const {
		return protocol;
	}

	BFC::Uint32 getStatus(
	) const {
		return status;
	}

	const BFC::BufferArray & getOptions(
	) const {
		return options;
	}

	const BFC::Buffer & getPayload(
	) const {
		return payload;
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.RTP.HTTPAnswer" );

	CLASS_EXCEPTION( InvalidData, "Invalid data" );

protected :

private :

	BFC::Buffer		protocol;
	BFC::Uint32		status;
	BFC::BufferArray	options;
	BFC::Buffer		payload;

	/// \brief Forbidden copy constructor.

	HTTPAnswer(
		const	HTTPAnswer&
	);

	/// \brief Forbidden copy operator.

	HTTPAnswer& operator = (
		const	HTTPAnswer&
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_HTTPAnswer_H_

// ============================================================================

