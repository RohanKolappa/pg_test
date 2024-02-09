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
//	BFC.MT.Thread.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Thread_H_
#define _BFC_MT_Thread_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.ThreadPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"

#include "BFC.Debug.DObject.h"

#include "BFC.MT.EmulCondition.h"
#include "BFC.MT.EmulMutex.h"
#include "BFC.MT.EmulSynchronizer.h"

// ============================================================================

namespace Sys {
namespace MT {

class ThreadData;

} // namespace MT
} // namespace Sys

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief Base class of objects representing a specific thread of execution.
///
/// \ingroup BFC_MT

class BFC_MT_DLL Thread : virtual public DObject {

public :

	/// \brief Creates a new Thread object.

	Thread(
	);

	/// \brief Destroys this object, first stopping and joining the run()
	///	method, if it's still running.
	///
	/// A Thread should first be stopped before destroying it! Otherwise,
	/// the run() method could still be using resources of a child class,
	/// while this has already been destroyed, resulting in unexpected
	/// behaviour ("pure virtrual function call" on Windows, ...).

	virtual ~Thread(
	);

	/// \brief Starts executing the run() method in a separate thread of
	///	execution.

	virtual void start(
	);

	/// \brief Stops executing the run() method.
	///
	/// This method only sends a signal to the running thread, indicating
	/// it has to stop working. Wether a thread should stop working or not
	/// should be regularly tested in the run() method by using the
	/// shouldStop() method.
	///
	/// Additionnally, any thread blocked in some Lockable::lock() method
	/// is automatically release, and receives a ThreadStopped exception.

	virtual void stop(
	);

	/// \brief Blocks until the run() method has finished running.
	///
	/// This method MUST be called after each stop()!!!
	///
	/// \returns If the run() method terminates with an exception, this
	///	exception is returned here. It then MUST be deleted by the
	///	caller process.

	virtual Exception * join(
	);

	/// \brief Safely combines the stop() and join() steps in one call.

	void safeStopAndJoin(
	);

	static void exit(
		Int32	code = 0
	);

	/// \brief Enumerated representing the status of this object.

	enum Status {

		/// This object is waiting to be started by using the
		/// start() method.

		Stopped,

		/// This object has been started using the start() method,
		/// and is about to enter its run() method.

		Launching,

		/// This object has entered its run() method, and is executing
		/// it.

		Running,

		/// Thread has finished executing its run() method, and is
		/// waiting to be joined using the join() method.

		Waiting

	};

	/// \brief Returns the actual status of this object.

	Status getStatus(
	) const;

	/// \brief Returns the name of the actual status of this object.

	const char * getStatusName(
	) const {
		return getStatusName( getStatus() );
	}

	/// \brief Returns the name of the status \a s.

	static const char * getStatusName(
		const	Status		s
	);

	/// \brief Returns true if this stopped is stopped, and waiting to
	///	be started using the start() method.

	Bool isStopped(
	) const {
		return ( getStatus() == Stopped );
	}

	/// \brief Returns true if this object has been started using the
	///	start() method, and is not joinable yet.

	Bool isRunning(
	) const {
		Status s = getStatus();
		return ( s == Running || s == Launching );
	}

	/// \brief Returns true if this object is waiting to be joined, using
	///	the join() method.

	Bool isWaiting(
	) const {
		return ( getStatus() == Waiting );
	}

	Uint32 getThreadPID(
	) const;

	/// \brief Returns a pointer to the Thread object representing the
	///	thread of execution from which this method was called.
	///
	/// There should always be a Thread object representing this specific
	/// thread of execution, even for the "main" thread, i.e. the one in
	/// which the "main()" function executes.
	///
	/// An exception to this situation is when the user is also creating
	/// threads using some other mechanism, for example when BFC_Common
	/// is used in some exotic environment (ex. DirectShow). In this case,
	/// calling this method returns a pointer to "our" MainThread.
	///
	/// \note This breaks ALL the synchronization mechanisms in the
	/// multi-threading module!!! Accordingly, you should NEVER rely on
	/// the BFC synchronization objects to synchronize between threads that
	/// were NOT created using Thread!

	static Thread * self(
	);

	/// \brief Returns true if this object has been stopped using the
	///	stop() method, but is still executing its run() method.

	Bool shouldStop(
	) const {
		return shdStpFlg;
	}

protected :

	/// \brief Pure virtual method to reimplement in inheriting classes.

	virtual void run(
	) = 0;

	virtual void abort(
	);

	// Interface for the MainThread...

	static void initTLS(
	);

	static void setTLS(
			Thread *	ptr
	);

	static Thread * getTLS(
	);

	Sys::MT::ThreadData *	localData;	///< System specific data.
	Status			currState;	///< Thread status.
	mutable EmulMutex	stateMutx;	///< Thread status protection.
	EmulCondition		stateCond;	///< Thread status changed condition.
	Bool			shdStpFlg;	///< "Should stop" flag.
	EmulSynchronizer	shdStpEvt;	///< "Should stop" event.

private :

#if defined( PLATFORM_WIN32 )
	static void __cdecl launcher(
#else
	static void * launcher(
#endif
			void *		arg
	);

	friend class Condition;
	friend class CriticalSection;
	friend class Mutex;
	friend class Semaphore;

	EmulSynchronizer * getStoppedEvent(
	) {
		return &shdStpEvt;
	}

	Thread(
		const	Thread &
	);

	Thread & operator = (
		const	Thread &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Thread_H_

// ============================================================================

