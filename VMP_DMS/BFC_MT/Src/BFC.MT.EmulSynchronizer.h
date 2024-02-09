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
//	BFC.MT.EmulSynchronizer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_EmulSynchronizer_H_
#define _BFC_MT_EmulSynchronizer_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.MT.EmulCS.h"
#include "BFC.MT.SysLockable.h"

// ============================================================================

namespace BFC {
namespace Time {

class Delta;
class Clock;

} // namespace Time
} // namespace BFC

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief An OS-independent thread synchronizer.
///
/// \ingroup BFC_MT

class BFC_MT_DLL EmulSynchronizer : public SysLockable {

public :

	/// \brief Creates a new sync object.
	///
	/// The initial state of the sync object is specified by the \a
	/// signaled parameter. Use the signal() method to set the state of
	/// this sync object to signaled. Use the reset() method to reset the
	/// state of this sync object to nonsignaled. 
	///
	/// When the state of a manual-reset sync object is signaled, it
	/// remains signaled until it is explicitly reset to nonsignaled by
	/// the reset() method. Any number of waiting threads, or threads that
	/// subsequently begin wait operations for this sync object,
	/// can be released while the object's state is signaled.
	///
	/// When the state of an auto-reset sync object is signaled, it
	/// remains signaled until a single waiting thread is released; the
	/// system then automatically resets the state to nonsignaled. If no
	/// threads are waiting, the event object's state remains signaled.
	///
	/// \param signaled
	///	Specifies the initial state of the sync object. If TRUE, the
	///	initial state is signaled; otherwise, it is nonsignaled.
	///
	/// \param manualReset
	///	Specifies whether a manual-reset or auto-reset sync object is
	///	created. If TRUE, then you must use the reset() method to
	///	manually reset the state to nonsignaled. If FALSE, the system
	///	automatically resets the state to nonsignaled after at least one
	///	waiting thread has been released.

	EmulSynchronizer(
		const	Bool		signaled,	// = true
		const	Bool		manualReset	// = false
	);

	/// \brief Wake-up as many waiting threads as possible.
	///
	/// If there are waiting threads, then this method wakes them all at
	/// once, then returns to the nonsignaled state if this is a auto-reset
	/// sync object, or stays in the signaled state if this is a
	/// manual-reset object.
	///
	/// If there are no waiting threads, this sync object is put in the
	/// signaled state.
	///
	/// \throws CantSignalEvent
	///	The additional message gives a system-specific error message.

	void broadcast(
	);

	/// \brief Sets this sync object to the signaled state.
	///
	/// The state of a manual-reset event object remains signaled until it
	/// is set explicitly to the nonsignaled state by the reset() method.
	/// Any number of waiting threads, or threads that subsequently begin
	/// wait operations for this sync object by calling one of
	/// the wait functions, can be released while the object's state is
	/// signaled.
	///
	/// The state of an auto-reset event object remains signaled until a
	/// single waiting thread is released, at which time the system
	/// automatically sets the state to nonsignaled. If no threads are
	/// waiting, the event object's state remains signaled.
	///
	/// \throws CantSignalEvent
	///	The additional message gives a system-specific error message.

	void signal(
	);

	void pulse(
	);

	void reset(
	);

	void wait(
	);

	Bool tryWait(
	);

	Bool timedWait(
		const	Time::Delta &	timeout
	);

	Bool timedWait(
		const	Time::Clock &	timeout
	);

	static Uint32 wait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1
	);

	static Int32 timedWait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1,
		const	Time::Delta &		timeout
	);

	static Int32 timedWait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1,
		const	Time::Clock &		timeout
	);

	static Uint32 wait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1,
			EmulSynchronizer *	sync2
	);

//	static Int32 timedWait(
//			EmulSynchronizer *	sync0,
//			EmulSynchronizer *	sync1,
//			EmulSynchronizer *	sync2,
//		const	Time::Delta &		timeout
//	);
//
//	static Int32 timedWait(
//			EmulSynchronizer *	sync0,
//			EmulSynchronizer *	sync1,
//			EmulSynchronizer *	sync2,
//		const	Time::Clock &		timeout
//	);

	void signalAndWait(
			EmulSynchronizer *	sync0
	);

	Bool signalAndTimedWait(
			EmulSynchronizer *	sync0,
		const	Time::Delta &		timeout
	);

	Bool signalAndTimedWait(
			EmulSynchronizer *	sync0,
		const	Time::Clock &		timeout
	);

	Uint32 signalAndWait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1
	);

	Int32 signalAndTimedWait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1,
		const	Time::Delta &		timeout
	);

	Int32 signalAndTimedWait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1,
		const	Time::Clock &		timeout
	);

	Uint32 signalAndWait(
			EmulSynchronizer *	sync0,
			EmulSynchronizer *	sync1,
			EmulSynchronizer *	sync2
	);

//	Int32 signalAndTimedWait(
//			EmulSynchronizer *	sync0,
//			EmulSynchronizer *	sync1,
//			EmulSynchronizer *	sync2,
//		const	Time::Delta &		timeout
//	);
//
//	Int32 signalAndTimedWait(
//			EmulSynchronizer *	sync0,
//			EmulSynchronizer *	sync1,
//			EmulSynchronizer *	sync2,
//		const	Time::Clock &		timeout
//	);

	virtual void lock(
	) {
		wait();
	}

	virtual Bool tryLock(
	) {
		return tryWait();
	}

	virtual void unlock(
	) {
		signal();
	}

protected :

	struct Data;

	Bool addBlocked(
			Data *		blocked,
			Bool		firstCalled
	);

	void delBlocked(
			Data *		blocked
	);

private :

	enum {
		MAX_BLOCKED		= 256
	};

	EmulCS		cs;	///< Protects the whole internal state.
	Bool		signaled;
	Bool		manReset;
	Uint32		lastUsed;
	Data *		blocked[ MAX_BLOCKED ];

	/// \brief Forbidden default constructor.

	EmulSynchronizer(
	);

	/// \brief Forbidden copy constructor.

	EmulSynchronizer(
		const	EmulSynchronizer &
	);

	/// \brief Forbidden copy operator.

	EmulSynchronizer & operator = (
		const	EmulSynchronizer &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_EmulSynchronizer_H_

// ============================================================================

