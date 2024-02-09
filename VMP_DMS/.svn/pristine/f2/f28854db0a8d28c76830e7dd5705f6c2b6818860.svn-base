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
//	BFC.Net.Proxy.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_Proxy_H_
#define _BFC_Net_Proxy_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.ProxyPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Net.PassAuth.h"
#include "BFC.Net.SockAddr.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents a proxy setting, typically a type (http, socks) and
///	a socket address. 
///
/// \ingroup BFC_Net

class BFC_Net_DLL Proxy : virtual public SObject {

public :

	/// \brief Represents the proxy type.

	enum Type {
		DIRECT,	///< Direct connection, or absence of a proxy.
		HTTP,	///< High level protocols such as HTTP or FTP.
		SOCKS	///< SOCKS (V4 or V5) proxy.
	};

	/// \brief Creates a new Proxy that represents a DIRECT connection.

	Proxy(
	) :
		type	( DIRECT ) {
	}

	/// \brief Creates a new Proxy.
	/// 
	/// For types HTTP and SOCKS, a SockAddr must be provided.

	Proxy(
		const	Type		pType,
		const	SockAddrCPtr &	pAddr,
		const	PassAuthCPtr &	pAuth = 0
	) :
		type	( pType ),
		addr	( pAddr ),
		auth	( pAuth ) {
	}

	/// \brief Compares this object against the specified object.
	///
	/// The result is true if and only if the argument represents the same
	/// proxy as this object.
	///
	/// Two instances of Proxy represent the same address if both the
	/// SockAddr and Type are equal.
	///
	/// \param pOther
	///	The object to compare against.
	///
	/// \returns
	///	true if the objects are the same; false otherwise.

	Bool operator == (
		const	Proxy &		pOther
	) const;

	Bool operator != (
		const	Proxy &		pOther
	) const {
		return ( ! operator == ( pOther ) );
	}

	/// \brief Returns the proxy type.
	///
	/// \returns
	///	A Type representing the proxy type.

	Type getType(
	) const {
		return type;
	}

	/// \brief Returns the socket address of the proxy, or null if it's a
	///	direct connection.
	///
	/// \returns
	///	A SockAddr representing the socket end point of the proxy.

	SockAddrCPtr getAddr(
	) const {
		return addr;
	}

	/// \brief Returns the password authentication for the proxy, or null
	///	if authentication is not needed.

	PassAuthCPtr getAuth(
	) const {
		return auth;
	}

	/// \brief Constructs a string representation of this Proxy.
	///
	/// This string is constructed by calling toBuffer() on its type
	/// and concatenating " @ " and the toBuffer() result from its address
	/// if its type is not DIRECT.
	///
	/// \returns
	///	A string representation of this object.

	Buffer toBuffer(
	) const;

	/// \brief A proxy setting that represents a DIRECT connection,
	///	basically telling the protocol handler not to use any proxying.
	///
	/// Used, for instance, to create sockets bypassing any other global
	/// proxy settings (like SOCKS):
	/// \code
	///	SocketPtr s = new Socket( Proxy::NO_PROXY );
	/// \endcode

	static ProxyCPtr NO_PROXY;

protected :

private :

	Type		type;
	SockAddrCPtr	addr;
	PassAuthCPtr	auth;

	/// \brief Forbidden copy operator.

	Proxy & operator = (
		const	Proxy &		pOther
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Proxy_H_

// ============================================================================

