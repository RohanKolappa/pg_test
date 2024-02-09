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
//	BFC.Net.System.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.IPAddr.h"
#include "BFC.Net.System.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, System )

// ============================================================================

Net::System::System() {

	setObjectName( "BFC.Net.System" );

	try {
		refresh();
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't refresh!" );
	}

}

// ============================================================================

Net::SystemPtr Net::System::instance() {

	static SystemPtr i = new System;

	return i;

}

// ============================================================================

void Net::System::refresh() {

	NetIfaceArray ifs = NetIface::getNetworkInterfaces();

	devices.kill();

	for ( Uint32 i = 0 ; i < ifs.getSize() ; i++ ) {
		devices.add( ifs[ i ]->getName(), ifs[ i ] );
	}

	address.kill();

	for ( Uint32 i = 0 ; i < ifs.getSize() ; i++ ) {
		address += ifs[ i ]->getAddress();
	}

}

// ============================================================================

Uint32 Net::System::getNbrNetIfaces() const {

	return devices.getSize();

}

Net::NetIfaceCPtr Net::System::getNetIface(
	const	Uint32		pIndex ) const {

	return devices.getValue( pIndex );

}

// ============================================================================

Net::NetIfaceCPtr Net::System::findNetIface(
	const	Buffer &	pDevName ) const {

	return devices.get( pDevName, 0 );

}

Net::NetIfaceCPtr Net::System::findNetIface(
	const	Uint32		index ) const {

	for ( Uint32 i = 0 ; i < devices.getSize() ; i++ ) {
		if ( devices.getValue( i )->getIndex() == index ) {
			return devices.getValue( i );
		}
	}

	return 0;

}

Net::NetIfaceCPtr Net::System::findNetIface(
		IPAddrCPtr	pAddr ) const {

	if ( ! pAddr ) {
		throw InvalidArgument();
	}

	for ( Uint32 i = 0 ; i < devices.getSize() ; i++ ) {
		NetIfaceCPtr curr = devices.getValue( i );
		if ( curr->getAddress()->equals( pAddr ) ) {
			return curr;
		}
	}

	return 0;

}

// ============================================================================

Uint32 Net::System::getNbrIPAddrs() const {

	return address.getSize();

}

Net::IPAddrCPtr Net::System::getIPAddr(
	const	Uint32		pIndex ) const {

	return address[ pIndex ];

}

// ============================================================================

