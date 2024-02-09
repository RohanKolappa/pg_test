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
//	BFC.Net.CookieManager.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_CookieManager_H_
#define _BFC_Net_CookieManager_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, CookieManager )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.CookieHandler.h"
#include "BFC.Net.CookiePolicy.h"
#include "BFC.Net.CookieStore.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Provides a concrete implementation of CookieHandler, which separates
///	the storage of cookies from the policy surrounding accepting and
///	rejecting cookies.
///
/// A CookieManager is initialized with a CookieStore which manages storage,
/// and a CookiePolicy object, which makes policy decisions on cookie
/// acceptance/rejection.
///
/// The HTTP cookie management in BFC::Net package looks like:
/// \verbatim
///	                 use
///	CookieHandler <------- HttpURLConnection
///	   ^
///	   | impl
///	   |            use
///	CookieManager -------> CookiePolicy
///	         |   use
///	         |--------> HttpCookie
///	         |              ^
///	         |              | use
///	         |   use        |
///	         |--------> CookieStore
///	                        ^
///	                        | impl
///	                        |
///	              Internal in-memory implementation
/// \endverbatim
///
/// -	CookieHandler is at the core of cookie management. User can call
///	CookieHandler::setDefault() to set a concrete CookieHanlder
///	implementation to be used.
/// -	CookiePolicy::shouldAccept() will be called by CookieManager::put() to
///	see whether or not one cookie should be accepted and put into cookie
///	store. User can use any of three pre-defined CookiePolicy, namely
///	ACCEPT_ALL, ACCEPT_NONE and ACCEPT_SERVER, or user can define his own
///	CookiePolicy implementation and tell CookieManager to use it.
/// -	CookieStore is the place where any accepted HTTP cookie is stored in.
///	If not specified when created, a CookieManager instance will use an
///	internal in-memory implementation. Or user can implements one and tell
///	CookieManager to use it.
/// -	Currently, only CookieStore::add( URI, HttpCookie ) and
///	CookieStore::get( URI ) are used by CookieManager. Others are for
///	completeness and might be needed by a more sophisticated CookieStore
///	implementation, e.g. a NetscapeCookieSotre.
///
/// There're various ways user can hook up his own HTTP cookie management
/// behavior, e.g.:
/// -	Use CookieHandler::setDefault() to set a brand new CookieHandler
///	implementation.
/// -	Let CookieManager be the default CookieHandler implementation, but
///	implement user's own CookieStore and CookiePolicy and tell default
///	CookieManager to use them:
///	\code
///		// this should be done at the beginning of an HTTP session
///		CookieHandler.setDefault(new CookieManager(new MyCookieStore(), new MyCookiePolicy()));
///	\endcode
/// -	Let CookieManager be the default CookieHandler implementation, but
///	use customized CookiePolicy:
///	\code
///		// this should be done at the beginning of an HTTP session
///		CookieHandler.setDefault(new CookieManager());
///		// this can be done at any point of an HTTP session
///		((CookieManager)CookieHandler.getDefault()).setCookiePolicy(new MyCookiePolicy());
///	\endcode
///
/// The implementation conforms to RFC 2965, section 3.3.
///
/// \ingroup BFC_Net

class BFC_Net_DLL CookieManager : public CookieHandler {

public :

	/// \brief Creates a new CookieManager with specified cookie store and
	///	cookie policy.
	///
	/// \param store
	///	A CookieStore to be used by this CookieManager. If null, this
	///	CookieManager will use a default one, which is an in-memory
	///	CookieStore implementation.
	///
	/// \param cookiePolicy
	///	A CookiePolicy instance to be used by this CookieManager as
	///	policy callback. If null, ACCEPT_SERVER will be used.

	CookieManager(
			CookieStorePtr		store = CookieStorePtr(),
			CookiePolicyCPtr	cookiePolicy = CookiePolicy::ACCEPT_SERVER
	);

	/// \brief Sets the cookie policy of this CookieManager.
	///
	/// \param pCookiePolicy
	///	The cookie policy. Can be null, which has no effects on current
	///	cookie policy.

	void setCookiePolicy(
			CookiePolicyCPtr	pCookiePolicy
	) {
		if ( pCookiePolicy ) {
			cookiePolicy = pCookiePolicy;
		}
	}

	/// \brief Gets the current cookie store.
	///
	/// \returns
	///	The CookieStore currently used by this CookieManager.

	CookieStorePtr getCookieStore(
	) const {
		return cookieStore;
	}

//	public Map<String, List<String>>
//		get(URI uri, Map<String, List<String>> requestHeaders)
//		throws IOException
//	{
//		// pre-condition check
//		if (uri == null || requestHeaders == null) {
//			throw new IllegalArgumentException("Argument is null");
//		}
//
//		Map<String, List<String>> cookieMap =
//						new java.util.HashMap<String, List<String>>();
//		// if there's no default CookieStore, no way for us to get any cookie
//		if (cookieStore == null)
//			return Collections.unmodifiableMap(cookieMap);
//
//		List<HttpCookie> cookies = new java.util.ArrayList<HttpCookie>();
//		for (HttpCookie cookie : cookieStore.get(uri)) {
//			// apply path-matches rule (RFC 2965 sec. 3.3.4)
//			if (pathMatches(uri.getPath(), cookie.getPath())) {
//				cookies.add(cookie);
//			}
//		}
//
//		// apply sort rule (RFC 2965 sec. 3.3.4)
//		List<String> cookieHeader = sortByPath(cookies);
//
//		cookieMap.put("Cookie", cookieHeader);
//		return Collections.unmodifiableMap(cookieMap);
//	}
//
//
//	public void
//		put(URI uri, Map<String, List<String>> responseHeaders)
//		throws IOException
//	{
//		// pre-condition check
//		if (uri == null || responseHeaders == null) {
//			throw new IllegalArgumentException("Argument is null");
//		}
//
//
//		// if there's no default CookieStore, no need to remember any cookie
//		if (cookieStore == null)
//			return;
//
//		for (String headerKey : responseHeaders.keySet()) {
//			// RFC 2965 3.2.2, key must be 'Set-Cookie2'
//			// we also accept 'Set-Cookie' here for backward compatibility
//			if (headerKey == null
//				|| !(headerKey.equalsIgnoreCase("Set-Cookie2")
//					 || headerKey.equalsIgnoreCase("Set-Cookie")
//					)
//				)
//			{
//				continue;
//			}
//
//			for (String headerValue : responseHeaders.get(headerKey)) {
//				try {
//					List<HttpCookie> cookies = HttpCookie.parse(headerValue);
//					for (HttpCookie cookie : cookies) {
//						if (shouldAcceptInternal(uri, cookie)) {
//							cookieStore.add(uri, cookie);
//						}
//					}
//				} catch (IllegalArgumentException e) {
//					// invalid set-cookie header string
//					// no-op
//				}
//			}
//		}
//	}

protected :

//	/* ---------------- Private operations -------------- */
//
//	// to determine whether or not accept this cookie
//	private boolean shouldAcceptInternal(URI uri, HttpCookie cookie) {
//		try {
//			return cookiePolicy.shouldAccept(uri, cookie);
//		} catch (Exception ignored) { // pretect against malicious callback
//			return false;
//		}
//	}
//
//
//	/*
//	 * path-matches algorithm, as defined by RFC 2965
//	 */
//	private boolean pathMatches(String path, String pathToMatchWith) {
//		if (path == pathToMatchWith)
//			return true;
//		if (path == null || pathToMatchWith == null)
//			return false;
//		if (path.startsWith(pathToMatchWith))
//			return true;
//
//		return false;
//	}
//
//
//	/*
//	 * sort cookies with respect to their path: those with more specific Path attributes
//	 * precede those with less specific, as defined in RFC 2965 sec. 3.3.4
//	 */
//	private List<String> sortByPath(List<HttpCookie> cookies) {
//		Collections.sort(cookies, new CookiePathComparator());
//
//		List<String> cookieHeader = new java.util.ArrayList<String>();
//		for (HttpCookie cookie : cookies) {
//			// Netscape cookie spec and RFC 2965 have different format of Cookie
//			// header; RFC 2965 requires a leading $Version="1" string while Netscape
//			// does not.
//			// The workaround here is to add a $Version="1" string in advance
//			if (cookies.indexOf(cookie) == 0 && cookie.getVersion() > 0) {
//				cookieHeader.add("$Version=\"1\"");
//			}
//
//			cookieHeader.add(cookie.toBuffer());
//		}
//		return cookieHeader;
//	}
//
//
//	static class CookiePathComparator implements Comparator<HttpCookie> {
//		public int compare(HttpCookie c1, HttpCookie c2) {
//			if (c1 == c2) return 0;
//			if (c1 == null) return -1;
//			if (c2 == null) return 1;
//
//			// path rule only applies to the cookies with same name
//			if (!c1.getName().equals(c2.getName())) return 0;
//
//			// those with more specific Path attributes precede those with less specific
//			if (c1.getPath().startsWith(c2.getPath()))
//				return -1;
//			else if (c2.getPath().startsWith(c1.getPath()))
//				return 1;
//			else
//				return 0;
//		}
//	}

private :

	CookieStorePtr		cookieStore;
	CookiePolicyCPtr	cookiePolicy;

	/// \brief Forbidden copy constructor.

	CookieManager(
		const	CookieManager &
	);

	/// \brief Forbidden copy operator.

	CookieManager & operator = (
		const	CookieManager &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_CookieManager_H_

#endif // 0

// ============================================================================

