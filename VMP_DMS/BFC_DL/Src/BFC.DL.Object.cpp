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
//	BFC.DL.Object.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.UUId.h"

#include "BFC.DL.Counter.h"
#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DL, Object )

// ============================================================================

const DL::TypeCPtr & DL::Object::getClassType() {

	static TypeCPtr i = new Type(
		UUId( "1b8f0c3e-2cde-4dd5-be9e-9e89a9024eee" ),
		"BFC.DL.Object",
		TypeCPtr() );

	return i;

}

// ============================================================================

DL::Object::Object(
	const	TypeCPtr &	pType ) :

	DObject	( pType->getName() ),
	type	( pType ) {

	setObjectName( pType->getName() );

//	msgDbg( "BFC::DL::Object::Object(): \"" + pType->getName() + "\"" );

}

DL::Object::Object(
	const	Object &	pOther ) :

	SObject	( pOther ),
	DObject	( pOther.type->getName() ),
	type	( pOther.type ) {

	setObjectName( pOther.type->getName() );

}

DL::Object::~Object() {

	if ( cntr ) {
		cntr->dec();
	}

}

// ============================================================================

Bool DL::Object::inherits(
	const	TypeCPtr &	pClassType ) const {

	return ( type->inherits( pClassType ) );

}

void DL::Object::setType(
	const	TypeCPtr &	pClassType ) {

	type = pClassType;
	setObjectName( pClassType->getName() );

}

// ============================================================================

void DL::Object::setCounter(
	const	CounterPtr &	pCounter ) {

	if ( cntr ) {
		cntr->dec();
	}

	cntr = pCounter;

	if ( cntr ) {
		cntr->inc();
	}

}

// ============================================================================

