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
//	BFC.Net.IPAddrUtil.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_IPAddrUtil_H_
#define _BFC_Net_IPAddrUtil_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net

class BFC_Net_DLL IPAddrUtil {

public :

	/// \brief Converts IPv4 address in its textual presentation form into
	///	its numeric binary form.
	///
	/// When only one part is given, the value is stored directly in the
	/// network address without any byte rearrangement.
	///
	/// When a two part address is supplied, the last part is interpreted
	/// as a 24-bit quantity and placed in the right most three bytes of
	/// the network address. This makes the two part address format
	/// convenient for specifying Class A network addresses as "net.host".
	///
	/// When a three part address is specified, the last part is
	/// interpreted as a 16-bit quantity and placed in the right most two
	/// bytes of the network address. This makes the three part address
	/// format convenient for specifying Class B network addresses as
	/// "128.net.host".
	///
	/// When four parts are specified, each is interpreted as a byte of
	/// data and assigned, from left to right, to the four bytes of an IPv4
	/// address.
	///
	/// \returns
	///	A 4-byte long Buffer.

	static Buffer textToNumericFormatV4(
		const	Buffer &	src
	);

	/// \brief Converts IPv6 presentation level address to network order
	///	binary form.

	static Buffer textToNumericFormatV6(
		const	Buffer &	src
	);

	/// \brief Indicates whether src is an IPv4 literal address.
	///
	/// \param src
	///	A Buffer representing an IPv4 address in textual format.
	///
	/// \returns
	///	A Bool indicating whether src is an IPv4 literal address.

	static Bool isIPv4LiteralAddress(
		const	Buffer &	src
	);

	/// \brief Tests if \a pSrc represents an IPv6 address in textual format.

	static Bool isIPv6LiteralAddress(
		const	Buffer &	pSrc
	);

	/// \brief Convert IPv4-Mapped address to IPv4 address.
	///
	/// Both input and returned value are in network order binary form.

	static Buffer convertFromIPv4MappedAddress(
		const	Buffer &	addr
	);

private :

	static const Uint32 INADDR4SZ = 4;
	static const Uint32 INADDR16SZ = 16;
	static const Uint32 INT16SZ = 2;

	/// \brief Checks if the InetAddress is an IPv4 mapped IPv6 address.

	static Bool isIPv4MappedAddress(
		const	Buffer &	addr
	);

	static Uint32 charToVal(
		const	Uchar		c
	) {
		return ( c >= '0' && c <= '9' )
		     ? ( Uint32 )( c - '0' )
		     : ( c >= 'a' && c <= 'f' )
		     ? ( Uint32 )( c - 'a' + 10 )
		     : ( Uint32 )( c - 'A' + 10 );
	}

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_IPAddrUtil_H_

// ============================================================================

