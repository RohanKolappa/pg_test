// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::MT".
// 
// "BFC::MT" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::MT" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::MT"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.MT.SysSyncEvent.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_SysSyncEvent_H_
#define _BFC_MT_SysSyncEvent_H_

// ============================================================================

#if defined( PLATFORM_WIN32 )

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.MT.SysLockable.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief OS-dependent event object (Win32 only).
///
/// \ingroup BFC_MT

class BFC_MT_DLL SysSyncEvent : public SysLockable {

public :

	/// \brief Creates a new event object.
	///
	/// The initial state of the event object is specified by the \a
	/// pSignaled parameter. Use the set() method to set the state of this
	/// event object to signaled. Use the reset() method to reset the
	/// state of this event object to nonsignaled. 
	///
	/// When the state of a manual-reset event object is signaled, it
	/// remains signaled until it is explicitly reset to nonsignaled by
	/// the reset() method. Any number of waiting threads, or threads that
	/// subsequently begin wait operations for this event object,
	/// can be released while the object's state is signaled.
	///
	/// When the state of an auto-reset event object is signaled, it
	/// remains signaled until a single waiting thread is released; the
	/// system then automatically resets the state to nonsignaled. If no
	/// threads are waiting, the event object's state remains signaled.
	///
	/// \param pSignaled
	///	Specifies the initial state of the event object. If TRUE, the
	///	initial state is signaled; otherwise, it is nonsignaled.
	///
	/// \param pManReset
	///	Specifies whether a manual-reset or auto-reset event object is
	///	created. If TRUE, then you must use the reset() method to
	///	manually reset the state to nonsignaled. If FALSE, the system
	///	automatically resets the state to nonsignaled after a single
	///	waiting thread has been released.

	SysSyncEvent(
		const	Bool		pSignaled = true,
		const	Bool		pManReset = false
	);

	/// \brief Destroys this object.

	virtual ~SysSyncEvent(
	);

	/// \brief Sets this event object to the signaled state.
	///
	/// The state of a manual-reset event object remains signaled until it
	/// is set explicitly to the nonsignaled state by the reset() method.
	/// Any number of waiting threads, or threads that subsequently begin
	/// wait operations for this event object by calling one of
	/// the wait functions, can be released while the object's state is
	/// signaled.
	///
	/// The state of an auto-reset event object remains signaled until a
	/// single waiting thread is released, at which time the system
	/// automatically sets the state to nonsignaled. If no threads are
	/// waiting, the event object's state remains signaled.
	///
	/// \throws InternalError
	///	The additional message gives a system-specific error message.

	void set(
	);

	/// \brief Sets this event object to the nonsignaled state.
	///
	/// The state of an event object remains nonsignaled until it is
	/// explicitly set to signaled by the set() or pulse() method. This
	/// nonsignaled state blocks the execution of any threads that have
	/// used one of the wait methods of this event object.
	///
	/// This method is used primarily for manual-reset event objects,
	/// which must be set explicitly to the nonsignaled state. Auto-reset
	/// event objects automatically change from signaled to nonsignaled
	/// after a single waiting thread is released.
	///
	/// \throws InternalError
	///	The additional message gives a system-specific error message.

	void reset(
	);

	/// \brief Returns when this object is in the signaled state.
	///
	/// This method checks the current state of this object. If the
	/// object's state is nonsignaled, the calling thread enters the wait
	/// state. It uses no processor time while waiting for the object
	/// state to become signaled.
	///
	/// \throws InternalError
	///	The additional message gives a system-specific error message.

	void wait(
	);

	/// \brief Returns when one of the objects is in the signaled state.
	///
	/// \param ev1
	///	First event (index 0) to wait on.
	///
	/// \param ev2
	///	Second event (index 1) to wait on.
	///
	/// \return
	///	This method returns the index of the event that has been
	///	signaled, thus 0 or 1.
	///
	/// \throws InternalError
	///	The additional message gives a system-specific error message.

	static Uint32 wait(
			SysSyncEvent &	ev1,
			SysSyncEvent &	ev2
	);

	/// \brief Returns when this object is in the signaled state, or when
	///	the timeout interval elapses.
	///
	/// This method checks the current state of this object. If the
	/// object's state is nonsignaled, the calling thread enters the wait
	/// state. It uses no processor time while waiting for the object
	/// state to become signaled or the time-out interval to elapse.
	///
	/// \param pWaitMSec
	///	Number of milliseconds to wait before a timeout occurs.
	///
	/// \return
	///	This method returns TRUE when the event is signaled, and FALSE
	///	if a timeout occurred.
	///
	/// \throws InvalidArgument
	///	\a pWaitMSec is 0.
	///
	/// \throws InternalError
	///	The additional message gives a system-specific error message.

	Bool timedWait(
		const	Uint32		pWaitMSec
	);

	/// \brief Allows the caller to atomically signal an event and wait
	///	on another event.
	///
	/// \param pToWaitOn
	///	Specifies the event to wait for after signaling this object.
	///
	/// \throws InternalError
	///	The additional message gives a system-specific error message.

	void signalAndWaitOn(
			SysSyncEvent &	pToWaitOn
	);

	/// \brief Allows the caller to atomically signal an event and wait
	///	on another event.
	///
	/// \param pToWaitOn
	///	Specifies the event to wait for after signaling this object.
	///
	/// \param pWaitMSec
	///	Number of milliseconds to wait before a timeout occurs.
	///
	/// \return
	///	This method returns TRUE when \a pToWaitOn is signaled, and
	///	FALSE if a timeout occurred.
	///
	/// \throws InvalidArgument
	///	\a pWaitMSec is 0.
	///
	/// \throws InternalError
	///	The additional message gives a system-specific error message.

	Bool signalAndTimedWaitOn(
			SysSyncEvent &	pToWaitOn,
		const	Uint32		pWaitMSec
	);

	virtual void lock(
	);

	virtual void unlock(
	);

protected :

private :

	struct Data;

	Data *	data;

	/// \brief Forbidden copy constructor.

	SysSyncEvent(
		const	SysSyncEvent &
	);

	/// \brief Forbidden copy oeprator.

	SysSyncEvent & operator = (
		const	SysSyncEvent &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

#endif // _BFC_MT_SysSyncEvent_H_

// ============================================================================

