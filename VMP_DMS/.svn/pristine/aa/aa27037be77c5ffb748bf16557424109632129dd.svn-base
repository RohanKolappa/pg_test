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
//	BFC.Net.IPv4Impl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <windows.h>
#else
//#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
//#include <sys/socket.h>
#endif

#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Net.IPv4Addr.h"
#include "BFC.Net.IPv4Impl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, IPv4Impl )

// ============================================================================

Net::IPv4Impl::IPv4Impl() {

}

// ============================================================================

Net::IPv4ImplPtr Net::IPv4Impl::instance() {

	static IPv4ImplPtr i = new IPv4Impl;

	return i;

}

// ============================================================================

Net::IPAddrCPtr Net::IPv4Impl::getAnyLocal() const {

	static IPAddrCPtr anyLocal = new IPv4Addr(
			"0.0.0.0",
			0 );

	return anyLocal;

}

Net::IPAddrCPtr Net::IPv4Impl::getLoopback() const {

	static IPAddrCPtr loopback = new IPv4Addr(
			"localhost",
			System::Host2BE( 0x7F000001 ) );

	return loopback;

}

// ============================================================================

