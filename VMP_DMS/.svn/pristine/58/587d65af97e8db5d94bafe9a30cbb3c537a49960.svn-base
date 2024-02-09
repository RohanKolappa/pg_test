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
//	BFC.Msg.Event.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_Event_H_
#define _BFC_Msg_Event_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Msg.EventPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.UUId.h"

#include "BFC.IO.Serializable.h"

#include "BFC.Msg.ObservablePtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief Base class of all events.
///
/// The Event class is the base class of all events that get transferred
/// between Producer objects and Consumer objects (by way of the
/// unique Dispatcher instance).
///
/// An Event should be generated whenever the status of some object changes.
/// Event objects are sent by Producer objects to 0, 1 or more Consumer
/// objects.
///
/// Sending an Event is semantically the same as broadcasting a message.
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL Event : virtual public IO::Serializable {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new Event.

	Event(
	) :
		uuid	( classUUId() ) {
	}

protected :

	Event(
		const	UUId &		pUUId
	) :
		uuid	( pUUId ) {
	}

	Event(
		const	UUId &		pUUId,
			ObservableCPtr	pSender
	) :
		uuid	( pUUId ) {
		setSender( pSender );
	}

public :

	const UUId & getUUId(
	) const {
		return uuid;
	}

	virtual Buffer toBuffer(
	) const;

	void setSender(
			ObservableCPtr	pSender
	);

	ObservableCPtr getSender(
	) const {
		return sender;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr		pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr		pIStream
	);

private :

	UUId		uuid;
	ObservableCPtr	sender;

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_Event_H_

// ============================================================================

