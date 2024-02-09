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
//	BFC.Net.IPAddr.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_IPAddr_H_
#define _BFC_Net_IPAddr_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.IPAddrPtr.h"

// ============================================================================

#include "BFC.Base.Comparable.h"

#include "BFC.Net.IPImpl.h"
#include "BFC.Net.NetIface.h"

// ============================================================================

struct sockaddr;

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents an Internet Protocol (IP) address.
///
/// \section intro Introduction
///
/// An IP address is either a 32-bit or 128-bit unsigned number used by IP, a
/// lower-level protocol on which protocols like UDP and TCP are built. The IP
/// address architecture is defined by:
/// -	RFC 790: Assigned Numbers (http://www.ietf.org/rfc/rfc790.txt)
/// -	RFC 1918: Address Allocation for Private Internets (http://www.ietf.org/rfc/rfc1918.txt)
/// -	RFC 2365: Administratively Scoped IP Multicast (http://www.ietf.org/rfc/rfc2365.txt)
/// -	RFC 2373: IP Version 6 Addressing Architecture (http://www.ietf.org/rfc/rfc2373.txt).
///
/// An instance of an IPAddr consists of an IP address and possibly its
/// corresponding host name (depending on whether it is constructed with a host
/// name or whether it has already done reverse host name resolution).
///
/// \section address_types Address types
/// 
/// Types of address:
/// -	Unicast: identifies a single interface. A packet sent to a unicast
///	address is delivered to the interface identified by that address.
///	-	The \a Unspecified Address: Also called anylocal or wildcard
/// 		address. It must never be assigned to any node. It indicates
///		the absence of an address. One example of its use is as the
///		target of bind, which allows a server to accept a client
///		connection on any interface, in case the server host has
///		multiple interfaces.<br>
///		The \a Unspecified address must not be used as the destination
///		address of an IP packet.
///	-	The \a Loopback Address: This is the address assigned to the
///		loopback interface. Anything sent to this IP address loops
///		around and becomes IP input on the local host. This address is
///		often used when testing a client.
/// -	Multicast: identifies a set of interfaces (typically belonging to
///	different nodes). A packet sent to a multicast address is delivered to
///	all interfaces identified by that address.
/// 
/// \section address_scope IP address scope
/// 
/// <i>Link-local</i> addresses are designed to be used for addressing on a
/// single link for purposes such as auto-address configuration, neighbor
/// discovery, or when no routers are present.
/// 
/// <i>Site-local</i> addresses are designed to be used for addressing inside
/// of a site without the need for a global prefix.
/// 
/// <i>Global</i> addresses are unique across the internet.
/// 
/// \section text Textual representation of IP addresses
/// 
/// The textual representation of an IP address is address family specific.
/// 
/// For IPv4 address format, please refer to IPv4Addr documentation. For IPv6
/// address format, please refer to IPv6Addr documentation.
/// 
/// \section dns Host Name Resolution
/// 
/// Host name-to-IP address \e resolution is accomplished through the use of a
/// combination of local machine configuration information and network naming
/// services such as the Domain Name System (DNS) and Network Information
/// Service (NIS). The particular naming services(s) being used is by default
/// the local machine configured one. For any host name, its corresponding IP
/// address is returned.
/// 
/// <i>Reverse name resolution</i> means that for any IP address, the host
/// associated with the IP address is returned.
/// 
/// The IPAddr class provides methods to resolve host names to their IP
/// addresses and vice versa.
/// 
/// \ingroup BFC_Net

class BFC_Net_DLL IPAddr : virtual public Comparable {

protected :

	IPAddr(
		const	Uint32		pFamily,
		const	Buffer &	pHostName = Buffer()
	) :
		family	( pFamily ),
		name	( pHostName ) {
	}

public :

	/// \brief Returns an IPAddr object given the raw IP address.
	///
	/// The argument is in network byte order: the highest order byte of
	/// the address is in pRaw[ 0 ].
	///
	/// This method doesn't block, i.e. no reverse name service lookup is
	/// performed.
	///
	/// IPv4 address byte array must be 4 bytes long and IPv6 byte array
	/// must be 16 bytes long.
	///
	/// \param pRaw
	///	The raw IP address in network byte order (4 or 16 bytes).
	///
	/// \return
	///	An IPAddr object created from the raw IP address.

	static IPAddrCPtr createNew(
		const	Buffer &	pRaw
	);

	/// \brief Creates an IPAddr based on the provided host name and IP
	///	address.
	///
	/// No name service is checked for the validity of the address.
	///
	/// The host name can either be a machine name, such as "java.sun.com",
	/// or a textual representation of its IP address.
	///
	/// No validity checking is done on the host name either.
	///
	/// If pRaw specifies an IPv4 address, an instance of IPv4Addr will be
	/// returned; otherwise, an instance of IPv6Addr will be returned.
	///
	/// IPv4 address byte array must be 4 bytes long and IPv6 byte array
	/// must be 16 bytes long.
	///
	/// \param pName
	///	The specified host.
	///
	/// \param pRaw
	///	The raw IP address in network byte order (4 or 16 bytes).
	///
	/// \return
	///	An IPAddr object created from the raw IP address.

	static IPAddrCPtr createNew(
		const	Buffer &	pName,
		const	Buffer &	pRaw
	);

	static IPAddrPtr createNew(
		const	sockaddr *	pSockAddr
	);

	/// \brief Specify the address family: Internet Protocol, Version 4.

	static const Uint32 IPv4 = 1;

	/// \brief Specify the address family: Internet Protocol, Version 6.

	static const Uint32 IPv6 = 2;

	/// \brief Returns true iff this IPAddr is an IPv4 address.

	Bool isIPv4(
	) const {
		return ( family == IPv4 );
	}

	/// \brief Returns true iff this IPAddr is an IPv6 address.

	Bool isIPv6(
	) const {
		return ( family == IPv6 );
	}

	/// \brief Compares this object against the specified object.
	///
	/// The result is true if and only if the argument is not null and it
	/// represents the same IP address as this object.
	///
	/// Two instances of IPAddr represent the same IP address if the length
	/// of the byte arrays returned by getRawAddr() is the same for both,
	/// and each of the array components is the same for the byte arrays.
	///
	/// \param pOther
	///	The object to compare against.
	///
	/// \return
	///	true if the objects are the same; false otherwise.

	virtual Bool equals(
		const	IPAddrCPtr &	pOther
	) const = 0;

	/// \brief Returns true if this object has a non-empty host name.
	///
	/// This method returns true iff this object was created with a host
	/// name, or if a reverse name lookup has already been performed.
	///
	/// \return
	///	true if this object has a non-empty host name.

	Bool hasHostName(
	) const {
		return ( name.hasContent() );
	}

	/// \brief Gets the host name for this IP address.
	///
	/// If this IPAddr was created with a host name, this host name will
	/// be remembered and returned; otherwise, a reverse name lookup will
	/// be performed and the result will be returned based on the system
	/// configured name lookup service.
	///
	/// \return
	///	The host name for this IP address.

	const Buffer & getName(
	) {
		if ( name.isEmpty() ) {
			name = lookupName();
		}
		return name;
	}

	/// \brief Gets the host name for this IP address.
	///
	/// If this IPAddr was created with a host name, this host name will
	/// be remembered and returned; otherwise, an empty Buffer is returned.
	///
	/// \return
	///	The host name for this IP address.

	const Buffer & getName(
	) const {
		return name;
	}

	/// \brief Returns the IP address string in textual presentation.
	///
	/// \return
	///	The raw IP address in a string (e.g. IPv4 dotted) format.

	virtual Buffer getAddr(
	) const = 0;

	/// \brief Returns the raw IP address of this IPAddr.
	///
	/// The result is in network byte order: the highest order byte of the
	/// address is in getRawAddr()[0].
	///
	/// \return
	///	The raw IP address of this object (4 bytes for IPv4, 16 bytes
	///	for IPv6).

	virtual Buffer getRawAddr(
	) const = 0;

	/// \brief Utility routine to check if the IPAddr is an IP multicast
	///	address.
	///
	/// \return
	///	A Bool indicating if the IPAddr is an IP multicast address.

	virtual Bool isMulticast(
	) const = 0;

	/// \brief Utility routine to check if the IPAddr in a wildcard
	///	address.
	///
	/// \return
	///	A Bool indicating if the IPAddr is a wildcard address.

	virtual Bool isAnyLocal(
	) const = 0;

	/// \brief Utility routine to check if the IPAddr is a loopback
	///	address.
	///
	/// \return
	///	A Bool indicating if the IPAddr is a loopback address; or
	///	false otherwise.

	virtual Bool isLoopback(
	) const = 0;

	/// \brief Utility routine to check if the IPAddr is an link local
	///	address.
	///
	/// \return
	///	A Bool indicating if the IPAddr is a link local address; or
	///	false if address is not a link local unicast address.

	virtual Bool isLinkLocal(
	) const = 0;

	/// \brief Utility routine to check if the IPAddr is a site local
	///	address.
	///
	/// \return
	///	A Bool indicating if the IPAddr is a site local address; or
	///	false if address is not a site local unicast address.

	virtual Bool isSiteLocal(
	) const = 0;

	/// \brief Utility routine to check if the multicast address has
	///	global scope.
	///
	/// \return
	///	A Bool indicating if the address has is a multicast address of
	///	global scope, false if it is not of global scope or it is not a
	///	multicast address.

	virtual Bool isMCGlobal(
	) const = 0;

	/// \brief Utility routine to check if the multicast address has node
	///	scope.
	///
	/// \return
	///	A Bool indicating if the address has is a multicast address of
	///	node-local scope, false if it is not of node-local scope or it
	///	is not a multicast address.

	virtual Bool isMCNodeLocal(
	) const = 0;

	/// \brief Utility routine to check if the multicast address has link
	///	scope.
	///
	/// \return
	///	A Bool indicating if the address has is a multicast address of
	///	link-local scope, false if it is not of link-local scope or it
	///	is not a multicast address.

	virtual Bool isMCLinkLocal(
	) const = 0;

	/// \brief Utility routine to check if the multicast address has site
	///	scope.
	///
	/// \return
	///	A Bool indicating if the address has is a multicast address of
	///	site-local scope, false if it is not of site-local scope or it
	///	is not a multicast address.

	virtual Bool isMCSiteLocal(
	) const = 0;

	/// \brief Utility routine to check if the multicast address has
	///	organization scope.
	///
	/// \return
	///	A Bool indicating if the address has is a multicast address of
	///	organization-local scope, false if it is not of organization
	///	local scope or it is not a multicast address.

	virtual Bool isMCOrgLocal(
	) const = 0;

	/// \brief Converts this IP address to a Buffer. The string returned
	///	is of the form: "hostname / literal IP address".
	///
	/// If the host name is unresolved, no reverse name service lookup is
	/// performed. The hostname part will be represented by an empty
	/// string.
	///
	/// \return
	///	A string representation of this IP address.

	virtual Buffer toBuffer(
	) const;

	/// \brief Returns the default IP implementation used by the BFC_Net
	///	module.
	///
	/// By default, the BFC_Net module will use IPv4 instead of IPv6. You
	/// may override this by using setImpl().
	///
	/// \return
	///	The default IP implementation.

	static IPImplPtr getImpl(
	) {
		return impl;
	}

	/// \brief Sets the default IP implementation used by the BFC_Net
	///	module.
	///
	/// \param pImpl
	///	The implementation to use.

	static void setImpl(
			IPImplPtr	pImpl
	) {
		impl = pImpl;
	}

	/// \brief Returns the "any" ("unspecified", "wildcard") address.

	static IPAddrCPtr getAnyLocal(
	);

	/// \brief Returns the "loopback" address.

	static IPAddrCPtr getLoopback(
	);

	/// \brief Returns the host name.

	static Buffer getHostName(
	);

	/// \brief Returns the address of the local host.
	///
	/// This is achieved by retrieving the name of the host from the
	/// system, then resolving that name into an IPAddr.
	///
	/// \return
	///	The address of the local host.

	static IPAddrCPtr getLocalhost(
	);

	static Buffer rslvRawToName(
		const	Buffer &	pRaw
	);

	/// \brief Determines the IP address of a host, given the host's name.
	///
	/// The host name can either be a machine name, such as "java.sun.com",
	/// or a textual representation of its IP address. If a literal IP
	/// address is supplied, only the validity of the address format is
	/// checked.
	///
	/// For host specified in literal IPv6 address, either the form defined
	/// in RFC 2732 or the literal IPv6 address format defined in RFC 2373
	/// is accepted. IPv6 scoped addresses are also supported.
	///
	/// If the host is empty, then an IPAddr representing an address of the
	/// loopback interface is returned.
	///
	/// \param pHost
	///	The specified host, or null.
	///
	/// \return
	///	An IP address for the given host name.

	static IPAddrCPtr getByName(
		const	Buffer &	pHost
	);

	/// \brief Given the name of a host, returns an array of its IP
	///	addresses, based on the configured name service on the system.
	///
	/// The host name can either be a machine name, such as "java.sun.com",
	/// or a textual representation of its IP address. If a literal IP
	/// address is supplied, only the validity of the address format is
	/// checked.
	///
	/// For host specified in <i>literal IPv6 address</i>, either the form
	/// defined in RFC 2732 or the literal IPv6 address format defined in
	/// RFC 2373 is accepted. A literal IPv6 address may also be qualified
	/// by appending a scoped zone identifier or scope_id.
	///
	/// If the host is empty, then an <tt>IPAddr</tt> representing an
	/// address of the loopback interface is returned.
	///
	/// \param host
	///	The name of the host, or null.
	///
	/// \return
	///	An array of all the IP addresses for a given host name.

	static IPAddrCArray rslvNameToAddr(
		const	Buffer &	host,
			IPAddrCPtr	reqAddr = 0
	);

protected :

	/// \brief Returns the hostname for this address.
	///
	/// \return
	///	The host name for this IP address.

	virtual Buffer lookupName(
	) const = 0;

	static Uint32 digitToVal(
		const	Uchar		c
	) {
		return ( c >= '0' && c <= '9' )
			? ( Uint32 )( c - '0' )
			: ( Uint32 )-1;
	}

	static Bool isHexChar(
		const	Uchar		c
	) {
		return ( c >= '0' && c <= '9' )
		    || ( c >= 'a' && c <= 'f' )
		    || ( c >= 'A' && c <= 'F' );
	}

	static Uint32 checkNumericZone(
		const	Buffer &	s
	);

	Uint32		family;		///< Address family type.
	Buffer		name;		///< Host name.

	static FastCS		lock;

private :

	static IPImplPtr	impl;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_IPAddr_H_

// ============================================================================

