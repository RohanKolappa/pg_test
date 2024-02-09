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
//	BFC.MT.CriticalSection.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_CriticalSection_H_
#define _BFC_MT_CriticalSection_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.MT.EmulSynchronizer.h"
#include "BFC.MT.Lockable.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

class Thread;

// ============================================================================

/// \brief A critical section.
///
/// \ingroup BFC_MT

class BFC_MT_DLL CriticalSection : public Lockable {

public :

	CriticalSection(
	);

	virtual ~CriticalSection(
	);

	virtual void lock(
	) {
		lock( true );
	}

	virtual void lock(
		const	Bool		checkStopped
	);

	virtual Bool tryLock(
	);

	virtual void unlock(
	);

	void enter(
		const	Bool		checkStopped
	) {
		lock( checkStopped );
	}

	Bool tryEnter(
	) {
		return tryLock();
	}

	void leave(
	) {
		unlock();
	}

	BASE_CLASS_GEN_EXCEPTION("BFC.CriticalSection");

	CLASS_EXCEPTION(NotLocked, "Not locked");
	CLASS_EXCEPTION(NotOwner, "Not owner");
	CLASS_EXCEPTION(WouldDeadLock, "Would dead-lock");

protected :

private :

	// internal state...

	EmulSynchronizer	accessEvent;
	EmulSynchronizer	unlockedEvent;
	Thread			*owner;

	// others...

	CriticalSection(
		const	CriticalSection&
	);

	CriticalSection& operator = (
		const	CriticalSection&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_CriticalSection_H_

// ============================================================================

