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
//	VMP.L2.Sink.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.Locker.h"

#include "VMP.L2.Sink.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::L2::Sink
// ============================================================================

BFC_PTR_IMPL_NS( L2, Sink )

// ============================================================================

L2::Sink::Sink(
	const	Uint32		pMaxSize ) {

	fifo = new Fifo( pMaxSize );

	iEngine = new IEngine( fifo );
	oEngine = new OEngine( fifo );

}

// ============================================================================
// VMP::L2::Sink::Fifo
// ============================================================================

BFC_PTR_IMPL_NS_CC( L2, Sink, Fifo )

// ============================================================================

L2::Sink::Fifo::Fifo(
	const	Uint32		pMaxSize ) :

	cond	( mutx ),
	stat	( NoneInSession ),
	fifo	( pMaxSize ) {

}

// ============================================================================

void L2::Sink::Fifo::putSessionCallback(
		SessionPtr	pSession ) {

	MT::Locker locker( mutx );

//	msgDbg( "putSessionCallback()" );

	for (;;) {

		switch ( stat ) {

		case NoneInSession :
			stat = LeftInSession;
			sess = pSession;
			cond.signal();
			return;

		case LeftInSession :
		case BothInSession :
		case RightOutOfSession :
			throw AlreadyInSession();

		case LeftOutOfSession :
			cond.wait();
			break;

		}

	}

}

SessionPtr L2::Sink::Fifo::getSessionCallback() {

	MT::Locker locker( mutx );

//	msgDbg( "getSessionCallback()" );

	for (;;) {

		switch ( stat ) {

		case LeftInSession : {
			SessionPtr res = sess;
			stat = BothInSession;
			sess.kill();
			cond.signal();
			return res;
			}

		case BothInSession :
		case LeftOutOfSession :
			throw AlreadyInSession();

		case NoneInSession :
		case RightOutOfSession :
			cond.wait();
			break;

		}

	}

}

void L2::Sink::Fifo::leftEndSessionCallback() {

	MT::Locker locker( mutx );

	switch ( stat ) {

	case NoneInSession :
	case LeftOutOfSession :
		throw NotInSession();

	case LeftInSession :
		stat = NoneInSession;
		sess.kill();
		break;

	case BothInSession :
		stat = LeftOutOfSession;
		break;

	case RightOutOfSession :
		stat = NoneInSession;
		break;

	}

	cond.signal();

}

void L2::Sink::Fifo::rightEndSessionCallback() {

	MT::Locker locker( mutx );

	switch ( stat ) {

	case NoneInSession :
	case RightOutOfSession :
	case LeftInSession :
		throw NotInSession();

	case BothInSession :
		stat = RightOutOfSession;
		break;

	case LeftOutOfSession :
		stat = NoneInSession;
		break;

	}

	fifo.kill();
	cond.signal();

}

void L2::Sink::Fifo::putFrameCallback(
		FramePtr	data ) {

	MT::Locker locker( mutx );

//	msgDbg("putFrameCallback(): index == " + Buffer(data->getOffset()));

	for (;;) {

		switch ( stat ) {

		case NoneInSession :
		case LeftOutOfSession :
			throw NotInSession();

		case RightOutOfSession :
			stat = NoneInSession;
			cond.signal();
			throw BrokenSession();

		case BothInSession :
			if ( ! fifo.isFull() ) {
				fifo.push( data );
				cond.signal();
//				msgDbg( "putFrameCallback(): OK" );
				return;
			}
//			msgDbg( "Blocked..." );

		case LeftInSession :
//			msgDbg( "putFrameCallback(): wait..." );
			cond.wait();
			break;

		}

	}

}

FramePtr L2::Sink::Fifo::getFrameCallback(
	const	Uint64		/* index */ ) {

	MT::Locker locker( mutx );

//	msgDbg( "getFrameCallback(): index == " + Buffer( index ) );

	Bool done = false;

	while ( ! done ) {

		switch ( stat ) {

		case NoneInSession :
		case LeftInSession :
		case RightOutOfSession :
			throw NotInSession();

		case BothInSession :
			if ( ! fifo.isEmpty() ) {
				done = true;
				break;
			}
//			msgDbg( "Blocked..." );
			cond.wait();
			break;

		case LeftOutOfSession :
			if ( ! fifo.isEmpty() ) {
				done = true;
				break;
			}
			stat = NoneInSession;
			cond.signal();
			throw BrokenSession();

		}

	}

	FramePtr res = fifo.top();

	fifo.pop();

//	msgDbg( "getFrameCallback(): index == " + Buffer( index )
//		+ ", returned == " + Buffer( res->getOffset() ) );

	cond.signal();

	return res;

}
	
// ============================================================================
// VMP::L2::Sink::IEngine
// ============================================================================

BFC_PTR_IMPL_NS_CC( L2, Sink, IEngine )

// ============================================================================

L2::Sink::IEngine::IEngine(
		FifoPtr		pFifo ) :

	PushEngine	( getClassType() ),
	fifo		( pFifo ) {

}

// ============================================================================

void L2::Sink::IEngine::putSessionCallback(
		SessionPtr	pSession ) {

	fifo->putSessionCallback( pSession );

}

void L2::Sink::IEngine::endSessionCallback() {

	fifo->leftEndSessionCallback();

}

void L2::Sink::IEngine::putFrameCallback(
		FramePtr	pFrame ) {

	fifo->putFrameCallback( pFrame );

}

// ============================================================================
// VMP::L2::Sink::OEngine
// ============================================================================

BFC_PTR_IMPL_NS_CC( L2, Sink, OEngine )

// ============================================================================

L2::Sink::OEngine::OEngine(
		FifoPtr		pFifo ) :

	PullEngine	( getClassType() ),
	fifo		( pFifo ) {

}

// ============================================================================

SessionPtr L2::Sink::OEngine::getSessionCallback() {

	return fifo->getSessionCallback();

}

void L2::Sink::OEngine::endSessionCallback() {

	fifo->rightEndSessionCallback();

}

FramePtr L2::Sink::OEngine::getFrameCallback(
	const	Uint64		pIndex ) {

	return fifo->getFrameCallback( pIndex );

}

// ============================================================================

