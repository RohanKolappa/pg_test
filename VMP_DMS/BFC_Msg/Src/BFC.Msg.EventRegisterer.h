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
//	BFC.Msg.EventRegisterer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_EventRegisterer_H_
#define _BFC_Msg_EventRegisterer_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Base.UUId.h"

#include "BFC.Msg.EventFactory.h"
#include "BFC.Msg.EventRepository.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Msg

template < typename T >
class EventRegisterer {

public :

	/// \brief Creates a new EventRegisterer.

	EventRegisterer(
	);

	/// \brief Destroys this object.

	virtual ~EventRegisterer(
	);

private :

	EventRepositoryPtr	repo;
	UUId			uuid;
	FactoryPtr		fact;

};

// ============================================================================

template < typename T >
EventRegisterer< T >::EventRegisterer() {

	repo = EventRepository::instance();
	uuid = T::classUUId();
	fact = new FactoryTmpl< T >;
	repo->addFactory( uuid, fact );

}

template < typename T >
EventRegisterer< T >::~EventRegisterer() {

	repo->delFactory( uuid );

}

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_EventRegisterer_H_

// ============================================================================

