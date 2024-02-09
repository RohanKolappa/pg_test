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
//	BFC.Msg.ChildEvent.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_ChildEvent_H_
#define _BFC_Msg_ChildEvent_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

BFC_PTR_DECL( BFC_Msg_DLL, ChildEvent )

} // namespace Msg
} // namespace BFC

// ============================================================================

#include "BFC.Msg.Event.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief Represents some "child" event.
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL ChildEvent : public Event {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new ChildEvent.

	ChildEvent(
	) :
		Event	( classUUId() ) {
	}

	/// \brief Creates a new ChildEvent.

	ChildEvent(
		const	Buffer &	pChild,
			EventCPtr	pEvent
	) :
		Event	( classUUId() ),
		child	( pChild ),
		event	( pEvent ) {
	}

	virtual Buffer toBuffer(
	) const;

	const Buffer & getChild(
	) const {
		return child;
	}

	EventCPtr getEvent(
	) const {
		return event;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr	pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr	pIStream
	);

protected :

private :

	Buffer		child;
	EventCPtr	event;

	/// \brief Forbidden copy constructor.

	ChildEvent(
		const	ChildEvent &
	);

	/// \brief Forbidden copy operator.

	ChildEvent & operator = (
		const	ChildEvent &
	);

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_ChildEvent_H_

// ============================================================================

