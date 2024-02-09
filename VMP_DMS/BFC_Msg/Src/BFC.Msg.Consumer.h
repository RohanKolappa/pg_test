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
//	BFC.Msg.Consumer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_Consumer_H_
#define _BFC_Msg_Consumer_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

BFC_PTR_DECL( BFC_Msg_DLL, Consumer )

}
}

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Msg.Event.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief Base class of all classes that can handle incoming Message objects.
///
/// An object inheriting the Consumer interface has to implement
/// only one method (eventHandler) that will get called every time an
/// Event has been received and has to be handled.
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL Consumer : virtual public SObject {

public :

	/// \brief Creates a new Consumer.

	Consumer(
	);

	/// \brief Destroys this object.

	virtual ~Consumer(
	);

	/// \brief Returns the unique id associated to this Consumer.
	///
	/// This id univoquely identifies this Consumer to the unique
	/// Dispatcher.

	Uint32 getConsId(
	) const {
		return consId;
	}

	/// \brief To be implemented in subclasses.
	///
	/// This handler will receive incoming Event objects, with an
	/// indication of the service id those objects have been sent to.

	virtual void eventHandler(
		const	EventPtr &		event,
		const	Uint32			servId
	) = 0;

protected :

	/// \brief To be called in destructors (for example) in subclasses.

	void disconnect(
	);

private :

	Uint32		consId;		///< Unique id, given by Dispatcher.

	/// \brief Forbidden copy constructor.

	Consumer(
		const	Consumer &		other
	);

	/// \brief Forbidden copy operator.

	Consumer & operator = (
		const	Consumer &		other
	);

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_Consumer_H_

// ============================================================================


