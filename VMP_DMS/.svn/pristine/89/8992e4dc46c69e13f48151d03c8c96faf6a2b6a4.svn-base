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
//	BFC.MT.SysCS.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_SysCS_H_
#define _BFC_MT_SysCS_H_

// ============================================================================

#if defined(PLATFORM_WIN32)

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.SysLockable.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

struct SysCSData;

// ============================================================================

/// \brief OS-dependent critical section object (Win32 only).
///
/// Critical section objects provide synchronization similar to that provided
/// by mutex objects, but critical section objects provide a slightly faster,
/// more efficient mechanism for mutual-exclusion synchronization.
///
/// Like a mutex object, a critical section object can be owned by only one
/// thread at a time, which makes it useful for protecting a shared resource
/// from simultaneous access. There is no guarantee about the order in which
/// threads will obtain ownership of the critical section, however, the system
/// will be fair to all threads.
///
/// To enable mutually exclusive access to a shared resource, each thread calls
/// the lock() method to request ownership of the critical section before
/// executing any section of code that accesses the protected resource.
///
/// When it has finished executing the protected code, the thread uses the
/// unlock() method to relinquish ownership, enabling another thread to become
/// owner and access the protected resource.
///
/// The thread must call unlock() once for each time that it entered the
/// critical section. The thread enters the critical section each time
/// lock() succeeds.
///
/// \ingroup BFC_MT

class BFC_MT_DLL SysCS : public SysLockable {

public :

	SysCS(
	);

	/// \brief Destroys this critical section.
	///
	/// If a critical section is deleted while it is still owned, the state
	/// of the threads waiting for ownership of the deleted critical
	/// section is undefined.

	virtual ~SysCS(
	);

	/// \brief Enters (takes ownership of) this critical section.
	///
	/// This method waits for ownership of this critical section
	/// object. The function returns when the calling thread is granted
	/// ownership.
	///
	/// If a thread terminates while it has ownership of a critical
	/// section, the state of the critical section is undefined.


	virtual void lock(
	);

	/// \brief Tries to enter this critical section.
	///
	/// This method attempts to enter this critical section without
	/// blocking. If the call is successful, the calling thread takes
	/// ownership of this critical section.
	///
	/// \returns True iff the attempt succeeded.

	virtual Bool tryLock(
	);

	/// \brief Leaves (releases ownership of) this critical section.
	///
	/// If a critical section is deleted while it is still owned, the
	/// state of the threads waiting for ownership of the deleted critical
	/// section is undefined.

	virtual void unlock(
	);

protected :

private :

	// internal state...

	SysCSData	*data;

	// others...

	SysCS(
		const	SysCS&
	);

	SysCS& operator = (
		const	SysCS&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

#endif // _BFC_MT_SysCS_H_

// ============================================================================

