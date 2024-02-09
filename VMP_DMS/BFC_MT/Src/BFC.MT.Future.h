// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.MT.Future.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Future_H_
#define _BFC_MT_Future_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.FuturePtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.MT.CancellationException.h"
#include "BFC.MT.ExecutionException.h"

#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief Represents the result of an asynchronous computation.
///
/// Methods are provided to check if the computation is complete, to wait for
/// its completion, and to retrieve the result of the computation. The result
/// can only be retrieved using method get() when the computation has
/// completed, blocking if necessary until it is ready. Cancellation is
/// performed by the cancel() method. Additional methods are provided to
/// determine if the task completed normally or was cancelled. Once a
/// computation has completed, the computation cannot be cancelled.
///
/// \ingroup BFC_MT

class BFC_MT_DLL Future : virtual public SObject {

public :

	/// \brief Attempts to cancel execution of this task.
	///
	/// This attempt will fail if the task has already completed, has
	/// already been cancelled, or could not be cancelled for some other
	/// reason. If successful, and this task has not started when cancel()
	/// is called, this task should never run. If the task has already
	/// started, then the \a mayInterruptIfRunning parameter determines
	/// whether the thread executing this task should be interrupted in
	/// an attempt to stop the task.
	///
	/// After this method returns, subsequent calls to isDone() will
	/// always return true. Subsequent calls to isCancelled() will always
	/// return true if this method returned true.
	///
	/// \param mayInterruptIfRunning
	///	true if the thread executing this task should be interrupted;
	///	otherwise, in-progress tasks are allowed to complete.
	///
	/// \returns
	///	false if the task could not be cancelled, typically because it
	/// has already completed normally; true otherwise.

	virtual Bool cancel(
		const	Bool		mayInterruptIfRunning
	) = 0;

	/// \brief Returns true if this task was cancelled before it completed
	///	normally.
	///
	/// \returns
	///	true if this task was cancelled before it completed.

	virtual Bool isCancelled(
	) = 0;

	/// \brief Returns true if this task completed.
	///
	/// Completion may be due to normal termination, an exception, or
	/// cancellation -- in all of these cases, this method will return
	/// true.
	///
	/// \returns
	///	true if this task completed.

	virtual Bool isDone(
	) = 0;

	/// \brief Waits if necessary for the computation to complete, and
	///	then retrieves its result.
	///
	/// \returns
	///	The computed result.
	///
	/// \throws CancellationException
	///	If the computation was cancelled.
	///
	/// \throws ExecutionException
	///	If the computation threw an exception.
	///
	/// \throws ThreadStopped
	///	If the current thread was interrupted while waiting.

	virtual SPtr get(
	) = 0;

	/// \brief Waits if necessary for at most the given time for the
	///	computation to complete, and then retrieves its result, if
	///	available.
	///
	/// \param timeout
	///	The maximum time to wait.
	///
	/// \returns
	///	The computed result.
	///
	/// \throws CancellationException
	///	If the computation was cancelled.
	///
	/// \throws ExecutionException
	///	If the computation threw an exception.
	///
	/// \throws ThreadStopped
	///	If the current thread was interrupted while waiting.
	///
	/// \throws TimeoutException
	///	If the wait timed out.

	virtual SPtr get(
		const	Time::Delta	timeout
	) = 0;

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Future_H_

// ============================================================================

