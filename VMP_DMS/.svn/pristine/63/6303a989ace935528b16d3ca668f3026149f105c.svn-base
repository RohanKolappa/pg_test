// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.UObject.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.DL.Type.h"

#include "VMP.UI.UObject.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, UObject )

// ============================================================================

const DL::TypeCPtr & UI::UObject::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "d986c553-ed81-48ca-b239-59d4cb58ae5f" ),
		"VMP.UI.UObject",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

void UI::UObject::setUserInterface(
		UI::IElementPtr		pUserInterface ) {

//	msgWrn( "setUserInterface(): attaching..." );

	ui = pUserInterface;

	if ( ui ) {
//		msgWrn( "setUserInterface(): initializing..." );
		initUICallback( ui );
	}

}

Bool UI::UObject::hasUserInterface() const {

	return ( Bool )ui;

}

UI::IElementPtr UI::UObject::getUserInterface() const {

	return ui;

}

// ============================================================================

void UI::UObject::initUICallback(
		UI::IElementPtr		/* pUserInterface */ ) {

	msgWrn( "initUICallback(): ignoring UI!!!" );

}

// ============================================================================

