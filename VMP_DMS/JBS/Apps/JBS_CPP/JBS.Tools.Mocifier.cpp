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
//	JBS.Tools.Mocifier.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include <stdlib.h>

#include "JBS.Atoms.HdrAtom.h"
#include "JBS.Context.h"
#include "JBS.CppProject.h"
#include "JBS.Tools.Mocifier.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Tools, Mocifier )

// ============================================================================

Tools::Mocifier::Mocifier() {

	setObjectName( "JBS.Tools.Mocifier" );
	setDebug( true );

}

Tools::Mocifier::~Mocifier() {



}

// ============================================================================

void Tools::Mocifier::init() {

#if defined( PLATFORM_WIN32 )

	JBS_MOCIFIER = context->environment->getPath( "JBS-QT-DIR-BIN" ) + "moc.exe";

#else

	JBS_MOCIFIER = Buffer( "moc" );

#endif

}

// ============================================================================

void Tools::Mocifier::mocify(
		Atoms::MocCppAtomPtr	cppAtom ) {

	CppProjectPtr		project = cppAtom->getProject();
	Atoms::HdrAtomPtr	hdrAtom = cppAtom->getHdrAtom();

	Buffer string = JBS_MOCIFIER.toBuffer()
		+ " -o "
		+ cppAtom->getAtomPath().toBuffer()
		+ " "
		+ hdrAtom->getAtomPath().toBuffer();

	Message( Message::MsgDebug, string );

	string += '\0';

	if ( ::system( (const char *)string.getBufferAddr() ) ) {
		throw Failed();
	}

}

// ============================================================================

