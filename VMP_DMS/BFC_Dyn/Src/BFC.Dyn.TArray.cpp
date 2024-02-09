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
//	BFC.Dyn.TArray.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.TArray.h"
#include "BFC.Dyn.VArray.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, TArray )

// ============================================================================

Dyn::TArray::TArray() :

	Type	( /* TypeArray */ ) {

}

Dyn::TArray::TArray(
		TypePtr		pSubType ) :

	Type	( /* TypeArray */ ),
	sub	( pSubType ) {

}

Dyn::TArray::TArray(
	const	TArray &	pOther ) :

	SObject	(),
	Cloneable(),
	Type	( pOther ),
	sub	( pOther.sub ) {

}

Dyn::TArray & Dyn::TArray::operator = (
	const	TArray &	pOther ) {

	if ( this != &pOther ) {
		Type::operator = ( pOther );
		sub = pOther.sub;
	}

	return *this;

}

// ============================================================================

SPtr Dyn::TArray::clone() const {

	return new TArray( *this );

}

Dyn::VarPtr Dyn::TArray::create() {

	return new VArray( this );

}

// ============================================================================

void Dyn::TArray::setLocked() {

	sub->setLocked();

	Type::setLocked();

}

// ============================================================================

Dyn::TypePtr Dyn::TArray::getSubType() const {

	return sub;

}

void Dyn::TArray::setSubType(
		TypePtr		pSubType ) {

	crashIfLocked();

	sub = pSubType;

}

// ============================================================================

