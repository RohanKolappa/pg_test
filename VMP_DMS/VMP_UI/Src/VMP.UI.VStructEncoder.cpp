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
//	VMP.UI.VStructEncoder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.TBool.h"
#include "BFC.Dyn.TBuffer.h"
#include "BFC.Dyn.TEnum.h"
#include "BFC.Dyn.TInt.h"
#include "BFC.Dyn.TSignal.h"

#include "BFC.Msg.Funcs.h"

#include "VMP.UI.ICheckBox.h"
#include "VMP.UI.IComboBox.h"
#include "VMP.UI.IHiddenFrame.h"
#include "VMP.UI.ILCDNumber.h"
#include "VMP.UI.ILineEdit.h"
#include "VMP.UI.IProgressBar.h"
#include "VMP.UI.IPushButton.h"
#include "VMP.UI.IRadioButtons.h"
#include "VMP.UI.ISlider.h"
#include "VMP.UI.ISpacer.h"
#include "VMP.UI.ISpinBox.h"
#include "VMP.UI.ITextEdit.h"
#include "VMP.UI.ITextLabel.h"
#include "VMP.UI.ITitledFrame.h"

#include "VMP.UI.VStructEncoder.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, VStructEncoder )

// ============================================================================

UI::VStructEncoder::VStructEncoder() {

}

UI::VStructEncoder::~VStructEncoder() {

}

// ============================================================================

Dyn::VStructPtr UI::VStructEncoder::encode(
		IElementPtr	pRoot ) {

	Dyn::TStructPtr	t = encodeType( pRoot );
	Dyn::VStructPtr	v = t->create();

	syncVar( pRoot, v );

	return v;

}

// ============================================================================

Dyn::TypePtr UI::VStructEncoder::encodeType(
		IElementPtr	pElt ) {

	Dyn::TypePtr	type;

	if ( pElt.isA< UI::ICheckBox >() ) {
		type = Dyn::TBool::instance();
	}
//	else if ( pElt.isA< UI::ICheckBoxes >() ) {
//	}
	else if ( pElt.isA< UI::IComboBox >() ) {
		type = new Dyn::TEnum;
		type->setName( "enum" );
	}
	else if ( pElt.isA< UI::IHiddenFrame >()
	  || pElt.isA< UI::ITitledFrame >() ) {

		Dyn::TStructPtr tmp = new Dyn::TStruct;
		tmp->setName( "struct" );
		for ( Uint32 i = 0 ; i < pElt->getNbrChildren() ; i++ ) {
			IElementPtr sub = pElt->getChildByIndex( i );
			Dyn::TypePtr t = encodeType( sub );
			if ( t ) {
				tmp->addMember( sub->getElementName(), t );
			}
		}
		type = tmp;

	}
	else if ( pElt.isA< UI::ILCDNumber >() ) {
		type = Dyn::TInt::instance();
	}
	else if ( pElt.isA< UI::ILineEdit >() ) {
		type = Dyn::TBuffer::instance();
	}
//	else if ( pElt.isA< UI::IListView >() ) {
//	}
//	else if ( pElt.isA< UI::IPictView >() ) {
//	}
	else if ( pElt.isA< UI::IProgressBar >() ) {
		type = Dyn::TInt::instance();
	}
	else if ( pElt.isA< UI::IPushButton >() ) {
		type = Dyn::TSignal::instance();
	}
	else if ( pElt.isA< UI::IRadioButtons >() ) {
		type = new Dyn::TEnum;
		type->setName( "enum" );
	}
//	else if ( pElt.isA< UI::IScrolledFrame >() ) {
//	}
	else if ( pElt.isA< UI::ISlider >() ) {
		type = Dyn::TInt::instance();
	}
	else if ( pElt.isA< UI::ISpinBox >() ) {
		type = Dyn::TInt::instance();
	}
//	else if ( pElt.isA< UI::IStackedFrame >() ) {
//	}
//	else if ( pElt.isA< UI::IStackedItem >() ) {
//	}
	else if ( pElt.isA< UI::ITextEdit >() ) {
		type = Dyn::TBuffer::instance();
	}

	else if ( pElt.isA< UI::ISpacer >() ) {
	}
	else if ( pElt.isA< UI::ITextLabel >() ) {
	}

	else {
		throw InternalError( "No strategy for: " + Buffer( pElt.typeName() ) );
	}

//	Buffer name;
//
//	if ( type ) {
//		name = type->getName();
//	}
//
//	msgDbg( "encodeType(): " + pElt->getObjectName()
//		+ "\n--> " + name );

	return type;

}

// ============================================================================

void UI::VStructEncoder::syncVar(
		IElementPtr	pElt,
		Dyn::VarPtr	pVar ) {

	Dyn::TypePtr	type;

	if ( pElt.isA< UI::ICheckBox >()
	  || pElt.isA< UI::IComboBox >()
	  || pElt.isA< UI::ILCDNumber >()
	  || pElt.isA< UI::ILineEdit >()
	  || pElt.isA< UI::IProgressBar >()
	  || pElt.isA< UI::IPushButton >()
	  || pElt.isA< UI::IRadioButtons >()
	  || pElt.isA< UI::ISlider >()
	  || pElt.isA< UI::ISpinBox >()
	  || pElt.isA< UI::ITextEdit >() ) {
		Msg::sync( pElt, pVar );
	}
	else if ( pElt.isA< UI::IHiddenFrame >()
	       || pElt.isA< UI::ITitledFrame >() ) {
		Dyn::TStructCPtr t = pVar->getType();
		for ( Uint32 i = 0 ; i < t->getNbrMembers() ; i++ ) {
			Buffer n = t->getMemberName( i );
			syncVar( pElt->getChildByName( n ), pVar->sub( n ) );
		}

	}

}

// ============================================================================

