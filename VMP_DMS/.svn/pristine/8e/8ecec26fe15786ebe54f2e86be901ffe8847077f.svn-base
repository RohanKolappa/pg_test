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
//	BFC.Net.InMemCookieStore.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#include "BFC.Base.FastCSLocker.h"

#include "BFC.Net.InMemCookieStore.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, InMemCookieStore )

// ============================================================================

void Net::InMemCookieStore::add(
	const	URL &		url,
	const	HttpCookieCPtr &
				cookie ) {

	if ( ! cookie ) {
		throw InvalidArgument();
	}

	FastCSLocker lock( crit );

	allCookies.remove( cookie );

	if ( cookie->getMaxAge() != Time::Delta( 0 ) ) {
		allCookies.add( cookie );
		addIndex( domainIndex, cookie->getDomain(), cookie );
		addIndex( uriIndex, getEffectiveURL( url ), cookie );
	}

}

Net::HttpCookieCArray Net::InMemCookieStore::get(
	const	URL &		/*pUrl*/ ) {

	throw NotImplemented();

// FIXME: we need URL::compareTo( const URL & )

//	if ( ! pUrl.isValid() ) {
//		throw InvalidArgument();
//	}
//
//	HttpCookieCArray	res;
//
//	FastCSLocker lock( crit );
//
//	for ( Uint32 i = 0 ; i < domainIndex.getSize() ; i++ ) {
//		Buffer domain = domainIndex.getKey( i );
//		if ( HttpCookie::domainMatches( domain, pUrl.getHost() ) ) {
//			HttpCookieCArray & sub = domainIndex.getValue( i );
//			Uint32 j = 0;
//			while ( j < sub.getSize() ) {
//				HttpCookieCPtr ck = sub[ j ];
//				if ( ! allCookies.contains( ck ) ) {
//					sub.pop( j );
//				}
//				else if ( ck->hasExpired() ) {
//					sub.pop( j );
//					allCookies.remove( ck );
//				}
//				else {
//					if ( ! res.contains( ck ) ) {
//						res.add( ck );
//					}
//					j++;
//				}
//			}
//		}
//	}
//
//	for ( Uint32 i = 0 ; i < uriIndex.getSize() ; i++ ) {
//		URL url = uriIndex.getKey( i );
//		if ( url.compareTo( getEffectiveURL( pUrl ) ) ) {
//			HttpCookieCArray & sub = uriIndex.getValue( i );
//			Uint32 j = 0;
//			while ( j < sub.getSize() ) {
//				HttpCookieCPtr ck = sub[ j ];
//				if ( ! allCookies.contains( ck ) ) {
//					sub.pop( j );
//				}
//				else if ( ck->hasExpired() ) {
//					sub.pop( j );
//					allCookies.remove( ck );
//				}
//				else {
//					if ( ! res.contains( ck ) ) {
//						res.add( ck );
//					}
//					j++;
//				}
//			}
//		}
//	}
//
//	return res;

}

Net::HttpCookieCArray Net::InMemCookieStore::getCookies() {

	FastCSLocker lock( crit );

	Uint32 i = 0;

	while ( i < allCookies.getSize() ) {
		if ( allCookies[ i ]->hasExpired() ) {
			allCookies.pop( i );
		}
		else {
			i++;
		}
	}

	return allCookies;

}

CompArray< Net::URL > Net::InMemCookieStore::getURIs() {

	FastCSLocker lock( crit );

	Uint32 i = 0;

	while ( i < uriIndex.getSize() ) {
		if ( uriIndex.getValue( i ).isEmpty() ) {
			uriIndex.pop( i );
		}
		else {
			i++;
		}
	}

	return uriIndex.getAllKeys();

}

Bool Net::InMemCookieStore::remove(
	const	URL &		/*url*/,
	const	HttpCookieCPtr &
				cookie ) {

	if ( ! cookie ) {
		throw InvalidArgument();
	}

	FastCSLocker lock( crit );

	Uint32	idx;
	Bool	res = allCookies.contains( cookie, idx );

	if ( res ) {
		allCookies.pop( idx );
	}

	return res;

}

Bool Net::InMemCookieStore::removeAll() {

	FastCSLocker lock( crit );

	allCookies.kill();
	domainIndex.kill();
	uriIndex.kill();

	return true;

}

// ============================================================================

Net::URL Net::InMemCookieStore::getEffectiveURL(
	const	URL &		/*url*/ ) {

	throw NotImplemented();

//	URL res;
//
//	try {
//		res = URL(url.getScheme(),
//			url.getAuth(),
//			null,  // path component
//			null,  // query component
//			null   // fragment component
//			);
//	}
//	catch ( Exception & ) {
//		res = url;
//	}
//
//	return res;

}

#endif // 0

// ============================================================================

