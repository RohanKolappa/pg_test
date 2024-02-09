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
//	BFC.Msg.Observable.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Msg.Observable.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Msg, Observable )

// ============================================================================

void Msg::Observable::addObserver(
		ObserverPtr	pObserver,
	const	Observer::SrvId	pObsSrvId ) {

	pObserver.setMaster( false );

	CallbackPtr newCB = pObserver->getCallback( pObsSrvId );

	callbacks += newCB;

	onObserverAdded( pObserver, pObsSrvId );

	Array< CallbackPtr >	tmpCB = callbacks;
	callbacks.kill();
	callbacks += newCB;
	syncObservers();
	callbacks = tmpCB;

}

void Msg::Observable::delObserver(
		ObserverPtr	pObserver ) {

	pObserver.setMaster( false );

	for ( Uint32 i = 0 ; i < callbacks.getSize() ; ) {
		if ( ! callbacks[ i ]
		  || callbacks[ i ]->getObserver() == pObserver ) {
			callbacks.pop( i );
		}
		else {
			i++;
		}
	}

}

// ============================================================================

void Msg::Observable::warnObservers(
		EventPtr	pEvent ) const {

	if ( pEvent ) {
		pEvent->setSender( getCThis() );
	}

	for ( Uint32 i = 0 ; i < callbacks.getSize() ; i++ ) {
		try {
			callbacks[ i ]->call( pEvent );
		}
		catch ( Exception & e ) {
			msgExc( e, "Exception in Event handler!" );
//			throw;
		}
	}

}

void Msg::Observable::warnObserver(
		ObserverPtr	pObserver,
	const	Observer::SrvId	pObsSrvId,
		EventCPtr	pEvent ) const {

	pObserver.setMaster( false );

	pObserver->getCallback( pObsSrvId )->call( pEvent );

}

// ============================================================================

void Msg::Observable::onObserverAdded(
		ObserverPtr	/* pObserver */,
	const	Observer::SrvId	/* pObsSrvId */ ) {

}

// ============================================================================

