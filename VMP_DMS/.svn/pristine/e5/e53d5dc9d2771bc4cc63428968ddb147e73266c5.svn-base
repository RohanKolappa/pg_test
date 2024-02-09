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
//	BFC.Net.CookiePolicy.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.CookiePolicy.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, CookiePolicy )

// ============================================================================

Net::CookiePolicyCPtr Net::CookiePolicy::ACCEPT_ALL = new Net::CookiePolicyAcceptAll;

Net::CookiePolicyCPtr Net::CookiePolicy::ACCEPT_NONE = new Net::CookiePolicyAcceptNone;

Net::CookiePolicyCPtr Net::CookiePolicy::ACCEPT_SERVER = new Net::CookiePolicyAcceptServer;

// ============================================================================

Bool Net::CookiePolicyAcceptAll::shouldAccept(
	const	URL &		,
	const	HttpCookieCPtr & ) const {

	return true;

}

Bool Net::CookiePolicyAcceptNone::shouldAccept(
	const	URL &		,
	const	HttpCookieCPtr & ) const {

	return false;

}

Bool Net::CookiePolicyAcceptServer::shouldAccept(
	const	URL &		url,
	const	HttpCookieCPtr &
				cookie ) const {

	return HttpCookie::domainMatches(
			cookie->getDomain(),
			url.getHost() );

}

// ============================================================================

