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
//	VMP.M2S.TSPacketDispatcher.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.M2S.TSPacket.h"
#include "VMP.M2S.TSPacketDispatcher.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSPacketDispatcher )

// ============================================================================

M2S::TSPacketDispatcher::TSPacketDispatcher() :

	trErrCnt	( 0 ),
	pcktCntr	( 0 ),
	dropCntr	( 0 ) {

	setObjectName( "VMP.M2S.TSPacketDispatcher" );

}

// ============================================================================

void M2S::TSPacketDispatcher::setPeerConsumer(
	const	Uint32		pEStrmPid,
		GPC::ConsumerPtr
				pConsumer ) {

	checkPid( pEStrmPid );

	if ( tbl[ pEStrmPid ].consumer ) {
		throw InternalError( "Already got a consumer!" );
	}

	tbl[ pEStrmPid ].consumer = pConsumer;
	tbl[ pEStrmPid ].currCC   = InvalidCC;

}

void M2S::TSPacketDispatcher::delPeerConsumer(
	const	Uint32		pEStrmPid ) {

	checkPid( pEStrmPid );

	if ( ! tbl[ pEStrmPid ].consumer ) {
		throw InternalError( "No consumer to detach!" );
	}

	tbl[ pEStrmPid ].consumer.kill();

}

Bool M2S::TSPacketDispatcher::hasPeerConsumer(
	const	Uint32		pEStrmPid ) const {

	checkPid( pEStrmPid );

	return ( Bool )tbl[ pEStrmPid ].consumer;

}

// ============================================================================

void M2S::TSPacketDispatcher::attachPCRConsumer(
	const	Uint32		pEStrmPid,
		GPC::ConsumerPtr
				pConsumer ) {

	checkPid( pEStrmPid );

	if ( tbl[ pEStrmPid ].pcr ) {
		throw InternalError( "Already got a PCR consumer!" );
	}

	tbl[ pEStrmPid ].pcr = pConsumer;

}

void M2S::TSPacketDispatcher::detachPCRConsumer(
	const	Uint32		pEStrmPid ) {

	checkPid( pEStrmPid );

	if ( ! tbl[ pEStrmPid ].pcr ) {
		throw InternalError( "No consumer to detach!" );
	}

	tbl[ pEStrmPid ].pcr.kill();

}

Bool M2S::TSPacketDispatcher::hasPCRConsumer(
	const	Uint32		pEStrmPid ) const {

	checkPid( pEStrmPid );

	return ( Bool )tbl[ pEStrmPid ].pcr;

}

// ============================================================================

void M2S::TSPacketDispatcher::attachFallback(
		GPC::ConsumerPtr
				pFallback ) {

	if ( fallback ) {
		throw InternalError( "Already got a fallback consumer!" );
	}

	fallback = pFallback;

}

void M2S::TSPacketDispatcher::detachFallback() {

	if ( ! fallback ) {
		throw InternalError( "No consumer to detach!" );
	}

	fallback.kill();

}

Bool M2S::TSPacketDispatcher::hasFallback() const {

	return ( Bool )fallback;

}

// ============================================================================

void M2S::TSPacketDispatcher::attachPassThru(
		GPC::ConsumerPtr
				pPassthru ) {

	if ( passthru ) {
		throw InternalError( "Already got a passthru consumer!" );
	}

	passthru = pPassthru;

}

void M2S::TSPacketDispatcher::detachPassThru() {

	if ( ! passthru ) {
		throw InternalError( "No consumer to detach!" );
	}

	passthru.kill();

}

Bool M2S::TSPacketDispatcher::hasPassThru() const {

	return ( Bool )passthru;

}

// ============================================================================

void M2S::TSPacketDispatcher::putObject(
		SPtr		pTSPacket ) {

	TSPacketPtr	tsPacket = pTSPacket;

	pcktCntr++;

	// transport_error_indicator ?...

	if ( tsPacket->getTransportErrorIndicator() ) {
		trErrCnt++;
//		msgWrn( "putObject(): transport_error_indicator bypassed!" );
		return;
	}
	else if ( trErrCnt ) {
		msgWrn( "putObject(): skipped " + Buffer( trErrCnt )
			+ " corrupted packets!" );
		trErrCnt = 0;
	}

	Uint32		pid = tsPacket->getPID();

	tbl[ pid ].received++;

	if ( ! fallback
	  && ! passthru
	  && ! tbl[ pid ].consumer
	  && ! tbl[ pid ].pcr ) {
		dropCntr++;
		return;
	}

//	// overall sanity checks... (and other)
//
//	if ( tsPacket->hasAdaptationField()
//	  && tsPacket->getAdaptationField().transport_private_data_flag ) {
//		msgDbg( "Found private data in AF on PID["
//			+ Buffer( pid, Buffer::Base16, 4 )
//			+ "]: "
//			+ Utils::toPrintable( tsPacket->getAdaptationField().private_data_byte ) );
//	}

	// continuity_counter OK ?...

	if ( pid != 0x1FFF ) {
		Uint32	curCC = tbl[ pid ].currCC;
		Uint32	gotCC = tsPacket->getContinuityCounter();
		if ( curCC != InvalidCC ) {
			Uint32	expCC = curCC;
			if ( tsPacket->hasPayload() ) {
				expCC++;
				expCC &= 0x0F;
			}
			if ( expCC != gotCC
			  && ( ! tsPacket->hasAdaptationField() || ! tsPacket->getAdaptationField().discontinuity_indicator ) ) {
				msgWrn( "putObject(): Discontinuity on PID["
					+ Buffer( pid, Buffer::Base16, 4 )
					+ "]: exp: "
					+ Buffer( expCC, Buffer::Base16, 2 )
					+ ", got: "
					+ Buffer( gotCC, Buffer::Base16, 2 ) );
			}
		}
		tbl[ pid ].currCC = gotCC;
	}

	// Dispatch!

	// Send to passthru asap...

	if ( passthru ) {
		passthru->putObject( tsPacket );
	}

	if ( tbl[ pid ].pcr && tsPacket->hasAdaptationField() && tsPacket->getAdaptationField().PCR_flag ) {
		tbl[ pid ].pcr->putObject( tsPacket->getAdaptationField().PCR );
	}

	if ( tbl[ pid ].consumer ) {
		tbl[ pid ].consumer->putObject( tsPacket );
		tbl[ pid ].consumed++;
	}
	else if ( fallback ) {
		fallback->putObject( tsPacket );
		tbl[ pid ].consumed++;
	}

}

// ============================================================================

void M2S::TSPacketDispatcher::dumpStatus() const {

	msgDbg( "Packets consumed: " + Buffer( pcktCntr ) + " packets." );
	msgDbg( "Packets dropped : " + Buffer( dropCntr ) + " packets." );

	Uint32 active = 0;

	for ( Uint32 i = 0 ; i < 0x2000 ; i++ ) {
		if ( tbl[ i ].received ) {
			if ( ! tbl[ i ].consumer ) {
				msgWrn( "No consumer for PID "
					+ Buffer( i, Buffer::Base16, 4 )
					+ " (" + Buffer( i ) + ")" );
			}
			active++;
//			msgDbg( "PID["
//				+ Buffer( i, Buffer::Base16, 4 )
//				+ "]: received: "
//				+ Buffer( tbl[ i ].received, Buffer::Base10, 8 )
//				+ " / handled: "
//				+ Buffer( tbl[ i ].consumed, Buffer::Base10, 8 ) );
		}
	}

	Uint64 stuffing = tbl[ 0x1FFF ].received;

	// Windows cannot convert Uint64 ---> Double, hence the intermediate
	// (Int64) conversion below...

	if ( pcktCntr ) {
		msgDbg( "Stuffing packets: " + Buffer( stuffing )
			+ "/" + Buffer( pcktCntr )
			+ " ---> "
			+ ( (Double)(Int64)stuffing * 100.0 / (Double)(Int64)pcktCntr )
			+ " %" );
	}

	if ( active ) {
		msgDbg( "Active PIDs     : " + Buffer( active ) );
	}
	else {
		msgDbg( "No active PIDs!!!" );
	}

}

// ============================================================================

void M2S::TSPacketDispatcher::checkPid(
	const	Uint32		pEStrmPid ) const {

	if ( pEStrmPid >= 0x2000 ) {
		throw InvalidArgument( "PID should be <= 0x1FFF, but == "
			+ Buffer( pEStrmPid, Buffer::Base16, 4 ) );
	}

}

// ============================================================================

