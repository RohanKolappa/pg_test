// ============================================================================

#if 0

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
//	BFC.Net.IfaceAddr.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_IfaceAddr_H_
#define _BFC_Net_IfaceAddr_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.IfaceAddrPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Net.IPAddrPtr.h"
#include "BFC.Net.IPv4AddrPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents a Network Interface address.
///
/// In short it's an IP address, a subnet mask and a broadcast address when the
/// address is an IPv4 one. An IP address and a network prefix length in the
/// case of IPv6 address.
///
/// \ingroup BFC_Net

class BFC_Net_DLL IfaceAddr : virtual public SObject {

protected :

	/// \brief Creates a new IfaceAddr.

	IfaceAddr(
	) :
		maskLength	( 0 ) {
	}

public :

	/// \brief Returns an IPAddr for this address.
	///
	/// \returns
	///	The IPAddr for this address.

	IPAddrCPtr getRawAddr(
	) const {
		return address;
	}

	void setRawAddr(
			IPAddrCPtr	pAddr
	) {
		address = pAddr;
	}

	/// \brief Returns an IPAddr for the brodcast address for this
	///	IfaceAddr.
	///
	/// Only IPv4 networks have broadcast address therefore, in the case
	/// of an IPv6 network, null will be returned.
	///
	/// \returns
	///	The IPAddr representing the broadcast address or null if there
	///	is no broadcast address.

	IPAddrCPtr getBroadcast(
	) const {
		return broadcast;
	}

	void setBroadcast(
			IPAddrCPtr	pAddr
	) {
		broadcast = pAddr;
	}

	/// \brief Returns the network prefix length for this address.
	///
	/// This is also known as the subnet mask in the context of IPv4
	/// addresses.
	///
	/// Typical IPv4 values would be 8 (255.0.0.0), 16 (255.255.0.0) or 24
	/// (255.255.255.0). Typical IPv6 values would be 128 (::1/128) or 10
	/// (fe80::203:baff:fe27:1243/10).
	///
	/// \returns
	///	The prefix length for the subnet of that address.

	Uint32 getNetworkPrefixLength(
	) const {
		return maskLength;
	}

	void setMaskLength(
		const	Uint32		pMaskLength
	) {
		maskLength = pMaskLength;
	}

	/// \brief Compares this object against the specified object.
	///
	/// The result is true if and only if the argument is not null and it
	/// represents the same interface address as this object.
	///
	/// Two instances of IfaceAddr represent the same address if the
	/// IPAddr, the prefix length and the broadcast are the same for both.
	///
	/// \param pOther
	///	The object to compare against.
	///
	/// \returns
	///	true if the objects are the same; false otherwise.

	Bool operator == (
		const	IfaceAddr &	pOther
	) const;

	/// \brief Converts this Interface address to a Buffer.
	///
	/// \returns
	///	A representation for this IfaceAddr of the form:
	///	"IPAddr / prefix length [ broadcast address ]".

	Buffer toBuffer(
	) const;

protected :

private :

	IPAddrCPtr	address;
	IPv4AddrCPtr	broadcast;
	Uint32		maskLength;

	/// \brief Forbidden copy constructor.

	IfaceAddr(
		const	IfaceAddr &
	);

	/// \brief Forbidden copy operator.

	IfaceAddr & operator = (
		const	IfaceAddr &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_IfaceAddr_H_

// ============================================================================

#endif // 0

// ============================================================================

