// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Msg".
// 
// "BFC::Msg" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Msg" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Msg"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Msg.Dispatcher.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Msg.Dispatcher.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Msg, Dispatcher )

// ============================================================================

Msg::Dispatcher::Dispatcher() :

	prodConsId	( 0 ) {

}

// ============================================================================

Msg::DispatcherPtr Msg::Dispatcher::instance() {

	static DispatcherPtr i = new Dispatcher;

	return i;

}

// ============================================================================

void Msg::Dispatcher::connect(
		ProducerPtr	producer,
	const	Uint32		prodServId,
		ConsumerPtr	consumer,
	const	Uint32		consServId ) {

	producer.setMaster( false );
	consumer.setMaster( false );

	const	Uint32	prodUniqId = producer->getProdId();
	const	Uint32	consUniqId = consumer->getConsId();

	assocTable += AssocTableEntry(
			prodUniqId,
			prodServId,
			consUniqId,
			consServId );

}

// ============================================================================

Uint32 Msg::Dispatcher::addProducer(
		ProducerPtr	producer ) {

	FastCSLocker	locker( prodConsCS );
	Uint32		res = prodConsId++;

	producer.setMaster( false );
	prodSHash.insert( res, producer );

	return res;

}

Uint32 Msg::Dispatcher::addConsumer(
		ConsumerPtr	consumer ) {

	FastCSLocker	locker( prodConsCS );
	Uint32		res = prodConsId++;

	consumer.setMaster( false );
	consSHash.insert( res, consumer );

	return res;

}

// ============================================================================

void Msg::Dispatcher::delProducer(
	const	Uint32		pProdUniqId ) {

	disconnectProducer( pProdUniqId );

	FastCSLocker locker( prodConsCS );

	prodSHash.remove( pProdUniqId );

}

void Msg::Dispatcher::delConsumer(
	const	Uint32		pConsUniqId ) {

	disconnectConsumer( pConsUniqId );

	FastCSLocker locker( prodConsCS );

	consSHash.remove( pConsUniqId );

}

// ============================================================================

void Msg::Dispatcher::disconnectProducer(
	const	Uint32		pProdUniqId ) {

	FastCSLocker locker( assocMutex );
	Uint32 i = 0;
	while ( i < assocTable.size() ) {
		if ( assocTable[ i ].prodUniqId == pProdUniqId ) {
			assocTable.pop( i );
		}
		else {
			i++;
		}
	}

}

void Msg::Dispatcher::disconnectConsumer(
	const	Uint32		pConsUniqId ) {

	{
		FastCSLocker locker( pendingMutex );
		Uint32 i = 0;
		while ( i < pendingTable.size() ) {
			if ( pendingTable[ i ].consUniqId == pConsUniqId ) {
				pendingTable.pop( i );
			}
			else {
				i++;
			}
		}
	}

	{
		FastCSLocker locker( assocMutex );
		Uint32 i = 0;
		while ( i < assocTable.size() ) {
			if ( assocTable[ i ].consUniqId == pConsUniqId ) {
				assocTable.pop( i );
			}
			else {
				i++;
			}
		}
	}

}

// ============================================================================

void Msg::Dispatcher::dispatch(
	const	Uint32		prodUniqId,
	const	Uint32		prodServId,
	const	EventPtr &	e ) {

//	DObject::staticMsgDbg( "dispatch(): " + Buffer( e.typeName() ) );

	PendingTable	tmpTable;

	{
		FastCSLocker locker( assocMutex );
		for ( Uint32 ip = 0 ; ip < assocTable.size() ; ip++ ) {
			if ( assocTable[ip].prodUniqId == prodUniqId
			  && ( prodServId == ( Uint32 )-1
			    || assocTable[ip].prodServId == prodServId ) ) {
				tmpTable += PendingTableEntry(
					assocTable[ ip ].consUniqId,
					assocTable[ ip ].consServId,
					e );
			}
		}
	}

	for ( Uint32 i = 0 ; i < tmpTable.getSize() ; i++ ) {
		try {
			ConsumerPtr consPtr = consSHash[ tmpTable[ 0 ].consUniqId ];
//			DObject::staticMsgDbg( "dispatch(): " + Buffer( e.typeName() )
//				+ " ---> "
//				+ Buffer( consPtr.typeName() ) );
			consPtr->eventHandler(
				tmpTable[ 0 ].producerEv,
				tmpTable[ 0 ].consServId );
		}
		catch ( Exception & ) {
		}
	}

}

// ============================================================================

//void Msg::Dispatcher::processPendingEvents() {
//
//	for (;;) {
//
//		// Extract event to process...
//
//		PendingTableEntry e;
//
//		{
//			FastCSLocker locker( pendingMutex );
//			if ( pendingTable.isEmpty() ) {
//				return;
//			}
//			e = pendingTable[ 0 ];
//			pendingTable.pop( 0 );
//		}
//
//		try {
//			ConsumerPtr consPtr = consSHash[ e.consUniqId ];
//			consPtr->eventHandler(
//				e.producerEv,
//				e.consServId );
//		}
//		catch ( Exception & ) {
//		}
//
//	}
//
//}

// ============================================================================

