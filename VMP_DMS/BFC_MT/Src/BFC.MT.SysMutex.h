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
//	BFC.MT.SysMutex.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_SysMutex_H_
#define _BFC_MT_SysMutex_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.NotOwner.h"
#include "BFC.MT.SysLockable.h"
#include "BFC.MT.WouldDeadLock.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief OS-dependent mutex.
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

class BFC_MT_DLL SysMutex : public SysLockable {

public :

	/// \brief Enumerated value determining the mutex type.
	///
	/// The type of a mutex determines whether it can be locked again by a
	/// thread that already owns it.

	enum Type {
		Normal = 0,
		Recursive,
		ErrorCheck
#if defined( PLATFORM_LINUX )
		, Fast
#endif
	};

	/// \brief Initializes this mutex object.
	///
	/// This constructor initializes this mutex object, according to the
	/// mutex type \a type.

	SysMutex(
		const	Type		type
	);

	/// \brief destroys this object.
	///
	/// The destructor destroys this mutex object, freeing the resources it
	/// might hold. The mutex must be unlocked on entrance.

	virtual ~SysMutex(
	);

	/// \brief Locks this mutex.
	///
	/// This method locks this mutex. If it is currently unlocked, it
	/// becomes locked and owned by the calling thread, and this method
	/// returns immediately. If the mutex is already locked by another
	/// thread, this method suspends the calling thread until the mutex is
	/// unlocked.
	///
	/// If the mutex is already locked by the calling thread, the behavior
	/// of this method depends on the type of the mutex. If the mutex is of
	/// the Fast type, the calling thread is suspended until the mutex is
	/// unlocked, thus effectively causing the calling thread to deadlock.
	/// If the mutex is of the ErrorCheck type, this method returns
	/// immediately, throwing a WouldDeadLock exception. If the mutex is of
	/// the Recursive type, this method succeeds and returns immediately,
	/// recording the number of times the calling thread has locked the
	/// mutex. An equal number of calls to the unlock() method must be
	/// performed before the mutex returns to the unlocked state.
	///
	/// \throws InternalError
	///
	/// \throws WouldDeadLock

	virtual void lock(
	);

	/// \brief Unlocks this mutex.
	///
	/// This method unlocks this mutex. The mutex is assumed to be locked
	/// and owned by the calling thread on entrance to this method. If the
	/// mutex is of the Fast type, this method always returns it to the
	/// unlocked state. If it is of the Recursive type, it decrements the
	/// count of the mutex (number of calls to the lock() method performed
	/// on it by the calling thread), and only when this count reaches zero
	/// is the mutex actually unlocked.
	///
	/// On ErrorCheck mutexes, this method actually checks at run-time that
	/// the mutex is locked on entrance, and that it was locked by the same
	/// thread that is now calling this method. If these conditions are not
	/// met, an exception is thrown and the mutex remains unchanged.
	/// Fast and Recursive mutexes perform no such checks, thus allowing a
	/// locked mutex to be unlocked by a thread other than its owner. This
	/// is non-portable behavior and must not be relied upon.
	///
	/// \throws InternalError
	///
	/// \throws NotOwner

	virtual void unlock(
	);

protected :

	void initMutex(
	);

private :

	friend class SysCondition;

	struct Data;

	Type		type;
	Data *		data;

	/// \brief Forbidden default constructor.

	SysMutex(
	);

	/// \brief Forbidden copy constructor.

	SysMutex(
		const	SysMutex &
	);

	/// \brief Forbidden copy operator.

	SysMutex & operator = (
		const	SysMutex &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_SysMutex_H_

// ============================================================================

