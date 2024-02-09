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
//	VMP.M2S.TSPacketFramer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.GPC.Consumer.h"

#include "VMP.M2S.TSPacket.h"
#include "VMP.M2S.TSPacketFramer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSPacketFramer )

// ============================================================================

M2S::TSPacketFramer::TSPacketFramer() :

	friendly	( false ),
	totlSize	(     0 ),
	byteOffs	(     0 ),
	isSynced	( false ),
	skipSize	(     0 ) {

	setObjectName( "VMP.M2S.TSPacketFramer" );

}

// ============================================================================

void M2S::TSPacketFramer::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	setAccess( WriteAccess );

}

void M2S::TSPacketFramer::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	resetAccess();

}

// ============================================================================

void M2S::TSPacketFramer::putBytes(
	const	Buffer &	pDataBuff ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

//	msgDbg( "putBuffer(): " + Utils::toHex( pDataBuff( 0, 188 * 4 ) ) );

	if ( ! strtTime.isStarted() ) {
		strtTime.start();
	}

	dataBuff += pDataBuff;

	Uint32 pos = 0;

	while ( pos + 188 <= dataBuff.getLength() ) {

		// Make sure we are [still] sync'ed...

		if ( dataBuff[ pos ] == 0x47 ) {
			if ( ! isSynced ) {
				if ( skipSize ) {
					msgWrn( "putBuffer(): Got sync! (skip: "
						+ Buffer( skipSize )
						+ ", pos: "
						+ Buffer( byteOffs + pos, Buffer::Base16 )
						+ ")" );
				}
				isSynced = true;
				dataBuff = dataBuff( pos );
				byteOffs += pos;
				pos = 0;
			}
			TSPacketPtr tsPacket = new TSPacket;
			try {
				tsPacket->decode( byteOffs + pos, dataBuff( pos, 188 ) );
			}
			catch ( Exception & e ) {
				if ( ! friendly ) {
					msgExc( e, "Can't decode TS packet!" );
					throw;
				}
				tsPacket.kill();
			}
			if ( tsPacket ) {
				getPeerConsumer()->putObject( tsPacket );
			}
			pos += 188;
		}
		else {
			if ( isSynced ) {
				msgWrn( "putBuffer(): Lost sync! (pos: "
					+ Buffer( byteOffs + pos, Buffer::Base16 )
					+ ", char: "
					+ Buffer( ( Uint32 )dataBuff[ pos ], Buffer::Base16 )
					+ ")" );
				isSynced = false;
				skipSize = 0;
			}
			skipSize++;
			pos++;
		}

	}

	totlSize += pDataBuff.getLength();
	byteOffs += pos;
	dataBuff = dataBuff( pos ); // safe, even if pos >= len!

}

// ============================================================================

Uint64 M2S::TSPacketFramer::seek(
	const	Uint64		/*pByteOffs*/ ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	throw NoRandomAccess();

}

Uint64 M2S::TSPacketFramer::tell() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return byteOffs + dataBuff.getLength();

}

Uint64 M2S::TSPacketFramer::length() const {

	return 0;

}

// ============================================================================

void M2S::TSPacketFramer::dumpStatus() {

	Int64 elapsed = strtTime.getElapsed();

	elapsed /= 1000;

	if ( ! elapsed ) {
		elapsed = 1;
	}

	msgDbg( "Consumed: " + Buffer( totlSize ) );
	msgDbg( "Elapsed : " + Buffer( (Double)elapsed / 1000.0 ) + " s"  );

	Int64 ratio = totlSize / elapsed;

	msgDbg( "Rate    : " + Buffer( ratio ) + " kB/s" );

}

// ============================================================================

