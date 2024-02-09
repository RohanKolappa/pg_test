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
//	JBS.Atoms.CppAtom.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.Atoms.CppAtom.h"
#include "JBS.Context.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, CppAtom )
BFC_PTR_IMPL_NS( Atoms, MocCppAtom )
BFC_PTR_IMPL_NS( Atoms, VuiImplCppAtom )
BFC_PTR_IMPL_NS( Atoms, UicImplCppAtom )

// ============================================================================

Atoms::CppAtom::CppAtom() {



}

Atoms::CppAtom::~CppAtom() {



}

// ============================================================================

const Buffer Atoms::CppAtom::fileExt( ".cpp" );

// ============================================================================

Atoms::UserCppAtom::UserCppAtom(
		ProjectPtr	proj,
	const	BFC::FSPath&	path )
	: UserAtom( proj, path, fileExt ) {



}

Atoms::UserCppAtom::~UserCppAtom() {



}

// ============================================================================

Atoms::MocCppAtom::MocCppAtom(
		AtomPtr		hdrFile )
	: TempAtom(
		hdrFile,
		hdrFile->getBaseName() + ".moc",
		fileExt ) {

	hdrAtom = hdrFile;

}

Atoms::MocCppAtom::~MocCppAtom() {



}

// ============================================================================

void Atoms::MocCppAtom::doBuildLocalTarget() {

	Message( Message::Mocifying, hdrAtom->getAtomPath() );
	context->mocifier->mocify( this );

}

// ============================================================================

Atoms::VuiImplCppAtom::VuiImplCppAtom(
		AtomPtr		__vuiAtom,
		AtomPtr		__hdrAtom )
	: TempAtom(
		__vuiAtom,
		__vuiAtom->getBaseName() + VuiAtom::fileExt,
		fileExt ) {

	addInternalDep( __hdrAtom );

	vuiAtom = __vuiAtom;
	hdrAtom = __hdrAtom;

}

Atoms::VuiImplCppAtom::~VuiImplCppAtom() {



}

// ============================================================================

void Atoms::VuiImplCppAtom::doBuildLocalTarget() {

	Message( Message::Vui_imp, vuiAtom->getAtomPath() );
	context->vui2cpp->generateImpl( this );

}

// ============================================================================

Atoms::UicImplCppAtom::UicImplCppAtom(
		AtomPtr		__uiAtom,
		AtomPtr		__hdrAtom )
	: TempAtom(
		__uiAtom,
		fileExt ) {

	addInternalDep( __hdrAtom );

	uiAtom = __uiAtom;
	hdrAtom = __hdrAtom;

}

Atoms::UicImplCppAtom::~UicImplCppAtom() {



}

// ============================================================================

void Atoms::UicImplCppAtom::doBuildLocalTarget() {

	Message( Message::Uicifying, uiAtom->getAtomPath() );
	context->uicifier->uicify( this );

}

// ============================================================================

