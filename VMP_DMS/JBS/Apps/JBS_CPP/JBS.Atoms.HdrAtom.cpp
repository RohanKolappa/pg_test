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
//	JBS.Atoms.HdrAtom.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.Atoms.HdrAtom.h"
#include "JBS.Context.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, HdrAtom )
BFC_PTR_IMPL_NS( Atoms, VuiDeclHdrAtom )
BFC_PTR_IMPL_NS( Atoms, UicDeclHdrAtom )

// ============================================================================

Atoms::HdrAtom::HdrAtom() {



}

Atoms::HdrAtom::~HdrAtom() {



}

// ============================================================================

const Buffer Atoms::HdrAtom::fileExt( ".h" );

// ============================================================================

Atoms::UserHdrAtom::UserHdrAtom(
		ProjectPtr	proj,
	const	FSPath&		path )
	: UserAtom( proj, path, fileExt ) {



}

Atoms::UserHdrAtom::~UserHdrAtom() {



}

// ============================================================================

Atoms::VuiDeclHdrAtom::VuiDeclHdrAtom(
		AtomPtr		__vuiAtom )
	: TempAtom(
		__vuiAtom,
		__vuiAtom->getBaseName() + VuiAtom::fileExt,
		fileExt ) {

	vuiAtom = __vuiAtom;

}

Atoms::VuiDeclHdrAtom::~VuiDeclHdrAtom() {



}

// ============================================================================

void Atoms::VuiDeclHdrAtom::doBuildLocalTarget() {

	Message( Message::Vui_dec, vuiAtom->getAtomPath() );
	context->vui2cpp->generateDecl( this );

}

// ============================================================================

Atoms::UicDeclHdrAtom::UicDeclHdrAtom(
		AtomPtr		__uiFile )
	: TempAtom( __uiFile, fileExt ) {

	uiAtom = __uiFile;

}

Atoms::UicDeclHdrAtom::~UicDeclHdrAtom() {



}

// ============================================================================

void Atoms::UicDeclHdrAtom::doBuildLocalTarget() {

	Message( Message::Uicifying, uiAtom->getAtomPath() );
	context->uicifier->uicify( this );

}

// ============================================================================

