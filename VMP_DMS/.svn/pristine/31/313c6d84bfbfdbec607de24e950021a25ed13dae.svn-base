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
//	BFC.Msg.EventRepository.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_EventRepository_H_
#define _BFC_Msg_EventRepository_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

BFC_PTR_DECL( BFC_Msg_DLL, EventRepository )

} // namespace Msg
} // namespace BFC

// ============================================================================

#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "BFC.Msg.Event.h"
#include "BFC.Msg.EventFactory.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL EventRepository : virtual public SObject {

protected :

	/// \brief Creates a new EventRepository.

	EventRepository(
	);

public :

	static EventRepositoryPtr instance(
	);

	EventPtr getNewEvent(
		const	UUId &		pUUId
	);

	void addFactory(
		const	UUId &		pUUId,
			FactoryPtr	pFactory
	);

	void delFactory(
		const	UUId &		pUUId
	);

protected :

private :

	FastCS			cs;
	Map< UUId, FactoryPtr >	map;

	/// \brief Forbidden copy constructor.

	EventRepository(
		const	EventRepository &
	);

	/// \brief Forbidden copy operator.

	EventRepository & operator = (
		const	EventRepository &
	);

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_EventRepository_H_

// ============================================================================

