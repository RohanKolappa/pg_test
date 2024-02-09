// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::UI".
// 
// "VMP::UI" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::UI" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::UI"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IStackedFrame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.UI.IStackedFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::UI::IStackedFrame
// ============================================================================

BFC_PTR_IMPL_NS( UI, IStackedFrame )

// =======S=====================================================================

UI::IStackedFrame::IStackedFrame() {

	addCallback( this, &IStackedFrame::raiseCB, ServRaiseId );

}

UI::IStackedFrame::~IStackedFrame() {

	delCallbacks();

}

// ============================================================================

void UI::IStackedFrame::raiseItem(
	const	Uint32		pIndex ) {

	getItem( pIndex )->raise();

}

// ============================================================================

Uint32 UI::IStackedFrame::getRaisedItemIndex() const {

	for ( Uint32 i = 0 ; i < getNbrItems() ; i++ ) {
		if ( getItem( i )->isRaised() ) {
			return i;
		}
	}

	return ( Uint32 )-1;

}

UI::IStackedItemPtr UI::IStackedFrame::getRaisedItem() const {

	for ( Uint32 i = 0 ; i < getNbrItems() ; i++ ) {
		IStackedItemPtr t = getItem( i );
		if ( t->isRaised() ) {
			return t;
		}
	}

	return IStackedItemPtr();

}

// ============================================================================

void UI::IStackedFrame::raiseCB(
		Msg::EventCPtr	pEvent ) {

	raiseItem( Msg::ValueEventCPtr( pEvent )->getUint32() );

}

// ============================================================================

void UI::IStackedFrame::cachePropRaised(
	const	Uint32		pIndex ) {

	warnObservers( new Msg::ValueEvent( pIndex ) );

}

// ============================================================================

void UI::IStackedFrame::sendEvent() const {

}

// ============================================================================
// VMP::UI::IStackedItem
// ============================================================================

BFC_PTR_IMPL_NS( UI, IStackedItem )

// ============================================================================

void UI::IStackedItem::sendEvent() const {

}

// ============================================================================

