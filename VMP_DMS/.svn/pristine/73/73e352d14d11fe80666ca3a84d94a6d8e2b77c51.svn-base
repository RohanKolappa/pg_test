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
//	JBS.Atoms.LibAtom.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "JBS.Atoms.LibAtom.h"
#include "JBS.Context.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, LibAtom )

// ============================================================================

Atoms::LibAtom::LibAtom(
		ProjectPtr	__project,
	const	AtomList&	__objList,
	const	AtomList&	__depList,
	const	Bool		__dbgMode,
	const	Bool		__staMode )
	: TempAtom(
		__project,
		filePrefix + __project->getProjectName()
			+ Buffer( __staMode ? ".sta" : "" )
			+ Buffer( __dbgMode ? ".dbg" : "" ),
		( __staMode ? staticFileExt : sharedFileExt ) ) {

	shortName = __project->getProjectName()
			+ Buffer( __staMode ? ".sta" : "" )
			+ Buffer( __dbgMode ? ".dbg" : "" );

	objList = __objList;
	depList = __depList;
	staMode = __staMode;
	dbgMode = __dbgMode;

	AtomList::Iterator it;

	for ( it = objList.firstElt() ; it ; it++ ) {
		addInternalDep( *it );
	}

	for ( it = depList.firstElt() ; it ; it++ ) {
		addExternalDep( *it );
	}

}

Atoms::LibAtom::~LibAtom() {



}

// ============================================================================

#if defined( PLATFORM_WIN32 )
const Buffer Atoms::LibAtom::staticFileExt( ".lib" );
const Buffer Atoms::LibAtom::sharedFileExt( ".dll" );
const Buffer Atoms::LibAtom::filePrefix( "" );
#else
const Buffer Atoms::LibAtom::staticFileExt( ".a" );
const Buffer Atoms::LibAtom::sharedFileExt( ".so" );
const Buffer Atoms::LibAtom::filePrefix( "lib" );
#endif

// ============================================================================

void Atoms::LibAtom::doBuildLocalTarget() {

	Message( Message::Linking_lib, getAtomPath() );
	context->linker->link( this );

}

// ============================================================================

