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
//	BFC.MT.Scheduler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Scheduler_H_
#define _BFC_MT_Scheduler_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.SchedulerPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.MT.Condition.h"
#include "BFC.MT.Mutex.h"
#include "BFC.MT.Tasklet.h"
#include "BFC.MT.Thread.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief Manages a list of Thread objects.
///
/// A Scheduler is used to start, stop, and join Thread objects.
///
/// \ingroup BFC_MT

class BFC_MT_DLL Scheduler : virtual public SObject {

public :

	/// \brief Creates a new Scheduler.

	Scheduler(
	);

	/// \brief Adds Tasklet \a pTask to this object, to be started
	///	immediatly.

	void addTask(
			TaskletPtr	pTask,
		const	Bool		pMaster = false
	);

	/// \brief Adds Tasklet \a pTask to this object, to be started
	///	at \a pWhen.

	void addTask(
		const	Time::Clock	pWhen,
			TaskletPtr	pTask,
		const	Bool		pMaster = false
	);

	/// \brief Adds Tasklet \a pTask to this object, to be started
	///	in \a pWhen time.

	void addTask(
		const	Time::Delta	pWhen,
			TaskletPtr	pTask,
		const	Bool		pMaster = false
	);

	/// \brief Adds Thread a pThread to this object, to be started
	///	immediatly.

	void addThread(
			ThreadPtr	pThread,
		const	Bool		pMaster = false
	);

	/// \brief Adds Thread a pThread to this object, to be started
	///	at \a pWhen.

	void addThread(
		const	Time::Clock	pWhen,
			ThreadPtr	pThread,
		const	Bool		pMaster = false
	);

	/// \brief Adds Thread a pThread to this object, to be started
	///	in \a pWhen time.

	void addThread(
		const	Time::Delta	pWhen,
			ThreadPtr	pThread,
		const	Bool		pMaster = false
	);

	/// \brief Finalizes this object, stopping any running Thread, and
	///	preventing further addition of any new Thread.

	void kill(
	);

	Bool isEmpty(
	) const;

	Bool isDead(
	) const;

	/// \brief Blocks until this Scheduler becomes "empty" (no subthread
	///	to start, stop, or join).

	void waitEmpty(
	);

protected :

	BFC_PTR_DECL( BFC_MT_DLL, Entry )

	class BFC_MT_DLL Entry : virtual public SObject {

	public :

		virtual ~Entry(
		);

		Time::Clock	when;
		ThreadPtr	thrd;
		TaskletPtr	task;
		Bool		mstr;

	};

	BFC_PTR_DECL( BFC_MT_DLL, Worker )

	class BFC_MT_DLL Worker : public Thread {

	public :

		Worker(
		);

		virtual ~Worker(
		);

		void insertEntry(
				EntryPtr	pEntry
		);

		void kill(
		);

		Bool isEmpty(
		) const;

		Bool isDead(
		) const;

		void waitEmpty(
		);

	protected :

		virtual void run(
		);

		void cleanup(
		);

	private :

		Mutex			dataMutex;	///< Protects agenda.
		Condition		schedCond;	///< Signals any change in the agenda.
		Condition		emptyCond;
		Array< EntryPtr >	waiting;
		Array< EntryPtr >	running;
		Bool			finishing;

	};

private :

	WorkerPtr	wrk;

	/// \brief Forbidden copy constructor.

	Scheduler(
		const	Scheduler &
	);

	/// \brief Forbidden copy operator.

	Scheduler & operator = (
		const	Scheduler &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Scheduler_H_

// ============================================================================

