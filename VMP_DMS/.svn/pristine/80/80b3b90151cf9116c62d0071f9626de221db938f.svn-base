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
//	BFC.Dyn.TEnum.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.TEnum.h"
#include "BFC.Dyn.VEnum.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, TEnum )

// ============================================================================

Dyn::TEnum::TEnum() :

	Type	( /*TypeEnum*/ ) {

//	addCallback( this, &TEnum::setValueCB );

}

Dyn::TEnum::TEnum(
	const	char *		pTable[] ) :

	Type	( /*TypeEnum*/ ),
	t	( pTable ) {

//	addCallback( this, &TEnum::setValueCB );

}

Dyn::TEnum::TEnum(
	const	TEnum &		pOther ) :

	SObject	(),
	Cloneable(),
	Type	( /*TypeEnum*/ ),
	t	( pOther.t ) {

//	addCallback( this, &TEnum::setValueCB );

}

// ============================================================================

SPtr Dyn::TEnum::clone() const {

	return new TEnum( *this );

}

Dyn::VarPtr Dyn::TEnum::create() {

	return new VEnum( this );

}

// ============================================================================

//Buffer Dyn::TEnum::toBuffer() const {
//
//	return Buffer();
//
//}
//
//void Dyn::TEnum::fromBuffer(
//	const	Buffer &	/* pBuffer */ ) {
//
//}

// ============================================================================

//void Dyn::TEnum::setValueCB(
//		Msg::EventCPtr	/* pEvent */ ) {
//
//}

// ============================================================================

void Dyn::TEnum::addEntry(
	const	Buffer &	pBuffer ) {

	t += pBuffer;
//	warnObservers( new Dyn::ItemAppended( pBuffer ) );

}

void Dyn::TEnum::delEntries() {

	t.kill();
//	warnObservers( new Dyn::ItemsDeleted );

}

// ============================================================================

//void Dyn::TEnum::syncObservers() const {
//
//	warnObservers( new Dyn::ItemsDeleted );
//	for ( Uint32 i = 0 ; i < t.getSize() ; i++ ) {
//		warnObservers( new Dyn::ItemAppended( t[ i ] ) );
//	}
//
//}

// ============================================================================

