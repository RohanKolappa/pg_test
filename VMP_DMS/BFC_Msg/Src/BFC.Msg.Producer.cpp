// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Msg".
// 
// "BFC::Msg" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Msg" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Msg"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Msg.Producer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Msg.Dispatcher.h"
#include "BFC.Msg.Producer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Msg, Producer )

// ============================================================================

Msg::Producer::Producer() {

	prodId = Dispatcher::instance()->addProducer( this );

}

Msg::Producer::~Producer() {

	Dispatcher::instance()->delProducer( prodId );

}

// ============================================================================

void Msg::Producer::disconnect() {

	Dispatcher::instance()->disconnectProducer( prodId );

}

// ============================================================================

void Msg::Producer::dispatchEvent(
	const	EventPtr &	event,
	const	Uint32		servId ) {

	Dispatcher::instance()->dispatch( prodId, servId, event );

}

// ============================================================================





