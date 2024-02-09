// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::STL".
// 
// "BFC::STL" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::STL" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::STL"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.STL.SList.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.STL.Functor.h"
#include "BFC.STL.SList.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::STL::SList
// ============================================================================

BFC_PTR_IMPL_NS( STL, SList )

// ============================================================================

STL::SList::SList() :

	nbr( 0 ) {

}

STL::SList::SList(
	const	SList &		o ) :

	SObject(),
	nbr( 0 ) {

	for ( CstIterator it = o.constFirstElt() ; it ; it++ ) {
		insertLast( *it );
	}

}

STL::SList::~SList() {

	kill();

}

// ============================================================================

STL::SList & STL::SList::operator = (
	const	SList &		o ) {

	if ( this != &o ) {
		kill();
		for ( CstIterator it = o.constFirstElt() ; it ; it++ ) {
			insertLast( *it );
		}
	}

	return *this;

}

// ============================================================================

Bool STL::SList::contains(
	const	SPtr &		t ) const {

	for ( CstIterator it = constFirstElt() ; it ; it++ ) {
		if ( *it == t ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

void STL::SList::kill() {

	while ( nbr ) {
		deleteLast();
	}

}

// ============================================================================

void STL::SList::pop(
		Iterator&	it ) {

	CellPtr p = it.ptr;

	if ( p->prev ) {
		p->prev->next = p->next;
	}
	else {
		first = p->next;
	}

	if ( p->next ) {
		it.ptr = p->next;
		p->next->prev = p->prev;
	}
	else {
		it.ptr = p->prev;
		last = p->prev;
	}

	nbr--;

}

// ============================================================================

void STL::SList::insertBefore(
		Iterator&	it,
	const	SPtr&		t ) {

	CellPtr cur = it.ptr;
	CellPtr ptr = new Cell( t );

	ptr->prev = cur->prev;
	ptr->next = cur;

	if ( cur->prev ) {
		cur->prev->next = ptr;
	}
	else {
		first = ptr;
	}

	cur->prev = ptr;

	nbr++;

}

void STL::SList::insertAfter(
		Iterator&	it,
	const	SPtr&		t ) {

	CellPtr cur = it.ptr;
	CellPtr ptr = new Cell(t);

	ptr->prev = cur;
	ptr->next = cur->next;

	if ( cur->next ) {
		cur->next->prev = ptr;
	}
	else {
		last = ptr;
	}

	cur->next = ptr;

	nbr++;

}

// ============================================================================

void STL::SList::insertLast(
	const	SPtr&		t ) {

	CellPtr ptr = new Cell( t );

	ptr->prev = last;

	if ( last ) {
		last->next = ptr;
	}
	else {
		first = ptr;
	}

	last = ptr;

	nbr++;

}

void STL::SList::insertFirst(
	const	SPtr&		t ) {

	CellPtr ptr = new Cell( t );

	ptr->next = first;

	if ( first ) {
		first->prev = ptr;
	}
	else {
		last = ptr;
	}

	first = ptr;

	nbr++;

}

// ============================================================================

void STL::SList::deleteLast() {

	CellPtr p = last;

	last = p->prev;

	if ( last ) {
		last->next.kill();
	}
	else {
		first.kill();
	}

	nbr--;

}

void STL::SList::deleteFirst() {

	CellPtr p = first;

	first = p->next;

	if ( first ) {
		first->prev.kill();
	}
	else {
		last.kill();
	}

	nbr--;

}

// ============================================================================

SPtr STL::SList::getLast() const {

	return last->data;

}

SPtr STL::SList::getFirst() const {

	return first->data;

}

// ============================================================================

void STL::SList::apply(
		FunctorPtr	pFunctor ) {

	for ( Iterator it = firstElt() ; it ; it++ ) {
		pFunctor->process( *it );
	}

}

// ============================================================================
// BFC::STL::SList::Cell
// ============================================================================

BFC_PTR_IMPL_NS_CC( STL, SList, Cell )

// ============================================================================

