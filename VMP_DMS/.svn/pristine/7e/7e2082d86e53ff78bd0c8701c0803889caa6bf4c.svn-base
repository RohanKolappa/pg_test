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
//	BFC.Dyn.Type.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Type.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, Type )

// ============================================================================

Dyn::Type::Type() :

	lock	( false ) {

}

Dyn::Type::Type(
	const	Type &		/*pOther*/ ) :

	SObject		(),
	Cloneable	(),
	lock		( false ) {

}

// ============================================================================

Dyn::Type & Dyn::Type::operator = (
	const	Type &		pOther ) {

	if ( this != &pOther ) {
		crashIfLocked();
		uuid = UUId();
	}

	return *this;

}

// ============================================================================

void Dyn::Type::setUUId(
	const	UUId &		pUUId ) {

	crashIfLocked();

	uuid = pUUId;

}

Bool Dyn::Type::hasUUId() const {

	return ( uuid != UUId() );

}

const UUId & Dyn::Type::getUUId() const {

	return uuid;

}

// ============================================================================

void Dyn::Type::setName(
	const	Buffer &	pName ) {

	crashIfLocked();
	
	name = pName;

}

Bool Dyn::Type::hasName() const {

	return ( name.hasContent() );

}

const Buffer & Dyn::Type::getName() const {

	return name;

}

// ============================================================================

void Dyn::Type::setLocked() {

	lock = true;

}

Bool Dyn::Type::isLocked() const {

	return lock;

}

// ============================================================================

void Dyn::Type::crashIfLocked() {

	if ( lock ) {
		throw InternalError( "Type is locked!" );
	}

}

// ============================================================================

