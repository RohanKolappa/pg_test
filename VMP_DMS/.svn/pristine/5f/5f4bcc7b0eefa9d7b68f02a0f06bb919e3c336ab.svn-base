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
//	BFC.Net.UUIdGenerator.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )
#else
#	include <string.h>
#	include <unistd.h>
#	include <sys/ioctl.h>
#	include <sys/socket.h>
#	include <sys/time.h>
#	include <sys/types.h>
#	include <netinet/in.h>
#	include <net/if.h>
#endif

#include "BFC.Math.Utils.h"
#include "BFC.Net.Socket.h"
#include "BFC.Net.UUIdGenerator.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, UUIdGenerator )

// ============================================================================

Net::UUIdGenerator::UUIdGenerator() {

	lastTime	= Time::now() - Time::Delta( 1, 0 );
	clockSeq	= Math::rand( ( Uint32 )0x3FFF ) | 0x8000; // variant 2!
	clockAdj	= 0;

	try {
		node_id = getNodeId().getRaw();
	}
	catch ( Exception & ) {
		node_id = Buffer::rand( 6 );
		node_id[ 0 ] |= 0x01;
	}

}

// ============================================================================

Net::UUIdGeneratorPtr Net::UUIdGenerator::instance() {

	static UUIdGeneratorPtr i = new UUIdGenerator;

	return i;

}

// ============================================================================

UUId Net::UUIdGenerator::generate() {

	Uint32	clock_low;
	Uint32	clock_mid;

	getClock( clock_mid, clock_low );

	return UUId(
		clock_low,
		clock_mid,
		((clock_mid >> 16) & 0x0FFF) | 0x1000,
		clockSeq,
		node_id
	);

}

// ============================================================================

const Uint32 Net::UUIdGenerator::MAX_ADJUSTMENT = 10;

// ============================================================================

void Net::UUIdGenerator::getClock(
		Uint32 &	clock_high,
		Uint32 &	clock_low ) {

	for (;;) {

		Time::Clock tv = Time::now();

		if ( tv == lastTime ) {
			if ( clockAdj >= MAX_ADJUSTMENT ) {
				continue;
			}
			clockAdj++;
		}
		else {
			clockAdj = 0;
			lastTime = tv;
		}

		Uint64	ts = tv.getSecond();
		Uint64	tu = tv.getuSecond();
		
		Uint64	clock_reg;

		clock_reg = tu * 10 + clockAdj;
		clock_reg += ts * 10000000;
		clock_reg += (((Uint64)0x01B21DD2) << 32) + 0x13814000;

		clock_high  = ( Uint32 )( clock_reg >> 32 );
		clock_low   = ( Uint32 )( clock_reg       );

		return;

	}

}

// ============================================================================

Net::MACAddress Net::UUIdGenerator::getNodeId() {

#if defined( PLATFORM_WIN32 )

	return MACAddress();

#else

	BufferArray	devs = Socket::getDevices();

	for ( Uint32 i = 0 ; i < devs.size() ; i++ ) {
		MACAddress mac;
		try {
			mac = Socket::getMACAddress( devs[ i ] );
		}
		catch ( Exception & ) {
			continue;
		}
		if ( mac.isOK() && ! mac.isNull() ) {
			return mac;
		}
	}

	return MACAddress();

#endif

}

// ============================================================================

