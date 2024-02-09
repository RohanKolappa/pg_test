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
//	BFC.Net.IPv6Impl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.IPv6Impl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, IPv6Impl )

// ============================================================================

Net::IPv6Impl::IPv6Impl() {

}

// ============================================================================

IPAddrCArray Net::IPv6Impl::rslvNameToAddr(
	const	Buffer &	hostname ) const {

}

Buffer Net::IPv6Impl::rslvRawToName(
	const	Buffer &	addr ) const {

}

Net::IPAddrCPtr Net::IPv6Impl::getAnyLocalAddress() const {

	if (anyLocalAddress == null) {
		if (InetAddress.preferIPv6Address) {
			anyLocalAddress = new Inet6Address();
			anyLocalAddress.hostName = "::";
		} else {
			anyLocalAddress = (new Inet4AddressImpl()).anyLocalAddress();
		}
	}

	return anyLocalAddress;

}

Net::IPAddrCPtr Net::IPv6Impl::getLoopbackAddress() const {

	if (loopbackAddress == null) {
		 if (InetAddress.preferIPv6Address) {
			 byte[] loopback =
					{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
			 loopbackAddress = new Inet6Address("localhost", loopback);
		 } else {
			loopbackAddress = (new Inet4AddressImpl()).loopbackAddress();
		 }
	}

	return loopbackAddress;

}

// ============================================================================

#endif // 0

// ============================================================================

