// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.TL.ListBase.cpp
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

#include "BFC.TL.ListBase.h"
#include "BFC.TL.ListCell.h"

// ============================================================================

using namespace BFC;

// ============================================================================

ListBase::ListBase()

	: id   ( globalId++ )
	, first( 0 )
	, last ( 0 )
	, nbr  ( 0 ) {



}

ListBase::~ListBase() {



}

// ============================================================================

ListCell * ListBase::privGetNew(
		void		* data ) {

	return new ListCell( id, data );

}

void ListBase::privDelete(
		ListCell	* cell ) {

	if ( ! cell || cell->list != id ) {
		throw InternalError( "ListBase::privDelete" );
	}

	if ( cell->data ) {
		throw InternalError( "ListBase::privDelete: got data!" );
	}

	if ( cell->prev || cell->next ) {
		throw InternalError( "ListBase::privDelete: got siblings!" );
	}

	delete cell;

}

// ============================================================================

ListCell * ListBase::privPop(
		ListCell	* cell ) {

	if ( ! cell || cell->list != id ) {
		throw InternalError( "ListBase::privPop()" );
	}

	FastCSLocker locker( fastCS );

	if ( cell->prev ) {
		cell->prev->next = cell->next;
	}
	else {
		first = cell->next;
	}

	if ( cell->next ) {
		cell->next->prev = cell->prev;
	}
	else {
		last = cell->prev;
	}

	ListCell * res = cell->next;

	cell->prev = cell->next = 0;

	nbr--;

	return res;

}

// ============================================================================

void ListBase::privPushBefore(
		ListCell	* curCell,
		ListCell	* newCell ) {

	if ( ! newCell || newCell->list != id ) {
		throw InternalError( "ListBase::privPushBefore(): invalid id" );
	}

	FastCSLocker locker( fastCS );

	if ( curCell ) {
		newCell->next = curCell;
		newCell->prev = curCell->prev;
		curCell->prev = newCell;
	}
	else if ( nbr ) {
		throw InternalError( "ListBase::privPushBefore(): null iterator" );
	}
	else {
		last = newCell;
	}

	if ( newCell->prev ) {
		newCell->prev->next = newCell;
	}
	else {
		first = newCell;
	}

	nbr++;

}

// ============================================================================

void ListBase::privPushAfter(
		ListCell	* curCell,
		ListCell	* newCell ) {

	if ( ! newCell || newCell->list != id ) {
		throw InternalError( "ListBase::privPushAfter()" );
	}

	FastCSLocker locker( fastCS );

	if ( curCell ) {
		newCell->prev = curCell;
		newCell->next = curCell->next;
		curCell->next = newCell;
	}
	else if ( nbr ) {
		throw InternalError( "ListBase::privPushAfter(): null iterator" );
	}
	else {
		first = newCell;
	}

	if ( newCell->next ) {
		newCell->next->prev = newCell;
	}
	else {
		last = newCell;
	}

	nbr++;

}

// ============================================================================

Uint32 ListBase::globalId = 0;

// ============================================================================

