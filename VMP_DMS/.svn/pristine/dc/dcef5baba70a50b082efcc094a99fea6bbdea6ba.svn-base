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
//	BFC.Msg.Observer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_Observer_H_
#define _BFC_Msg_Observer_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Msg.ObserverPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Msg.Callback.h"
#include "BFC.Msg.Event.h"

#include "BFC.STL.SHash.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL Observer : virtual public SObject {

public :

	typedef Uint32	SrvId;		///< Service id type.

	enum {
		AnyId = ( SrvId )0	///< "Any" service.
	};

	CallbackPtr getCallback(
		const	SrvId		pObsSrvId = AnyId
	) const;

protected :

	/// \brief Registers a new "service".
	///
	/// \param pObserver
	///	An pointer to an object that will be warned upon Event
	///	reception.
	///
	/// \param pCallback
	///	A method of this object to actually call.
	///
	/// \param pObsSrvId
	///	The id to associate to this service, to be used when
	///	attaching this Observer to an Observable.

	template < typename T >
	void addCallback(
			T *		pObserver,
			void		( T::*pCallback )( EventCPtr ),
		const	SrvId		pObsSrvId = AnyId
	) {
		ObserverPtr that = ObserverPtr( this, false, true );
		CallbackPtr cb = new CallbackTmpl< T >( that, pObserver, pCallback );
		addCallback( cb, pObsSrvId );
	}

	/// \brief Unregisters the "service" whose id is \a pObsSrvId (if any).

	void delCallback(
		const	SrvId		pObsSrvId = AnyId
	) {
		callers.remove( pObsSrvId );
	}

	/// \brief Unregisters all "services".

	void delCallbacks(
	) {
		callers.kill();
	}

private :

	void addCallback(
			CallbackPtr	pCallback,
		const	SrvId		pObsSrvId = AnyId
	) {
		callers.insert( pObsSrvId, pCallback );
	}

	STL::SHash		callers;

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_Observer_H_

// ============================================================================

