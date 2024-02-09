// ============================================================================
// 
// Copyright (c) 2003-2009 Barco N.V.
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
//	BFC.HTTP.Client.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_HTTP_Client_H_
#define _BFC_HTTP_Client_H_

// ============================================================================

#include "BFC.HTTP.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace HTTP {

BFC_PTR_DECL( BFC_HTTP_DLL, Client )

} // namespace HTTP
} // namespace BFC

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.HTTP.Answer.h"

#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.URL.h"

// ============================================================================

namespace BFC {
namespace HTTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_JMHP

class BFC_HTTP_DLL Client : virtual public DObject {

public :

	/// \brief Creates a new Client.

	Client(
	);

	AnswerPtr get(
		const	Net::URL &		url
	);

protected :

	AnswerPtr getDirect(
		const	Net::IPv4Address &	addr,
		const	Uint16			port,
		const	Buffer &		url
	);

	AnswerPtr getProxy(
		const	Net::URL &		proxy,
		const	Net::URL &		url
	);

private :

	/// \brief Forbidden copy constructor.

	Client(
		const	Client &
	);

	/// \brief Forbidden copy operator.

	Client & operator = (
		const	Client &
	);

};

// ============================================================================

} // namespace HTTP
} // namespace BFC

// ============================================================================

#endif // _BFC_HTTP_Client_H_

// ============================================================================

