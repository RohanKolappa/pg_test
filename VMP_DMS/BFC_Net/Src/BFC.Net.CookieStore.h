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
//	BFC.Net.CookieStore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_CookieStore_H_
#define _BFC_Net_CookieStore_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, CookieStore )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Net.HttpCookie.h"
#include "BFC.Net.URL.h"

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents a storage for cookie. Can store and retrieve cookies.
///
/// CookieManager will call CookieStore::add() to save cookies for every
/// incoming HTTP response, and call CookieStore::get() to retrieve cookie for
/// every outgoing HTTP request.
///
/// A CookieStore is responsible for removing HttpCookie instances which have
/// expired.
///
/// \ingroup BFC_Net

class BFC_Net_DLL CookieStore : virtual public SObject {

public :

	/// \brief Adds one HTTP cookie to the store.
	///
	/// This is called for every incoming HTTP response.
	///
	/// A cookie to store may or may not be associated with an URL. If it
	/// is not associated with an URL, the cookie's domain and path
	/// attribute will indicate where it comes from. If it is associated
	/// with an URL and its domain and path attribute are not speicifed,
	/// given URL will indicate where this cookie comes from.
	///
	/// If a cookie corresponding to the given URL already exists, then it
	/// is replaced with the new one.
	///
	/// \param url
	///	The URL this cookie associated with. If null, this cookie will
	///	not be associated with an URL.
	///
	/// \param cookie
	///	The cookie to store.

	virtual void add(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	) = 0;

	/// \brief Retrieves cookies associated with given URL, or whose domain
	///	matches the given URL.
	///
	/// Only cookies that have not expired are returned.
	///
	/// This is called for every outgoing HTTP request.
	///
	/// \returns
	///	An immutable list of HttpCookie, or an empty list if no cookies
	///	match the given URL.

	virtual HttpCookieCArray get(
		const	URL &		url
	) = 0;

	/// \brief Gets all not-expired cookies in cookie store.
	///
	/// \returns
	///	An immutable list of http cookies, or an empty list if there's
	///	no http cookie in store.

	virtual HttpCookieCArray getCookies(
	) = 0;

	/// \brief Gets all URLs which identify the cookies in this cookie
	///	store.
	///
	/// \returns
	///	An immutable list of URIs, or an empty list if no cookie in
	///	this cookie store is associated with an URL.

	virtual CompArray< URL > getURIs(
	) = 0;

	/// \brief Removes a cookie from store.
	///
	/// \param url
	///	The URL this cookie is associated with. If null, the cookie to
	///	be removed is not associated with an URL when added; if not
	///	null, the cookie to be removed is associated with the given URL
	///	when added.
	///
	/// \param cookie
	///	The cookie to remove.
	///
	/// \returns
	///	true if this store contained the specified cookie.

	virtual Bool remove(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	) = 0;

	/// \brief Removes all cookies in this cookie store.
	///
	/// \returns
	///	true if this store changed as a result of the call.

	virtual Bool removeAll(
	) = 0;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_CookieStore_H_

// ============================================================================

