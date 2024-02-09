// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "Sys::MT".
// 
// "Sys::MT" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "Sys::MT" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "Sys::MT"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	Sys.MT.ThreadCreator.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _Sys_MT_ThreadCreator_H_
#define _Sys_MT_ThreadCreator_H_

// ============================================================================

#include "Sys.MT.DLL.h"

// ============================================================================

namespace Sys {
namespace MT {

// ============================================================================

class ThreadData;

// ============================================================================

/// \brief Wrapper for the "pthread_create" function call.
///
/// This class implements a default behaviour (simple wrapper). Subclasses are
/// used to implement other behaviours, for example when running under the
/// supervision of a profiler, where it is mandatory to keep track of all
/// running threads.
///
/// \ingroup Sys_MT

class Sys_MT_DLL ThreadCreator {

public :

	/// \brief Creates a new ThreadCreator.

	ThreadCreator(
	);

	/// \brief Destroys this object.

	virtual ~ThreadCreator(
	);

	/// \brief Returns the unique instance that should be used to create
	///	new threads.

	static ThreadCreator * instance(
	);

	virtual bool create(
			ThreadData *	localData,
#if defined( PLATFORM_WIN32 )
			void		( * start_routine )( void * ),
#else
			void *		( * start_routine )( void * ),
#endif
			void *		arg
	);

protected :

private :

	static ThreadCreator *		threadCreator;

	/// \brief Forbidden copy constructor.

	ThreadCreator(
		const	ThreadCreator&
	);

	/// \brief Forbidden copy operator.

	ThreadCreator& operator = (
		const	ThreadCreator&
	);

};

// ============================================================================

} // namespace MT
} // namespace Sys

// ============================================================================

#endif // _Sys_MT_ThreadCreator_H_

// ============================================================================

