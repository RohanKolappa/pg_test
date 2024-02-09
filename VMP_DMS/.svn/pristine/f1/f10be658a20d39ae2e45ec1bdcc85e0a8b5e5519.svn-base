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
//	BFC.MT.Condition.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Condition_H_
#define _BFC_MT_Condition_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.MT.EmulSynchronizer.h"
#include "BFC.MT.Lockable.h"
#include "BFC.MT.NotLocked.h"
#include "BFC.MT.NotOwner.h"

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

class Mutex;

// ============================================================================

/// \brief A thread-aware OS-independent condition variable.
///
/// \ingroup BFC_MT

class BFC_MT_DLL Condition : public Lockable {

public :

	Condition(
			Mutex &		pSharedMutex
	) :
		shrdMutx( pSharedMutex ),
		signaled( false, false ) {
	}

	Mutex & getSharedMutex(
	) const {
		return shrdMutx;
	}

	/// \brief Restarts one of the threads that are waiting on this
	///	condition.
	///
	/// If there isn't any thread waiting, this method has no effect.

	void signal(
	);

	/// \brief Restarts all the threads that are waiting on this condition.

	void broadcast(
	);

	/// \brief Waits for the condition to be signaled.

//	void wait(
//		const	Buffer &	pLockMesg
//	);

	void wait(
		const	Bool		pChckStpd = true
	);

	Bool timedWait(
		const	Time::Delta &	delta,
		const	Bool		pChckStpd = true
	);

	Bool timedWait(
		const	Time::Clock &	timeout,
		const	Bool		pChckStpd = true
	);

	virtual void lock(
	) {
		lock( true );
	}

	virtual void lock(
		const	Bool		pChckStpd
	);

	virtual Bool tryLock(
	);

	virtual void unlock(
	);

protected :

private :

	Mutex &			shrdMutx;
	EmulSynchronizer	signaled;

	/// \brief Forbidden default constructor.

	Condition(
	);

	/// \brief Forbidden copy constructor.

	Condition(
		const	Condition &
	);

	/// \brief Forbidden copy operator.

	Condition & operator = (
		const	Condition &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Condition_H_

// ============================================================================

