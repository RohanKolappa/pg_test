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
//	BFC.Net.SockAddr.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_SockAddr_H_
#define _BFC_Net_SockAddr_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.SockAddrPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Net.Address.h"
#include "BFC.Net.IPAddr.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents an IP Socket Address (IP address + port number).
///
/// It can also be a pair (hostname + port number), in which case an attempt
/// will be made to resolve the hostname. If resolution fails then the address
/// is said to be unresolved but can still be used on some circumstances like
/// connecting through a proxy.
///
/// It provides an object used by sockets for binding, connecting, or
/// as returned values.
///
/// The wildcard is a special local IP address. It usually means "any" and
/// can only be used for bind operations.
///
/// \ingroup BFC_Net

class BFC_Net_DLL SockAddr : public Address {

public :

	/// \brief Creates a new SockAddr where the IP address is the wildcard
	///	address and the port number has value 0.

	SockAddr(
	);

	/// \brief Creates a new SockAddr where the IP address is the wildcard
	///	address and the port number a specified value.
	///
	/// A valid port value is between 0 and 65535. A port number of 0 will
	/// let the system pick up an ephemeral port in a bind operation.
	///
	/// \param pPort
	///	The port number.

	SockAddr(
		const	Uint16		pPort
	);

	/// \brief Creates a SockAddr from an IP address and a port number.
	///
	/// A valid port value is between 0 and 65535. A port number of 0 will
	/// let the system pick up an ephemeral port in a bind operation.
	///
	/// A null address will assign the wildcard address.
	///
	/// \param pAddr
	///	The IP address.
	///
	/// \param pPort
	///	The port number.

	SockAddr(
			IPAddrCPtr	pAddr,
		const	Uint16		pPort
	);

	/// \brief Creates a SockAddr from a hostname and a port number.
	///
	/// If \a pResolve is true, then an attempt will be made to resolve the
	/// hostname into an IPAddr. If that attempt fails, the address will be
	/// flagged as unresolved.
	///
	/// A valid port value is between 0 and 65535. A port number of 0 will
	/// let the system pick up an ephemeral port in a bind operation.
	///
	/// \param pHost
	///	The host name.
	///
	/// \param pPort
	///	The port number.
	///
	/// \param pResolve
	///	Try to resolve ?

	SockAddr(
		const	Buffer &	pHost,
		const	Uint16		pPort,
		const	Bool		pResolve = true
	);

	/// \brief Creates a copy of \a pOther.

	SockAddr(
		const	SockAddr &	pOther
	);

	/// \brief Copy constructor.

	SockAddr & operator = (
		const	SockAddr &	pOther
	);

	/// \brief Compares this object against \p pOther.
	///
	/// The result is true if and only if the argument is not null and it
	/// represents the same address as this object.
	///
	/// Two instances of SockAddr represent the same address if both the
	/// IPAddr (or hostnames if it is unresolved) and port numbers are
	/// equal. If both addresses are unresolved, then the hostname and the
	/// port number are compared.
	///
	/// \param pOther
	///	The SockAddr to compare against.
	///
	/// \returns
	///	true if the objects are the same; false otherwise.

	Bool operator == (
		const	SockAddr &	pOther
	) const;

	Bool operator != (
		const	SockAddr &	pOther
	) const {
		return ( ! operator == ( pOther ) );
	}

	virtual Int32 compareTo(
			SCPtr		pOther
	) const;

	/// \brief Checks whether the address has been resolved or not.
	///
	/// \returns
	///	True if the hostname couldn't be resolved into an IPAddr.

	Bool isUnresolved(
	) const {
		return ( ! addr );
	}

	/// \brief Gets the IPAddr.
	///
	/// \returns
	///	The IPAddr or null if it is unresolved.

	const IPAddrCPtr & getAddr(
	) const {
		return addr;
	}

	/// \brief Gets the port number.
	///
	/// \returns
	///	The port number.

	Uint16 getPort(
	) const {
		return port;
	}

	/// \brief Gets the hostname.
	///
	/// \param pReverse
	///	If true and the address was created using a literal, then
	///	perform a reverse lookup.
	///
	/// \returns
	///	The hostname part of the address.

	Buffer getHost(
		const	Bool		pReverse = true
	) const;

	virtual Buffer toBuffer(
		const	Bool		pReverse = true
	) const;

protected :

private :

	Buffer		host;	///< Hostname.
	IPAddrCPtr	addr;	///< IP address.
	Uint16		port;	///< Port number.

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_SockAddr_H_

// ============================================================================

