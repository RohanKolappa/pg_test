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
//	BFC.MT.EmulCondition.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_EmulCondition_H_
#define _BFC_MT_EmulCondition_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.SysLockable.h"

//#if defined( PLATFORM_WIN32 )
//#include "BFC.MT.SysSyncEvent.h"
//#else
#include "BFC.MT.SysCondition.h"
//#endif

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

class EmulMutex;

// ============================================================================

/// \brief An OS-independent condition variable.
///
/// See the documentation of the SysCondition class for more information.
///
/// \ingroup BFC_MT

class BFC_MT_DLL EmulCondition : public SysLockable {

public :

	EmulCondition(
			EmulMutex &	pShrdMutx
	);

	void signal(
	);

	void broadcast(
	);

	void wait(
	);

	Bool timedWait(
		const	Time::Delta &	delta
	);

	Bool timedWait(
		const	Time::Clock &	timeout
	);

	virtual void lock(
	);

	virtual void unlock(
	);

protected :

private :

	EmulMutex &	shrdMutx;

//#if defined( PLATFORM_WIN32 )
//
//	struct WaiterData {
//		WaiterData(
//		) :
//			flag	( false ),
//			prev	( 0 ),
//			next	( 0 )
//		{
//		}
//		Bool		flag;
//		WaiterData *	prev;
//		WaiterData *	next;
//	};
//
//	void insertWaiterData(
//			WaiterData *
//	);
//
//	void removeWaiterData(
//			WaiterData *
//	);
//
//	WaiterData *	waiterBeg;
//	WaiterData *	waiterEnd;
//
//	SysSyncEvent	signalEvent;
//
//#else

	SysCondition	cond;

//#endif

	/// \brief Forbidden copy constructor.

	EmulCondition(
		const EmulCondition &
	);

	/// \brief Forbidden copy operator.

	EmulCondition & operator = (
		const EmulCondition &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_EmulCondition_H_

// ============================================================================

