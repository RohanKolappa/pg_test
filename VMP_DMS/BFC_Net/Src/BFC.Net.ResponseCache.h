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
//	BFC.Net.ResponseCache.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_ResponseCache_H_
#define _BFC_Net_ResponseCache_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, ResponseCache )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.CacheReader.h"
#include "BFC.Net.CacheWriter.h"
#include "BFC.Net.URL.h"
#include "BFC.Net.URLConnection.h"

#include "BFC.TL.List.h"
#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents implementations of URLConnection caches.
///
/// An instance of such a class can be registered with the system by using
/// ResponseCache::setDefault(), and the system will call this object in order
/// to:
/// -	store resource data which has been retrieved from an external source
///	into the cache,
/// -	try to fetch a requested resource that may have been stored in the
///	cache.
///
/// The ResponseCache implementation decides which resources should be cached,
/// and for how long they should be cached. If a request resource cannot be
/// retrieved from the cache, then the protocol handlers will fetch the
/// resource from its original location.
///
/// The settings for URLConnection::useCaches controls whether the protocol is
/// allowed to use a cached response.
///
/// For more information on HTTP caching, see
/// <a href="http://www.ietf.org/rfc/rfc2616.txt">RFC 2616: Hypertext
/// Transfer Protocol -- HTTP/1.1</a>
///
/// \ingroup BFC_Net

class BFC_Net_DLL ResponseCache : virtual public SObject {

public :

	/// \brief Gets the system-wide response cache.
	///
	/// \returns
	///	The system-wide ResponseCache.

	static ResponseCachePtr getDefault(
	) {
		return systCache;
	}

	/// \brief Sets (or unsets) the system-wide cache.
	///
	/// \note
	///	Non-standard procotol handlers may ignore this setting.
	///
	/// \param pSystCache
	///	The response cache, or null to unset the cache.

	static void setDefault(
			ResponseCachePtr	pSystCache
	) {
		systCache = pSystCache;
	}

	/// \brief Retrieves the cached response based on the requesting URL,
	///	request method and request headers.
	///
	/// Typically this method is called by the protocol handler before it
	/// sends out the request to get the network resource. If a cached
	/// response is returned, that resource is used instead.
	///
	/// \param url
	///	A URL used to reference the requested network resource.
	///
	/// \param rqstMethod
	///	A Buffer representing the request method.
	///
	/// \param rqstHeaders
	///	A Map from request header field names to lists of field
	///	values representing the current request headers.
	///
	/// \returns
	///	A CacheReader instance if available from cache, or null
	///	otherwise.

	virtual CacheReaderPtr get(
		const	URL &		url,
		const	Buffer &	rqstMethod,
		const	Map< Buffer, List< Buffer > > &
					rqstHeaders
	) = 0;

	/// \brief Called by the protocol handler after a resource has been
	///	retrieved, and the ResponseCache must decide whether or not to
	///	store the resource in its cache.
	///
	/// If the resource is to be cached, then put() must return a
	/// CacheWriter object which contains a BFC::IO::ByteOutputStream that
	/// the protocol handler will use to write the resource into the cache.
	/// If the resource is not to be cached, then put must return null.
	///
	/// \param url
	///	A URL used to reference the requested network resource.
	///
	/// \param conn
	///	A URLConnection instance that is used to fetch the response to
	///	be cached.
	///
	/// \returns
	///	A CacheWriter for recording the response to be cached. Null
	///	return indicates that the caller does not intend to cache the
	///	response.

	virtual CacheWriterPtr put(
		const	URL &		url,
		const	URLConnectionCPtr &	conn
	) = 0;

protected :

private :

	static ResponseCachePtr	systCache;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_ResponseCache_H_

#endif // 0

// ============================================================================

