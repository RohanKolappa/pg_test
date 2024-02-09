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
//	BFC.Net.CookiePolicy.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_CookiePolicy_H_
#define _BFC_Net_CookiePolicy_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, CookiePolicy )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Net.HttpCookie.h"
#include "BFC.Net.URL.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Decides which cookies should be accepted and which should be
///	rejected.
///
/// Three pre-defined policy implementations are provided, namely ACCEPT_ALL,
/// ACCEPT_NONE and ACCEPT_SERVER.
///
/// See RFC 2965 sec. 3.3 & 7 for more detail.
///
/// \ingroup BFC_Net

class BFC_Net_DLL CookiePolicy : virtual public SObject {

public :

	/// \brief Pre-defined policy which accepts all cookies.

	static CookiePolicyCPtr ACCEPT_ALL;

	/// \brief Pre-defined policy which accepts no cookies.

	static CookiePolicyCPtr ACCEPT_NONE;

	/// \brief Pre-defined policy which only accepts cookies from original
	///	server.

	static CookiePolicyCPtr ACCEPT_SERVER;

	/// \brief Called to see whether or not this cookie should be accepted.
	///
	/// \param url
	///	The URL to consult this policy with.
	///
	/// \param cookie
	///	The HttpCookie object in question.
	///
	/// \returns
	///	true if this cookie should be accepted; otherwise, false.

	virtual Bool shouldAccept(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	) const = 0;

};

// ============================================================================

class BFC_Net_DLL CookiePolicyAcceptAll : public CookiePolicy {

public :

	virtual Bool shouldAccept(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	) const;

};

// ============================================================================

class BFC_Net_DLL CookiePolicyAcceptNone : public CookiePolicy {

public :

	virtual Bool shouldAccept(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	) const;

};

// ============================================================================

class BFC_Net_DLL CookiePolicyAcceptServer : public CookiePolicy {

public :

	virtual Bool shouldAccept(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	) const;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_CookiePolicy_H_

// ============================================================================

