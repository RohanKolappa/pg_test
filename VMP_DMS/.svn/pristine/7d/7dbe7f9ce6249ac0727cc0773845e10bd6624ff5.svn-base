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
//	BFC.Msg.Callback.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_Callback_H_
#define _BFC_Msg_Callback_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Msg.CallbackPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Msg.EventPtr.h"
#include "BFC.Msg.ObserverPtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief Base class of all classes capable of triggering a specific callback
///	in an Observer.
///
/// You should not try to use this class yourself, unless you known what you're
/// doing.
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL Callback : virtual public SObject {

protected :

	/// \brief Creates a new Callback, owned by \a pObserver.

	Callback(
			ObserverCPtr	pObserver
	) :
		observer( pObserver ) {
	}

public :

	/// \brief Returns a pointer to the owner Observer.

	const ObserverCPtr & getObserver(
	) const {
		return observer;
	}

	/// \brief Sends Event \a pEvent to some object via some function.

	virtual void call(
			EventCPtr	pEvent
	) const = 0;

private :

	ObserverCPtr	observer;	///< Owner Observer.

};

// ============================================================================

/// \brief Class specialized in calling a member method of some object of
///	type \a T.
///
/// You should not try to use this class yourself, unless you known what you're
/// doing.
///
/// \ingroup BFC_Msg

template < class T >
class CallbackTmpl : public Callback {

public :

	typedef T * ObjectType;

	typedef void ( T::*MemberType )(
				EventCPtr
		);

	/// \brief Creates a new CallbackTmpl, owned by \a pObsOwner, and
	///	forwarding incoming events to the member method \a pCallback
	///	of object \a pObserver.

	CallbackTmpl(
			ObserverPtr	pObsOwner,
			ObjectType	pObserver,
			MemberType	pCallback
	) :
		Callback( pObsOwner ),
		observer( pObserver ),
		callback( pCallback ) {
	}

	/// \brief Sends Event \a pEvent to the attached object/member.

	virtual void call(
			EventCPtr	pEvent
	) const;

protected :

private :

	ObjectType	observer;	///< Object to send Events to.
	MemberType	callback;	///< Member method to call.

};

// ============================================================================

template < class T >
void CallbackTmpl< T >::call(
		EventCPtr	pEvent ) const {

	( observer->*callback )( pEvent );

}

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_Callback_H_

// ============================================================================

