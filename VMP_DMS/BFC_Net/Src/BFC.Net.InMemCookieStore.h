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
//	BFC.Net.InMemCookieStore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_InMemCookieStore_H_
#define _BFC_Net_InMemCookieStore_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, InMemCookieStore )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.FastCS.h"

#include "BFC.Net.CookieStore.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief A simple in-memory CookieStore implementation.
///
/// \ingroup BFC_Net

class BFC_Net_DLL InMemCookieStore : public CookieStore {

public :

	/// \brief Creates a new InMemCookieStore.

	InMemCookieStore(
	) {
	}

	virtual void add(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	);

	virtual HttpCookieCArray get(
		const	URL &		url
	);

	virtual HttpCookieCArray getCookies(
	);

	virtual CompArray< URL > getURIs(
	);

	virtual Bool remove(
		const	URL &		url,
		const	HttpCookieCPtr &
					cookie
	);

	virtual Bool removeAll(
	);

protected :

	template < typename T >
	static void addIndex(
			Map< T, HttpCookieCArray > &
					store,
		const	T &		key,
			HttpCookieCPtr	cookie
	);

	static URL getEffectiveURL(
		const	URL &		url
	);

private :

	HttpCookieCArray		allCookies;
	Map< Buffer, HttpCookieCArray >	domainIndex;
	Map< URL, HttpCookieCArray >	uriIndex;
	FastCS				crit;

	/// \brief Forbidden copy constructor.

	InMemCookieStore(
		const	InMemCookieStore &
	);

	/// \brief Forbidden copy operator.

	InMemCookieStore & operator = (
		const	InMemCookieStore &
	);

};

// ============================================================================

template < typename T >
void InMemCookieStore::addIndex(
		Map< T, HttpCookieCArray > &
				store,
	const	T &		key,
		HttpCookieCPtr	cookie
) {

	if ( ! store.contains( key ) ) {
		store.add( key );
	}

	HttpCookieCArray & cookies = store[ key ];

	cookies.remove( cookie );
	cookies.add( cookie );

}

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_InMemCookieStore_H_

#endif // 0

// ============================================================================

