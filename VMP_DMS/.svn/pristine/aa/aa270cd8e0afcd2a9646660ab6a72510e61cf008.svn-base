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
//	JBS.Atoms.VuiAtom.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.Atoms.VuiAtom.h"
#include "JBS.Context.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, VuiAtom )
BFC_PTR_IMPL_NS( Atoms, Ui2VuiAtom )

// ============================================================================

Atoms::VuiAtom::VuiAtom() {



}

Atoms::VuiAtom::~VuiAtom() {



}

// ============================================================================

const Buffer Atoms::VuiAtom::fileExt( ".vui" );

// ============================================================================

Atoms::UserVuiAtom::UserVuiAtom(
		ProjectPtr	proj,
	const	BFC::FSPath&	path )
	: UserAtom( proj, path, fileExt ) {



}

Atoms::UserVuiAtom::~UserVuiAtom() {



}

// ============================================================================

Atoms::Ui2VuiAtom::Ui2VuiAtom(
		AtomPtr		__uiFile )
	: TempAtom( __uiFile, fileExt ) {

	uiAtom = __uiFile;

}

Atoms::Ui2VuiAtom::~Ui2VuiAtom() {



}

// ============================================================================

void Atoms::Ui2VuiAtom::doBuildLocalTarget() {

	Message( Message::Ui_vui, uiAtom->getAtomPath() );
	context->qui2vui->generate( this );

}

// ============================================================================

