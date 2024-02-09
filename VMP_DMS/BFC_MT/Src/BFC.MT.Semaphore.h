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
//	BFC.MT.Semaphore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Semaphore_H_
#define _BFC_MT_Semaphore_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.EmulSynchronizer.h"
#include "BFC.MT.Lockable.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief A semaphore.
///
/// \ingroup BFC_MT

class BFC_MT_DLL Semaphore : public Lockable {

public :

	Semaphore(
		const	Uint32		initialValue = 0
	);

	void wait(
		const	Bool		checkStopped = true
	);

	Bool tryWait(
	);

	void post(
	);

	virtual void lock(
	) {
		lock( true );
	}

	virtual void lock(
		const	Bool		checkStopped
	) {
		wait( checkStopped );
	}

	virtual Bool tryLock(
	) {
		return tryWait();
	}

	virtual void unlock(
	) {
		post();
	}

	Uint32 getCounter(
	) const {
		return count;
	}

protected :

private :

	Uint32			count;
	EmulSynchronizer	accessEvent;
	EmulSynchronizer	notNullEvent;

	Semaphore(
		const Semaphore&
	);

	Semaphore& operator = (
		const Semaphore&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Semaphore_H_

// ============================================================================

