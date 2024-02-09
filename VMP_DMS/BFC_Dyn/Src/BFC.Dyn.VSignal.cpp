// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.VSignal.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.Dyn.TSignal.h"
#include "BFC.Dyn.VSignal.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VSignal )

// ============================================================================

Dyn::VSignal::VSignal() :

	Var	( TSignal::instance() ),
	sending	( false ) {

	addCallback( this, &VSignal::triggerCB );

}

Dyn::VSignal::VSignal(
	const	Buffer &	pVarName ) :

	Var	( TSignal::instance(), pVarName ),
	sending	( false ) {

	addCallback( this, &VSignal::triggerCB );

}

Dyn::VSignal::VSignal(
	const	TSignalPtr &	pTSignal ) :

	Var	( pTSignal ),
	sending	( false ) {

	addCallback( this, &VSignal::triggerCB );

}

Dyn::VSignal::VSignal(
	const	VSignal &	/* pOther */ ) :

	SObject		(),
	Cloneable	(),
	Serializable	(),
//	Controllable	(),
	Invokable	(),
	Observable	(),
	Observer	(),
//	IVar		(),
	Var		( TSignal::instance() ),
	sending		( false ) {

	addCallback( this, &VSignal::triggerCB );

}

Dyn::VSignal::~VSignal() {

	delCallbacks();

}

// ============================================================================

Dyn::VSignal & Dyn::VSignal::operator = (
	const	VSignal &	pOther ) {

	Var::operator = ( pOther );

	return *this;

}

// ============================================================================

SPtr Dyn::VSignal::clone() const {

	return new VSignal( *this );

}

// ============================================================================

Buffer Dyn::VSignal::toBuffer() const {

	return Buffer();

}

void Dyn::VSignal::fromBuffer(
	const	Buffer &	/* pBuffer */ ) {

	trigger();

}

// ============================================================================

void Dyn::VSignal::trigger() {

	sendEvent();

}

// ============================================================================

void Dyn::VSignal::triggerCB(
		Msg::EventCPtr	/* pEvent */ ) {

	sendEvent();

}

// ============================================================================

void Dyn::VSignal::sendEvent() {

	{
		FastCSLocker locker( critSec );
		if ( sending ) {
			return;
		}
		sending = true;
	}

	warnObservers( Msg::EventPtr() );

	{
		FastCSLocker locker( critSec );
		sending = false;
	}

}

// ============================================================================

