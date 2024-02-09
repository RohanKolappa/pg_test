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
//	BFC.Net.IPv4Addr.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_IPv4Addr_H_
#define _BFC_Net_IPv4Addr_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.IPv4AddrPtr.h"

// ============================================================================

#include "BFC.Net.IPAddr.h"
#include "BFC.Net.IPv4Address.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents an Internet Protocol version 4 (IPv4) address.
///
/// Defined by:
/// -	RFC 790: Assigned Numbers (http://www.ietf.org/rfc/rfc790.txt)
/// -	RFC 1918: Address Allocation for Private Internets (http://www.ietf.org/rfc/rfc1918.txt)
/// -	RFC 2365: Administratively Scoped IP Multicast (http://www.ietf.org/rfc/rfc2365.txt).
///
/// \section sec1 Textual representation of IP addresses
///
/// Textual representation of IPv4 address used as input to methods takes one
/// of the following forms:
/// -	<tt>d.d.d.d</tt>
/// -	<tt>d.d.d</tt>
/// -	<tt>d.d</tt>
/// -	<tt>d</tt>
///
/// When four parts are specified, each is interpreted as a byte of data and
/// assigned, from left to right, to the four bytes of an IPv4 address.
///
/// When a three part address is specified, the last part is interpreted as a
/// 16-bit quantity and placed in the right most two bytes of the network
/// address. This makes the three part address format convenient for specifying
/// Class B net- work addresses as 128.net.host.
///
/// When a two part address is supplied, the last part is interpreted as a
/// 24-bit quantity and placed in the right most three bytes of the network
/// address. This makes the two part address format convenient for specifying
/// Class A network addresses as net.host.
///
/// When only one part is given, the value is stored directly in the network
/// address without any byte rearrangement.
///
/// For methods that return a textual representation as output value, the first
/// form, i.e. a dotted-quad string, is used.
///
/// \section scope The Scope of a Multicast Address
///
/// Historically the IPv4 TTL field in the IP header has doubled as a multicast
/// scope field: a TTL of 0 means node-local, 1 means link-local, up through 32
/// means site-local, up through 64 means region-local, up through 128 means
/// continent-local, and up through 255 are global. However, the administrative
/// scoping is preferred. Please refer to <a href="http://www.ietf.org/rfc/rfc2365.txt">
/// RFC 2365: Administratively Scoped IP Multicast</a>.
///
/// \ingroup BFC_Net

class BFC_Net_DLL IPv4Addr : public IPAddr {

public :

	static const Uint32	INADDRSZ = 4;

	/// \brief Creates a new IPv4Addr.

	IPv4Addr(
	);

//	/// \brief Constructs a new IPv4Addr.
//	///
//	/// \param pHost
//	///	Host name.
//	///
//	/// \param pAddr
//	///	4-bytes long Buffer containing the raw IPv4 address, in network
//	///	byte order.
//
//	IPv4Addr(
//		const	Buffer &	pHost,
//		const	Buffer &	pAddr
//	);

	/// \brief Constructs a new IPv4Addr.
	///
	/// \param pAddr
	///	Raw IPv4 address, in network byte order!

	IPv4Addr(
		const	Uint32		pAddr
	);

	/// \brief Constructs a new IPv4Addr.
	///
	/// \param pHost
	///	Host name.
	///
	/// \param pAddr
	///	Raw IPv4 address, in network byte order!

	IPv4Addr(
		const	Buffer &	pHost,
		const	Uint32		pAddr = 0x00000000
	);

	/// \brief For backward compatibility.

	IPv4Addr(
		const	IPv4Address &	pAddr
	);

	virtual Bool equals(
		const	IPAddrCPtr &	obj
	) const;

	virtual Int32 compareTo(
			SCPtr		pOther
	) const;

	virtual Buffer getAddr(
	) const;

	/// \brief Returns the IP address as an Uint32 in NBO.

	Uint32 getUint32(
	) const;

	virtual Buffer getRawAddr(
	) const;

	virtual Bool isMulticast(
	) const;

	virtual Bool isAnyLocal(
	) const;

	virtual Bool isLoopback(
	) const;

	virtual Bool isLinkLocal(
	) const;

	virtual Bool isSiteLocal(
	) const;

	virtual Bool isMCGlobal(
	) const;

	virtual Bool isMCNodeLocal(
	) const;

	virtual Bool isMCLinkLocal(
	) const;

	virtual Bool isMCSiteLocal(
	) const;

	virtual Bool isMCOrgLocal(
	) const;

	/// \brief Converts the textual dot address \p pDotAddress into its
	///	raw numerical value (in network byte order).
	///
	/// \throws InvalidArgument
	///	If \p pDotAddress cannot be converted.
	///
	/// \return
	///	The converted numerical value, in network byte order!

	static Uint32 convertDotToRaw(
		const	Buffer &	pDotAddress
	);

protected :

	virtual Buffer lookupName(
	) const;

	/// \brief Converts IPv4 binary address into a string suitable for
	///	presentation.
	///
	/// \param pAddr
	///	A byte array representing an IPv4 numeric address in network
	///	byte order.
	///
	/// \return
	///	A Buffer representing the IPv4 address in textual (dot)
	///	representation format.

	static Buffer numericToTextFormat(
		const	Uint32		pAddr
	);

private :

	Uint32		address;	///< Holds a 32-bit IPv4 address, in NBO.

	/// \brief Forbidden copy constructor.

	IPv4Addr(
		const	IPv4Addr &
	);

	/// \brief Forbidden copy operator.

	IPv4Addr & operator = (
		const	IPv4Addr &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_IPv4Addr_H_

// ============================================================================

