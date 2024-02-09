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
//	BFC.Msg.Producer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_Producer_H_
#define _BFC_Msg_Producer_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

BFC_PTR_DECL( BFC_Msg_DLL, Producer )

}
}

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Msg.Event.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief Base class of all classes generating Event objects.
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL Producer : virtual public SObject {

public :

	/// \brief Creates a new Producer.

	Producer(
	);

	/// \brief Destroys this object.

	virtual ~Producer(
	);

	/// \brief Returns the unique id associated to this Producer.
	///
	/// This id univoquely identifies this Producer to the unique
	/// Dispatcher.

	Uint32 getProdId(
	) const {
		return prodId;
	}

protected :

	/// \brief To be called in destructors (for example) in subclasses.

	void disconnect(
	);

	/// \brief To be used by subclasses to send Event objects.
	///
	/// This method is used by a Producer to send Event objects
	/// to some peer(s), with an associated service id.
	///
	/// \param pEvent
	///	The event to dispatch.
	///
	/// \param pServId
	///	The specific "channel" to use (default is -1, which means
	///	"broadcast", or "all channels").

	void dispatchEvent(
		const	EventPtr &	pEvent,
		const	Uint32		pServId = ( Uint32 )-1
	);

private :

	Uint32		prodId;		///< Unique id, given by Dispatcher.

	/// \brief Forbidden copy constructor.

	Producer(
		const	Producer &	other
	);

	/// \brief Forbidden copy operator.

	Producer & operator = (
		const	Producer &	other
	);

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_Producer_H_

// ============================================================================


