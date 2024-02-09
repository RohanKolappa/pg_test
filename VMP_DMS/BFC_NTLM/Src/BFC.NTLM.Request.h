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
//	BFC.NTLM.Request.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_NTLM_Request_H_
#define _BFC_NTLM_Request_H_

// ============================================================================

#include "BFC.NTLM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace NTLM {

BFC_PTR_DECL( BFC_NTLM_DLL, Request )

} // namespace NTLM
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace NTLM {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_NTLM

class BFC_NTLM_DLL Request : virtual public SObject {

public :

	/// \brief Creates a new Request message for the host of the specified
	///	domain.
	///
	/// This Request should be encoded according to protocol specific
	/// rules (e.g. base 64 encoding).
	///
	/// \param host
	///	The name of the host that is authenticating.
	///
	/// \param hostDomain
	///	The name of the domain to which the host belongs.

	Request(
		const	Buffer &	host,
		const	Buffer &	hostDomain
	);

	const Buffer & toBuffer(
	) const {
		return data;
	}

protected :

private :

	Buffer	data;

	/// \brief Creates a new Request.

	Request(
	);

	/// \brief Forbidden copy constructor.

	Request(
		const	Request &
	);

	/// \brief Forbidden copy operator.

	Request & operator = (
		const	Request &
	);

};

// ============================================================================

} // namespace NTLM
} // namespace BFC

// ============================================================================

#endif // _BFC_NTLM_Request_H_

// ============================================================================

