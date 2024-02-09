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
//	BFC.Net.URLStreamHandler.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if 0

#ifndef _BFC_Net_URLStreamHandler_H_
#define _BFC_Net_URLStreamHandler_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.URLStreamHandlerPtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.IPAddr.h"
#include "BFC.Net.Proxy.h"
#include "BFC.Net.URLConnection.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Common superclass for all stream protocol handlers.
///
/// A stream protocol handler knows how to make a connection for a particular
/// protocol type, such as http, ftp, or gopher.
///
/// In most cases, an instance of a URLStreamHandler subclass is not created
/// directly by an application. Rather, the first time a protocol name is
/// encountered when constructing a URL, the appropriate stream protocol
/// handler is automatically loaded.
///
/// \ingroup BFC_Net

class BFC_Net_DLL URLStreamHandler : virtual public SObject {

protected :

	/// \brief Opens a connection to the object referenced by the URL
	///	argument.
	///
	/// If for the handler's protocol (such as HTTP), there exists a public
	/// specialized URLConnection subclass, the connection returned will be
	/// of that subclass. For example, for HTTP, an HttpURLConnection will
	/// be returned.
	///
	/// \param pURL
	///	The URL that this connects to.
	///
	/// \returns
	///	A URLConnection object for the URL.

	virtual URLConnectionPtr openConnection(
		const	URL &		pURL
	) = 0;

	/// \brief Same as openConnection(URL), except that the connection will
	///	be made through the specified proxy.
	///
	/// Protocol handlers that do not support proxying will ignore the
	/// proxy parameter and make a normal connection.
	///
	/// Calling this method preempts the system's default ProxySelector
	/// settings.
	///
	/// \param pURL
	///	The URL that this connects to.
	///
	/// \param pProxy
	///	The proxy through which the connection will be made.
	///
	/// \returns
	///	A URLConnection object for the URL.
	///
	/// \throw NotImplemented
	///	If this method is not implemented by the subclass.

	virtual URLConnectionPtr openConnection(
		const	URL &		pURL,
			ProxyPtr	pProxy
	);

	/// \brief Parses the string representation of a URL into a URL object.
	///
	/// If there is any inherited context, then it has already been copied
	/// into the URL argument.
	///
	/// The parseURL method of URLStreamHandler parses the string
	/// representation as if it were an http specification. Most URL
	/// protocol families have a similar parsing. A stream protocol handler
	/// for a protocol that has a different syntax must override this
	/// routine.
	///
	/// \param pURL
	///	The URL to receive the result of parsing the spec.
	///
	/// \param pSpec
	///	The Buffer representing the URL that must be parsed. Any
	///	leading protocol name or trailing fragment must be stripped.

	virtual void parseURL(
			URL &		pURL,
		const	Buffer &	pSpec
	) const;

	/// \brief Returns the default port for a URL parsed by this handler.
	///
	/// This method is meant to be overidden by handlers with default port
	/// numbers.
	///
	/// \returns
	///	The default port for a URL parsed by this handler.

	virtual Uint16 getDefaultPort(
	) const;

	/// \brief Provides the default equals calculation.
	///
	/// May be overidden by handlers for other protocols that have
	/// different requirements for equals().
	///
	/// \param pURL1
	///	A URL object.
	///
	/// \param pURL2
	///	A URL object.
	///
	/// \returns
	///	true if the two urls are considered equal, ie. they
	///	refer to the same fragment in the same file.

	virtual Bool equals(
		const	URL &		pURL1,
		const	URL &		pURL2
	) const;

	/// \brief Compares two urls to see whether they refer to the same
	///	file, i.e., having the same protocol, host, port, and path.
	///
	/// \param pURL1
	///	A URL object.
	///
	/// \param pURL2
	///	A URL object.
	///
	/// \returns
	///	true if pURL1 and pURL2 refer to the same file.

	virtual Bool sameFile(
		const	URL &		pURL1,
		const	URL &		pURL2
	) const;

	/// \brief Gets the IP address of our host.
	///
	/// An empty host field or a DNS failure will result in a null return.
	///
	/// \param pURL
	///	A URL object.
	///
	/// \returns
	///	An IPAddr representing the host IP address.

	virtual IPAddrCPtr getAddr(
		const	URL &		pURL
	) const;

	/// \brief Compares the host components of two URLs.
	///
	/// \param pURL1
	///	The URL of the first host to compare.
	///
	/// \param pURL2
	///	The URL of the second host to compare.
	///
	/// \returns
	///	true if and only if they are equal, false otherwise.

	virtual Bool hostsEqual(
		const	URL &		pURL1,
		const	URL &		pURL2
	) const;

	/// \brief Converts a URL of a specific protocol to a Buffer.
	///
	/// \param  pURL
	///	The URL.
	///
	/// \returns
	///	A string representation of the URL argument.

	virtual Buffer toExternalForm(
		const	URL &		pURL
	) const;

	/// \brief Sets the fields of the URL argument to the indicated values.
	///
	/// Only classes derived from URLStreamHandler are supposed to be able
	/// to call the set method on a URL.
	///
	/// \param pURL
	///	The URL to modify.
	///
	/// \param protocol
	///	The protocol name.
	///
	/// \param host
	///	The remote host value for the URL.
	///
	/// \param port
	///	The port on the remote machine.
	///
	/// \param authority
	///	The authority part for the URL.
	///
	/// \param userInfo
	///	The userInfo part of the URL.
	///
	/// \param path
	///	The path component of the URL.
	///
	/// \param query
	///	The query part for the URL.
	///
	/// \param ref
	///	The reference.

	virtual void setURL(
			URL &		pURL,
		const	Buffer &	protocol,
		const	Buffer &	host,
		const	Uint16		port,
		const	Buffer &	authority,
		const	Buffer &	userInfo,
		const	Buffer &	path,
		const	Buffer &	query,
		const	Buffer &	ref
	) const;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_URLStreamHandler_H_

#endif // 0

// ============================================================================

