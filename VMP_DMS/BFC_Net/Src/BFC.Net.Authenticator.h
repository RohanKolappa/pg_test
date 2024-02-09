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
//	BFC.Net.Authenticator.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_Authenticator_H_
#define _BFC_Net_Authenticator_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, Authenticator )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.IPAddr.h"
#include "BFC.Net.PassAuth.h"
#include "BFC.Net.URL.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents an object that knows how to obtain authentication for a
///	network connection. Usually, it will do this by prompting the user for
///	information.
///
/// Applications use this class by overriding getPassAuth() in a
/// sub-class. This method will typically use the various getXXX() accessor
/// methods to get information about the entity requesting authentication. It
/// must then acquire a username and password either by interacting with the
/// user or through some other non-interactive means. The credentials are then
/// returned as a PassAuth return value.
///
/// An instance of this concrete sub-class is then registered with the system
/// by calling setDefault( Authenticator ). When authentication is required,
/// the system will invoke one of the requestPassAuth() methods
/// which in turn will call the getPassAuth() method of the
/// registered object.
///
/// All methods that request authentication have a default implementation that
/// fails.
///
/// \ingroup BFC_Net

class BFC_Net_DLL Authenticator : virtual public SObject {

public :

	/// \brief The type of the entity requesting authentication.

	enum RequestorType {
		PROXY,	///< Entity requesting authentication is a HTTP proxy server.
		SERVER	///< Entity requesting authentication is a HTTP origin server.
	};

	/// \brief Sets the authenticator that will be used by the networking
	///	code when a proxy or an HTTP server asks for authentication.
	///
	/// \param pSystAuth
	///	The authenticator to be set. If a is null, then any previously
	///	set authenticator is removed.

	static void setDefault(
			AuthenticatorPtr	pSystAuth
	) {
		systAuth = pSystAuth;
	}

	static AuthenticatorPtr getDefault(
	) {
		return systAuth;
	}

	/// \brief Asks the authenticator that has been registered with the
	///	system for a password.
	///
	/// \param addr
	///	The IPAddr of the site requesting authorization, or null
	///	if not known.
	///
	/// \param port
	///	The port for the requested connection.
	///
	/// \param protocol
	///	The protocol that's requesting the connection.
	///
	/// \param prompt
	///	A prompt string for the user.
	///
	/// \param scheme
	///	The authentication scheme.
	///
	/// \returns
	///	The username/password, or null if one can't be gotten.

	PassAuthPtr requestPassAuth(
		const	IPAddrCPtr &	addr,
		const	Uint16		port,
		const	Buffer &	protocol,
		const	Buffer &	prompt,
		const	Buffer &	scheme
	);

	/// \brief Asks the authenticator that has been registered with the
	///	system for a password.
	///
	/// This is the preferred method for requesting a password because the
	/// hostname can be provided in cases where the IPAddr is not
	/// available.
	///
	/// \param host
	///	The hostname of the site requesting authentication.
	///
	/// \param addr
	///	The IPAddr of the site requesting authentication, or null if
	///	not known.
	///
	/// \param port
	///	The port for the requested connection.
	///
	/// \param protocol
	///	The protocol that's requesting the connection.
	///
	/// \param prompt
	///	A prompt string for the user which identifies the
	///	authentication realm.
	///
	/// \param scheme
	///	The authentication scheme.
	///
	/// \returns
	///	The username/password, or null if one can't be gotten.

	PassAuthPtr requestPassAuth(
		const	Buffer &	host,
		const	IPAddrCPtr &	addr,
		const	Uint16		port,
		const	Buffer &	protocol,
		const	Buffer &	prompt,
		const	Buffer &	scheme
	);

	/// \brief Asks the authenticator that has been registered with the
	///	system for a password.
	///
	/// \param host
	///	The hostname of the site requesting authentication.
	///
	/// \param addr
	///	The IPAddr of the site requesting authorization, or null if not
	///	known.
	///
	/// \param port
	///	The port for the requested connection.
	///
	/// \param protocol
	///	The protocol that's requesting the connection.
	///
	/// \param prompt
	///	A prompt string for the user.
	///
	/// \param scheme
	///	The authentication scheme.
	///
	/// \param url
	///	The requesting URL that caused the authentication.
	///
	/// \param reqType
	///	The type (server or proxy) of the entity requesting
	///	authentication.
	///
	/// \returns
	///	The username/password, or null if one can't be gotten.

	PassAuthPtr requestPassAuth(
		const	Buffer &	host,
		const	IPAddrCPtr &	addr,
		const	Uint16		port,
		const	Buffer &	protocol,
		const	Buffer &	prompt,
		const	Buffer &	scheme,
		const	URL &		url,
		const	RequestorType	reqType
	);

protected :

	/// \brief Called when password authorization is needed.
	///
	/// Subclasses should implement this method.
	///
	/// \returns
	///	The PassAuth collected from the user, or null if
	///	none is provided.

	virtual PassAuthPtr getPassAuth(
	);

	/// \brief Gets the hostname of the site or proxy requesting
	///	authentication, or null if not available.
	///
	/// \returns
	///	The hostname of the connection requiring authentication, or
	///	null if it's not available.

	const Buffer & getRequestingHost(
	) const {
		return host;
	}

	/// \brief Gets the IPAddr of the site requesting authorization, or
	///	null if not available.
	///
	/// \returns
	///	The IPAddr of the site requesting authorization, or null if
	///	it's not available.

	const IPAddrCPtr & getRequestingSite(
	) const {
		return addr;
	}

	/// \brief Gets the port number for the requested connection.
	///
	/// \returns
	///	An int indicating the port for the requested connection.

	Uint16 getRequestingPort(
	) const {
		return port;
	}

	/// \brief Gives the protocol that's requesting the connection.
	///
	/// Often this will be based on a URL, but in a future BFC it could be,
	/// for example, "SOCKS" for a password-protected SOCKS5 firewall.
	///
	/// \returns
	///	The protcol, optionally followed by "/version", where version
	///	is a version number.

	const Buffer & getRequestingProtocol(
	) const {
		return prot;
	}

	/// \brief Gets the prompt string given by the requestor.
	///
	/// \returns
	///	The prompt string given by the requestor (realm for http
	///	requests).

	const Buffer & getRequestingPrompt(
	) const {
		return text;
	}

	/// \brief Gets the scheme of the requestor (the HTTP scheme for an
	///	HTTP firewall, for example).
	///
	/// \returns
	///	The scheme of the requestor.

	const Buffer & getRequestingScheme(
	) const {
		return schm;
	}

	/// \brief Returns the URL that resulted in this request for
	///	authentication.
	///
	/// \returns
	///	The requesting URL.

	const URL & getRequestingURL(
	) const {
		return rURL;
	}

	/// \brief Returns whether the requestor is a Proxy or a Server.
	///
	/// \returns
	///	The authentication type of the requestor.

	RequestorType getRequestorType(
	) const {
		return type;
	}

	void reset(
	);

private :

	static AuthenticatorPtr	systAuth;

	FastCS		crit;
	Buffer		host;
	IPAddrCPtr	addr;
	Uint16		port;
	Buffer		prot;
	Buffer		text;
	Buffer		schm;
	URL		rURL;
	RequestorType	type;

	/// \brief Forbidden default constructor.

	Authenticator(
	);

	/// \brief Forbidden copy constructor.

	Authenticator(
		const	Authenticator &
	);

	/// \brief Forbidden copy operator.

	Authenticator & operator = (
		const	Authenticator &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Authenticator_H_

#endif // 0

// ============================================================================

