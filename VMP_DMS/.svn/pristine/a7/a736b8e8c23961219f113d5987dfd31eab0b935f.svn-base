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
//	BFC.Msg.EventRepository.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.Msg.EventRepository.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Msg, EventRepository )

// ============================================================================

Msg::EventRepository::EventRepository() {

}

// ============================================================================

Msg::EventRepositoryPtr Msg::EventRepository::instance() {

	static EventRepositoryPtr i = new EventRepository;

	return i;

}

// ============================================================================

Msg::EventPtr Msg::EventRepository::getNewEvent(
	const	UUId &		pUUId ) {

	FastCSLocker locker( cs );

	return map[ pUUId ]->getNew();

}

// ============================================================================

void Msg::EventRepository::addFactory(
	const	UUId &		pUUId,
		FactoryPtr	pFactory ) {

	FastCSLocker locker( cs );

	map.add( pUUId, pFactory );

}

void Msg::EventRepository::delFactory(
	const	UUId &		pUUId ) {

	FastCSLocker locker( cs );

	map.del( pUUId );

}

// ============================================================================

