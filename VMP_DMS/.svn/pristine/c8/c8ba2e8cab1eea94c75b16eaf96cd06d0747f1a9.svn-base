// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.PESPacketFramer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.PESPacket.h"
#include "VMP.M2S.PESPacketFramer.h"
#include "VMP.M2S.TSPacket.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PESPacketFramer )

// ============================================================================

M2S::PESPacketFramer::PESPacketFramer(
	const	Uchar		pStrType,
	const	Uchar		pStrMask ) :

	strType	( pStrType ),
	strMask	( pStrMask ),
	synced( false ) {

	setObjectName( "VMP.M2S.PESPacketFramer" );

}

// ============================================================================

void M2S::PESPacketFramer::putObject(
		SPtr		pTSPacket ) {

	TSPacketPtr	packet = pTSPacket;

	if ( ! packet->hasPayload() ) {
		msgWrn( "consume(): no payload to consume!" );
		return;
	}

//	msgDbg( "Got TS packet!" );

	Buffer buf = packet->getPayload();

	if ( buf.isEmpty() ) {
		throw InvalidArgument( "Empty payload!" );
	}

	if ( packet->getPayloadUnitStartIndicator() ) {
//		msgDbg( "PUSI" );
		if ( curr ) {
			if ( ! curr->packetLength
			  && curr->packetDump.length() >= 6 ) {
				curr->forceReady();
				getPeerConsumer()->putObject( curr );
			}
			else {
				msgWrn( "Dropping incomplete old PES packet!" );
//				msgWrn( "buf len: " + Buffer( curr->data.length() ) );
//				msgWrn( "pck len: " + Buffer( curr->packetLength ) );
			}
		}
		curr = new PESPacket;
	}
	else if ( ! curr ) {
		return;
	}

	curr->decode( buf );

	if ( curr->packetReady ) {
//		msgDbg( "Got PES packet! Forwarding!" );
		getPeerConsumer()->putObject( curr );
		curr.kill();
	}

}

// ============================================================================

void M2S::PESPacketFramer::putBuffer(
	const	Buffer &	pBuffer ) {

	cache += pBuffer;

	Uint32	pos = 0;
	Uint32	len = cache.getLength();

	// Make sure we are sync'd...

	while ( ! synced ) {

		if ( pos + 4 > len ) {
			return;
		}

		if ( cache[ pos     ] == 0x00
		  && cache[ pos + 1 ] == 0x00
		  && cache[ pos + 2 ] == 0x01
		  && ( cache[ pos + 3 ] & strMask ) == strType ) {
			synced = true;
			break;
		}

		pos++;

	}

	for (;;) {

		if ( pos + 6 > len ) {
			return;
		}

		if ( cache[ pos     ] != 0x00
		  || cache[ pos + 1 ] != 0x00
		  || cache[ pos + 2 ] != 0x01
		  || ( cache[ pos + 3 ] & strMask ) != strType ) {
			if ( synced ) {
				msgWrn( "Lost sync!" );
				synced = false;
			}
			pos++;
			continue;
		}

		if ( ! synced ) {
			msgWrn( "Got sync!" );
			synced = true;
		}

		Uint32	exp = ( ( Uint32 )cache[ pos + 4 ] << 8 )
			    | ( ( Uint32 )cache[ pos + 5 ]      );

		if ( ! exp ) {
			throw InternalError( "putBytes(): PES has 0 length!" );
		}

		exp += 6;

		if ( pos + exp > len ) {
			return;
		}

		PESPacketPtr packet = new PESPacket;

		packet->decode( cache( pos, exp ) );

		if ( ! packet->packetReady ) {
			throw InternalError();
		}

		getPeerConsumer()->putObject( packet );

		pos += exp;

	}

}

// ============================================================================

