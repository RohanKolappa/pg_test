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
//	BFC.Net.Proxy.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.Proxy.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, Proxy )

// ============================================================================

Bool Net::Proxy::operator == (
	const	Proxy &		pOther ) const {

	return ( type == pOther.type
	    && ( ( addr && pOther.addr && *addr == *pOther.addr )
	      || ( ! addr && ! pOther.addr ) ) );

}

// ============================================================================

Buffer Net::Proxy::toBuffer() const {

	static const char * tbl[] = {
		"DIRECT",
		"HTTP",
		"SOCKS"
	};

	Buffer res( tbl[ type ] );

	if ( type != DIRECT ) {
		res += Buffer( " @ " ) + addr->toBuffer();
	}

	return res;

}

// ============================================================================

Net::ProxyCPtr Net::Proxy::NO_PROXY = new Proxy;

// ============================================================================

