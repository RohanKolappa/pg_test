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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
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
//	BFC.Net.ProxySelector.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_ProxySelector_H_
#define _BFC_Net_ProxySelector_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, ProxySelector )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Net.Proxy.h"
#include "BFC.Net.URL.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Selects the proxy server to use, if any, when connecting to the
///	network resource referenced by a URL.
///
/// A proxy selector is a concrete sub-class of this class and is registered by
/// invoking the setDefault() method. The currently registered proxy selector
/// can be retrieved by calling getDefault() method.
/// 
/// When a proxy selector is registered, for instance, a subclass of
/// URLConnection class should call the select() method for each URL request so
/// that the proxy selector can decide if a direct, or proxied connection
/// should be used. The select() method returns an array with the preferred
/// connection approaches.
/// 
/// If a connection cannot be established to a proxy (PROXY or SOCKS) servers
/// then the caller should call the proxy selector's connectFailed() method to
/// notify the proxy selector that the proxy server is unavailable.
/// 
/// \ingroup BFC_Net

class BFC_Net_DLL ProxySelector : virtual public SObject {

public :

	/// \brief Gets the system-wide proxy selector.
	///
	/// \returns
	///	The system-wide ProxySelector.

	static ProxySelectorPtr getDefault(
	) {
		return dfltSelector;
	}
  
	/// \brief Sets (or unsets) the system-wide proxy selector.
	///
	/// \note
	///	Non-standard protocol handlers may ignore this setting.
	///
	/// \param pSelector
	///	The proxy selector, or null to unset the proxy selector.

	static void setDefault(
			ProxySelectorPtr	pSelector
	) {
		dfltSelector = pSelector;
	}
	
	/// \brief Selects all the applicable proxies based on the protocol to
	///	access the resource with and a destination address to access
	///	the resource at.
	///
	/// The format of the URL is defined as follow:
	/// -	http URL for http connections
	/// -	https URL for https connections
	/// -	ftp URL for ftp connections
	/// -	socket://host:port for tcp client sockets connections.
	///
	/// \param url
	/// 	The URL that a connection is required to.
	///
	/// \returns
	///	An array of proxies. When no proxy is available, the list will
	///	contain one element that represents a direct connection.

	virtual ProxyCArray select(
		const	URL &		url
	) = 0;

	/// \brief Called to indicate that a connection could not be
	///	established to a proxy/socks server.
	///
	/// An implementation of this method can temporarily remove the proxies
	/// or reorder the sequence of proxies returned by select(), using the
	/// address provided.
	///
	/// \param url
	///	The URL that the proxy at address \p addr failed to serve.
	///
	/// \param addr
	///	The socket address of the proxy/SOCKS server.

	virtual void connectFailed(
		const	URL &		url,
		const	SockAddrCPtr &	addr
	) = 0;

protected :

private :

	static ProxySelectorPtr dfltSelector;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_ProxySelector_H_

// ============================================================================

