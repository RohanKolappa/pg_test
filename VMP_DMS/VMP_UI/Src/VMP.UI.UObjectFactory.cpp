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
//	VMP.UI.UObjectFactory.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.UI.UObjectDescr.h"
#include "VMP.UI.UObjectFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, UObjectFactory )

// ============================================================================

UI::UObjectFactory::UObjectFactory(
		DL::BuilderPtr	pBuilder ) :

	DL::Factory( pBuilder ),
	parser( new XMLParser ) {

}

// ============================================================================

UI::UObjectPtr UI::UObjectFactory::getNewUObject(
		IToolkitPtr	pToolkit,
		IElementPtr	pParent ) {

//	msgDbg( "UObjectFactory::getNewUObject()" );

	UObjectPtr resObj = makeObject();

	if ( pToolkit ) {

		Buffer manifest = UObjectDescrCPtr( getDescr() )->getGUI();

		if ( manifest.hasContent() ) {
			IElementPtr rootUI = parser->parse( manifest, pToolkit, pParent );
//			if ( rootUI ) {
//				msgDbg( "UObjectFactory: created valid user interface!" );
//			}
			resObj->setUserInterface( rootUI );
//			msgDbg( "UObjectFactory: created user interface!" );
			rootUI->setEnabled( false );
			rootUI->resize();
			rootUI->setHidden( false );
		}

	}

	return resObj;

}

// ============================================================================

