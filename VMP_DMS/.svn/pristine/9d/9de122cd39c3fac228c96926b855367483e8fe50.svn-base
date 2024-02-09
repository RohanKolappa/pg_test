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
//	BFC.MT.SysCondition.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_SysCondition_H_
#define _BFC_MT_SysCondition_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

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

class SysMutex;

// ============================================================================

/// \brief OS-dependent condition variable (Linux only).
///
/// A condition (short for "condition variable") is a synchronization device
/// that allows threads to suspend execution and relinquish the processors
/// until some predicate on shared data is satisfied.
///
/// The basic operations on conditions are: signal the condition (when the
/// predicate becomes true), and wait for the condition, suspending the thread
/// execution until another thread signals the condition.
///
/// A condition variable must always be associated with a mutex, to avoid the
/// race condition where a thread prepares to wait on a condition variable and
/// another thread signals the condition just before the first thread actually
/// waits on it.
///
/// \ingroup BFC_MT

class BFC_MT_DLL SysCondition : public SysLockable {

public :

	/// \brief Creates and initializes a new Condition variable.
	///
	/// This constructor creates and initializes a new Condition variable,
	/// using \a pShrdMutx as the SysMutex protecting the predicate.

	SysCondition(
			SysMutex &	pShrdMutx
	);

	/// \brief Destroys this condition variable.
	///
	/// The destructor destroys this condition variable, freeing the
	/// resources it might hold. No threads must be waiting on the
	/// condition variable on entrance to the destructor.

	virtual ~SysCondition(
	);

	/// \brief Restarts one the waiting threads.
	///
	/// This method restarts one of the threads that are waiting on this
	/// condition variable. If no threads are waiting, nothing happens.
	/// If several threads are waiting, exactly one is restarted, but it is
	/// not specified which.
	///
	/// \throws CantSignal

	void signal(
	);

	/// \brief Restarts all the waiting threads.
	///
	/// This method restarts all the threads that are waiting on this
	/// condition variable. Nothing happens if no threads are waiting on
	/// this condition.
	///
	/// \throws CantBroadcast

	void broadcast(
	);

	/// \brief Atomically unlocks the mutex, and starts waiting.
	///
	/// This method atomically unlocks the SysMutex associated with this
	/// object (see constructors), and starts waiting for this condition
	/// variable to be signaled. The thread execution is suspended and does
	/// not consume any CPU time until the condition variable is signaled.
	/// The SysMutex must be locked by the calling thread on entrance to
	/// this method.
	///
	/// Before returning to the calling thread, this method
	/// re-acquires the SysMutex object.
	///
	/// Unlocking the mutex and suspending on the condition variable is
	/// done atomically. Thus, if all threads always acquire the mutex
	/// before signaling the condition, this guarantees that the condition
	/// cannot be signaled (and thus ignored) between the time a thread
	/// locks the mutex and the time it waits on the condition variable.
	///
	/// \throws CantWait

	void wait(
	);

	/// \brief Atomically unlocks the mutex, and starts waiting.
	///
	/// This method atomically unlocks the SysMutex associated with this
	/// object (see constructors), and starts waiting for this condition
	/// variable to be signaled, as wait() does, but it also bounds the
	/// duration of the wait.
	///
	/// If this condition has not been signaled within the amount of time
	/// specified by \a delta, the mutex is re-acquired and this method
	/// returns FALSE. Otherwise, it returns TRUE.
	///
	/// \throws CantWait

	Bool timedWait(
		const	Time::Delta &	delta
	);

	/// \brief Atomically unlocks the mutex, and starts waiting.
	///
	/// \overload

	Bool timedWait(
		const	Time::Clock &	timeout
	);

	/// \brief Locks the internal SysMutex.

	virtual void lock(
	);

	/// \brief Unlocks the internal SysMutex.

	virtual void unlock(
	);

protected :

private :

	struct Data;

	SysMutex &	shrdMutx;
	Data *		condData;

	/// \brief Forbidden copy constructor.

	SysCondition(
		const SysCondition &
	);

	/// \brief Forbidden copy operator.

	SysCondition & operator = (
		const SysCondition &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_SysCondition_H_

// ============================================================================

