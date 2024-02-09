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
//	BFC.MT.Mutex.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Mutex_H_
#define _BFC_MT_Mutex_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.EmulSynchronizer.h"
#include "BFC.MT.Lockable.h"
#include "BFC.MT.NotLocked.h"
#include "BFC.MT.NotOwner.h"
#include "BFC.MT.WouldDeadLock.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

class Thread;

// ============================================================================

/// \brief A simple OS independent mutex (MUTual EXclusion) implementation.
///
/// A mutex is a MUTual EXclusion device, and is useful for protecting shared
/// data structures from concurrent modifications, and implementing critical
/// sections and monitors.
///
/// A mutex has two possible states: unlocked (not owned by any thread), and
/// locked (owned by one thread). A mutex can never be owned by two different
/// threads simultaneously. A thread attempting to lock a mutex that is already
/// locked by another thread is suspended until the owning thread unlocks the
/// mutex first.
///
/// \ingroup BFC_MT

class BFC_MT_DLL Mutex : public Lockable {

public :

	/// \brief Mutex type enum.
	///
	/// This implementation of a Mutex supports 2 different types of
	/// Mutex: Normal or Recursive. A Recursive Mutex can be locked again
	/// by a thread that already owns it. A Normal Thread cannot.

	enum Type {
		Normal		= 0,
		Recursive	= 1
	};

	/// \brief Creates a new Mutex of type \a pMutexType.
	///
	/// This constructor initializes this object according to the type
	/// attribute specified by \a pMutexType.
	///
	/// \param pMutexType
	///	Gives the Type of this Mutex (default: Normal).

	Mutex( 
		const	Type		pMutexType = Normal
	);

	virtual void lock(
	) {
		lock( true );
	}

	/// \brief Locks this object.
	///
	/// This method locks this object. If the mutex is currently unlocked,
	/// it becomes locked and owned by the calling thread, and this method
	/// returns immediately. If the mutex is already locked by another
	/// thread, this method suspends the calling thread until the mutex is
	/// unlocked by its current owner.
	///
	/// If the mutex is already locked by the calling thread, the behavior
	/// of this method depends on the type of the mutex. If this mutex is
	/// of the Normal type, this method throws a WouldDeadLock exception.
	/// If this mutex is of the Recursive type, this method succeeds and
	/// returns immediately, recording the number of times the calling
	/// thread has locked the mutex. An equal number of calls to the
	/// unlock() method must be performed before the mutex returns to the
	/// unlocked state.
	///
	/// \note
	///	This method IS a cancelation point! This means a thread
	///	trying to `lock()' a mutex CAN BE STOPPED using the
	///	Thread::stop() method!!!
	///
	/// \param pCheckStopped
	///	If true, this method gets interrupted (and actually throws a
	///	ThreadStopped exception) if the thread that is blocked is
	///	stopped using the Thread::stop() method.

	virtual void lock(
		const	Bool		pCheckStopped
	);

	/// \brief Tries to lock this object.
	///
	/// This method behaves identically to the lock() method, except that
	/// it does not block the calling thread if the mutex is already locked
	/// by another thread. Instead, it returns immediately with the "false"
	/// value.
	/// Otherwise, it takes ownership of the mutex, and returns true.

	virtual Bool tryLock( 
	);

	/// \brief Unlocks this object.
	///
	/// This method unlocks the given mutex. The mutex is assumed to be
	/// locked and owned by the calling thread on entrance to this method.
	///
	/// If this mutex is of the Recursive type, this method decrements the
	/// locking count of the mutex ( number of lock() calls performed on it
	/// by the calling thread ), and only when this count reaches zero is
	/// the mutex actually unlocked.
	///
	/// On Normal mutexes, this method actually checks at run-time that the
	/// mutex is locked on entrance, and that it was locked by the same
	/// thread that is now calling this method. If these conditions are not
	/// met, an exception is thrown, and the mutex remains unchanged.

	virtual void unlock(
	);

protected :

private :

	friend class Condition;

	Type			mutxType;
	EmulSynchronizer	critSect;	///< Signaled, auto.
	EmulSynchronizer	unlocked;	///< Not signaled, auto.
	Thread *		owner;
	Uint32			count;

	/// \brief Forbidden copy constructor.

	Mutex(
		const	Mutex &
	);

	/// \brief Forbidden copy operator.

	Mutex & operator = (
		const	Mutex &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Mutex_H_

// ============================================================================

