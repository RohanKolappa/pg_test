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
//	BFC.MT.MainThread.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_MainThread_H_
#define _BFC_MT_MainThread_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Debug.ThreadFinder.h"

#include "BFC.MT.Thread.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief Singleton representing the main thread of execution.
///
/// \ingroup BFC_MT

class BFC_MT_DLL MainThread : public Thread {

protected :

	/// \brief Unique (protected) constructor.
	///
	/// The unique main thread is created automatically and is accessed
	/// using the instance() method.

	MainThread(
	);		// singleton !!!!

public :

	/// \brief Destroys this object.

	virtual ~MainThread(
	);

	/// \brief Returns a reference to the unique main thread.

	static MainThread & instance(
	);

	virtual void start(
	);

	virtual void stop(
	);

	virtual Exception * join(
	);

	// ... to play with POSIX signals...

	enum SignalEnum {
		Hangup		= 1,	//	SIGHUP
		Quit		= 3,	//	SIGQUIT
		Interrupt	= 2,	//	SIGINT
		Term		= 15	//	SIGTERM
	};

	typedef void (*SignalHandler)(SignalEnum);

	virtual void installSignalHandler(
		SignalEnum	sig,
		SignalHandler	handler
	);

protected :

	static void centralSignalHandler(
			int
	);

	void realSignalHandler(
			int
	);

	virtual void run(
	);

private :


	typedef Map< SignalEnum, SignalHandler > HandlersMap;

	HandlersMap	handlers;

};

// ============================================================================

/// \brief Thread-aware thread name finder.
///
/// \ingroup BFC_MT

class BFC_MT_DLL MTThreadFinder : public ThreadFinder {

public :

	virtual const Buffer & getThreadName(
	) const;

	virtual Bool selfShouldStop(
	) const;

};

// ============================================================================

extern MainThread & mainThread;

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_MainThread_H_

// ============================================================================

