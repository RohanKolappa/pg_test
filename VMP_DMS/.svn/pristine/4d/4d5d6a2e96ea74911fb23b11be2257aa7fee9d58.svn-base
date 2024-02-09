// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "JBS".
//
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	JBS.Atoms.ListAtom.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "JBS.Atoms.ListAtom.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, ListAtom )

// ============================================================================

Atoms::ListAtom::ListAtom() {

	setObjectName( "Atom[ <list> ]" );

}

Atoms::ListAtom::~ListAtom() {



}

// ============================================================================

Time::Clock Atoms::ListAtom::getBuildTime() const {

	Time::Clock res;
	AtomList::CstIterator it;

	for ( it = getExtDeps().firstElt() ; it ; it++ ) {
		Time::Clock tmp = (*it)->getBuildTime();
		if ( tmp > res ) {
			res = tmp;
		}
	}

	for ( it = getIntDeps().firstElt() ; it ; it++ ) {
		Time::Clock tmp = (*it)->getBuildTime();
		if ( tmp > res ) {
			res = tmp;
		}
	}

	return res;

}

// ============================================================================

void Atoms::ListAtom::doUpdate() {

	// Bring all external dependencies up to date...

	AtomList::CstIterator it;

	for ( it = getExtDeps().firstElt() ; it ; it++ ) {
		(*it)->doUpdate();
	}

	// Bring all internal dependencies up to date...

	for ( it = getIntDeps().firstElt() ; it ; it++ ) {
		(*it)->doUpdate();
	}

}

void Atoms::ListAtom::doClean(
	const	Bool	distClean ) {

	// Clean all internal dependencies...

	for ( AtomList::CstIterator it = getIntDeps().firstElt() ; it ; it++ ) {
		(*it)->doClean( distClean );
	}

}

// ============================================================================

