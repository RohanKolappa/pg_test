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
//	JBS.Atoms.ObjAtom.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "JBS.Context.h"
#include "JBS.Atoms.ObjAtom.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, ObjAtom )

// ============================================================================

Atoms::ObjAtom::ObjAtom(
		AtomPtr		__cppAtom,
		AtomPtr		__depAtom,
	const	Bool		__dbgMode
#if defined( PLATFORM_WIN32 )
	, const	Bool		__staMode
#endif
	) : TempAtom(
		__cppAtom,
		__cppAtom->getBaseName()
#if defined( PLATFORM_WIN32 )
			+ Buffer( __staMode ? ".sta" : "" )
			+ Buffer( __dbgMode ? ".dbg" : "" ),
#else
			+ Buffer( __dbgMode ? ".dbg" : "" ),
#endif
		fileExt ) {

	addInternalDep( __depAtom );

	cppAtom = __cppAtom;
	depAtom = __depAtom;
#if defined( PLATFORM_WIN32 )
	staMode	= __staMode;
#endif
	dbgMode = __dbgMode;

}

Atoms::ObjAtom::~ObjAtom() {



}

// ============================================================================

#if defined( PLATFORM_WIN32 )
const Buffer Atoms::ObjAtom::fileExt( ".obj" );
#else
const Buffer Atoms::ObjAtom::fileExt( ".o" );
#endif

// ============================================================================

void Atoms::ObjAtom::doBuildLocalTarget() {

	Message( Message::Compiling, cppAtom->getAtomPath() );
	context->compiler->compile( this );

}

// ============================================================================

