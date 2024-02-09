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
//	BFC.Msg.Observable.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_Observable_H_
#define _BFC_Msg_Observable_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Msg.ObservablePtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Msg.Event.h"
#include "BFC.Msg.Observer.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief Base class of all objects capable of sending Event objects to
///	Observer objects.
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL Observable : virtual public SObject {

public :

	/// \brief Adds the callback identified by the pair (\a pObserver,
	///	\a pObsSrvId) to the list of callbacks to trigger when an
	///	Event is sent.
	///
	/// \param pObserver
	///	The Observer to warn.
	///
	/// \param pObsSrvId
	///	The id of the specific callback to use.

	void addObserver(
			ObserverPtr	pObserver,
		const	Observer::SrvId	pObsSrvId = Observer::AnyId
	);

	/// \brief Removes Observer \a pObserver from the list of Observer
	///	objects.

	void delObserver(
			ObserverPtr	pObserver
	);

	/// \brief Empties the list of Observer objects.

	void delObservers(
	) {
		callbacks.kill();
	}

	/// \brief Returns true if the list of Observer objects to warn is not
	///	empty.

	Bool hasObservers(
	) const {
		return ( ! callbacks.isEmpty() );
	}

	virtual void syncObservers(
	) const {
	}

protected :

	/// \brief Sends Event \a pEvent to all Observer objects.
	///
	/// \param pEvent
	///	The Event to send.

	void warnObservers(
			EventPtr	pEvent
	) const;

	/// \brief Sends Event \a pEvent to Observer \a pObserver on its
	///	service identified by \a pObsSrvId.
	///
	/// This should only be used by subclasses reimplementing the
	/// addObserver() method to send an initial synchronization event.
	///
	/// \param pObserver
	///	The Observer to warn.
	///
	/// \param pObsSrvId
	///	The service id.
	///
	/// \param pEvent
	///	The Event to send.

	void warnObserver(
			ObserverPtr	pObserver,
		const	Observer::SrvId	pObsSrvId,
			EventCPtr	pEvent
	) const;

	/// \brief Simple callback triggered every time a new Observer is
	///	added to this Observable.

	virtual void onObserverAdded(
			ObserverPtr	pObserver,
		const	Observer::SrvId	pObsSrvId
	);

private :

	Array< CallbackPtr >	callbacks;	///< Callbacks to trigger.

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_Observable_H_

// ============================================================================

