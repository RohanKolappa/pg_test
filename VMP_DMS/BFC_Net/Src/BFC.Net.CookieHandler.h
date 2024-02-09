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
//	BFC.Net.CookieHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_CookieHandler_H_
#define _BFC_Net_CookieHandler_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, CookieHandler )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.URL.h"

#include "BFC.TL.List.h"
#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Provides a callback mechanism to hook up a HTTP state management
///	policy implementation into the HTTP protocol handler.
///
/// The HTTP state management mechanism specifies a way to create a stateful
/// session with HTTP requests and responses.
///
/// A system-wide CookieHandler that to used by the HTTP protocol handler can
/// be registered by doing a CookieHandler::setDefault(). The currently
/// registered CookieHandler can be retrieved by calling
/// CookieHandler::getDefault().
///
/// For more information on HTTP state management, see
/// <a href="http://www.ietf.org/rfc/rfc2965.txt">RFC 2965: HTTP
/// State Management Mechanism</a>.
///
/// \ingroup BFC_Net

class BFC_Net_DLL CookieHandler : virtual public SObject {

public :

	/// \brief Gets the system-wide cookie handler.
	///
	/// \return
	///	The system-wide cookie handler. A null return means there is no
	///	system-wide cookie handler currently set.

	static CookieHandlerPtr getDefault(
	) {
		return systHandler;
	}

	/// \brief Sets (or unsets) the system-wide cookie handler.
	///
	/// \note
	///	Non-standard http protocol handlers may ignore this setting.
	///
	/// \param pSystHandler
	///	The HTTP cookie handler, or null to unset.

	static void setDefault(
			CookieHandlerPtr	pSystHandler
	) {
		systHandler = pSystHandler;
	}

	/// \brief Gets all the applicable cookies from a cookie cache for the
	///	specified URL in the request header.
	///
	/// HTTP protocol implementers should make sure that this method is
	/// called after all request headers related to choosing cookies are
	/// added, and before the request is sent.
	///
	/// \param url
	///	A URL to send cookies to in a request.
	///
	/// \param requestHeaders
	///	A Map from request header field names to lists of field values
	///	representing the current request headers.
	///
	/// \return
	///	An immutable map from state management headers, with field
	///	names "Cookie" or "Cookie2" to a list of cookies containing
	///	state information.

	virtual Map< Buffer, List< Buffer > > get(
		const	URL &		url,
		const	Map< Buffer, List< Buffer > > &
					requestHeaders
	) = 0;

	/// \brief Sets all the applicable cookies, examples are response
	///	header fields that are named Set-Cookie2, present in the
	///	response headers into a cookie cache.
	///
	/// \param url
	///	A URL where the cookies come from.
	///
	/// \param responseHeaders
	///	An immutable map from field names to lists of field values
	///	representing the response header fields returned.

	virtual void put(
		const	URL &		url,
		const	Map< Buffer, List< Buffer > > &
					responseHeaders
	) = 0;

protected :

private :

	static CookieHandlerPtr systHandler;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_CookieHandler_H_

// ============================================================================

