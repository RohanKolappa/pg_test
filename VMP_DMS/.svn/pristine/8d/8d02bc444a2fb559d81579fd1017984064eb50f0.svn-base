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
//	BFC.Dyn.TStruct.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.TStruct.h"
#include "BFC.Dyn.VStruct.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, TStruct )

// ============================================================================

Dyn::TStruct::TStruct() {

}

Dyn::TStruct::TStruct(
	const	TStruct &	pOther ) :

	SObject		(),
	Cloneable	(),
	Type		( pOther ),
	members		( pOther.members ) {

}

Dyn::TStruct & Dyn::TStruct::operator = (
	const	TStruct &	pOther ) {

	if ( this != &pOther ) {
		Type::operator = ( pOther );
		members = pOther.members;
	}

	return *this;

}

// ============================================================================

SPtr Dyn::TStruct::clone() const {

	return new TStruct( *this );

}

Dyn::VarPtr Dyn::TStruct::create() {

	return new VStruct( this );

}

// ============================================================================

void Dyn::TStruct::setLocked() {

	for ( Uint32 i = 0 ; i < members.getSize() ; i++ ) {
		members.getValue( i )->setLocked();
	}

	Type::setLocked();

}

// ============================================================================

void Dyn::TStruct::addMember(
	const	Buffer &	pMemberName,
		TypePtr		pMemberType ) {

	crashIfLocked();

	if ( members.contains( pMemberName ) ) {
		throw InvalidArgument( "Duplicate entry name \""
			+ pMemberName + "\"!" );
	}

	members.add( pMemberName, pMemberType );

}

void Dyn::TStruct::setMember(
	const	Buffer &	pMemberName,
		TypePtr		pMemberType ) {

	crashIfLocked();

	if ( ! members.contains( pMemberName ) ) {
		throw InvalidArgument( "No such entry name \""
			+ pMemberName + "\"!" );
	}

	members[ pMemberName ] = pMemberType;

}

// ============================================================================

