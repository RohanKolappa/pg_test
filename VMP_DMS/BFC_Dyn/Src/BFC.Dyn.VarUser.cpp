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
//	BFC.Dyn.VarUser.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.List.h"
#include "BFC.Dyn.VarSerializer.h"
#include "BFC.Dyn.VarUser.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VarUser )

// ============================================================================

Dyn::VarUser::VarUser() :

	list	( new List ) {

}

Dyn::VarUser::VarUser(
	const	VarUser &	pOther ) :

	SObject	(),
	DObject	(),
	list	( pOther.list->clone() ) {

}

Dyn::VarUser & Dyn::VarUser::operator = (
	const	VarUser &	pOther ) {

	if ( this != &pOther ) {
		list = pOther.list->clone();
	}

	return *this;

}

// ============================================================================

Dyn::VarPtr Dyn::VarUser::getDynVar(
	const	Buffer &	pName ) {

	return list->get( pName );

}

Dyn::VarCPtr Dyn::VarUser::getDynVar(
	const	Buffer &	pName ) const {

	return list->get( pName );

}

Buffer Dyn::VarUser::getDynVarValue(
	const	Buffer &	pName ) const {

	return list->get( pName )->toBuffer();

}

void Dyn::VarUser::setDynVarValue(
	const	Buffer &	pName,
	const	Buffer &	pValue ) {

	list->get( pName )->fromBuffer( pValue );

}

Dyn::VarCArray Dyn::VarUser::getDynVars() const {

	return ( list->getAll() );

}

// ============================================================================

void Dyn::VarUser::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	VarCArray a = getDynVars();

	pOStream->putLEUint32( a.getSize() );

	for ( Uint32 i = 0 ; i < a.getSize() ; i++ ) {
		VarSerializer::doSerialize( pOStream, a[ i ] );
	}

}

void Dyn::VarUser::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	VarArray a;

	a.resize( pIStream->getLEUint32() );

	for ( Uint32 i = 0 ; i < a.getSize() ; i++ ) {
		a[ i ] = VarSerializer::unSerialize( pIStream );
	}

	list->setAll( a );

}

// ============================================================================

void Dyn::VarUser::attachDynVarList(
		VarUserPtr	pOther ) {

	list = pOther->list;

}

void Dyn::VarUser::addDynVar(
		VarPtr		pDynVar ) {

	list->add( pDynVar );

}

void Dyn::VarUser::delDynVar(
		VarPtr		pDynVar ) {

	list->del( pDynVar );

}

void Dyn::VarUser::delAllDynVars() {

	list->kill();

}

// ============================================================================

