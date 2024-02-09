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
// FileName:
//	BFC.MT.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_MT_DLL_H_
#define _BFC_MT_DLL_H_

// ============================================================================

/// \defgroup BFC_MT BFC::MT (multi-threading module)
///
/// \brief Classes used to implement multi-threaded applications.
///
/// This modules provides 2 main categories of objects:
/// - classes representing separate threads of execution in an OS-independent
///	manner
/// - classes providing synchronization mechanisms.
///
/// The most important class here is the BFC::MT::Thread class, with its inheriting
/// class BFC::MainThread, which represents the "main" thread of execution (the
/// one in which the "main()" function was started).
///
/// The synchronization classes are organized in 3 layers, ranging from the
/// most simple to the most complex:
/// - OS layer: this layer contains classes wrapping the objects and functions
///	provided by the OS in compact C++ classes:
///	- BFC::SysCS: (Win32 only) A critical section.
///	- BFC::SysMutex: (Linux only) A MUTual EXclusion object.
///	- BFC::SysCondition: (Linux only) A condition variable.
///	- BFC::SysSemaphore: (Linux only) A semaphore.
///	- BFC::SysSyncEvent: (Win32 only) A generic synchronization event.
/// - Emulation layer: this layer implements an OS-independent version of each
///	of the classes found in the OS layer, using the direct implementation
///	found in this OS layer, of emulating it using some other classes:
///	- BFC::EmulCS: A fast critical section.
///	- BFC::EmulMutex: A MUTual EXclusion object.
///	- BFC::EmulCondition: A condition variable.
///	- BFC::EmulSemaphore: A semaphore
///	- BFC::EmulSynchronizer: A generic synchronization object.
///	- BFC::RWMutex: A N-readers, 1-writer mutex.
///	.
///	Note that all those objects are NOT thread-aware, meaning that their
///	blocking functions (mainly all the lock() and wait() methods) CANNOT BE
///	interrupted using the Thread::stop() method.
///	You should avoid using those classes directly! Use the safe layer
///	instead!
/// - Safe layer: this layer provides thread-aware (i.e. interruptible)
///	versions of some of the classes above:
///	- BFC::CriticalSection
///	- BFC::MT::Mutex
///	- BFC::MT::Condition
///	- BFC::Semaphore
///	.
///	The blocking methods of those classes throw the ThreadStopped
///	exception when the thread of execution they are called from is being
///	stopped using the Thread::stop() method.
///	Additionnally, all methods perform error checking (owner for mutex,
///	...).
///
/// To help lock and unlock synchronization objects in a safe way, this module
/// also provides auto-locking classes:
/// -	BFC::SysLocker: automatically locks any BFC::SysLockable object upon
///	construction, and automatically unlocks it upon destruction.
/// -	BFC::Locker: automatically locks any BFC::Lockable object upon
///	construction, and automatically unlocks it upon destruction.
/// -	BFC::RWLocker: automatically locks (in read or write mode) a
///	BFC::RWMutex object upon construction, and automatically unlocks it
///	upon destruction.

// ============================================================================

// Some preliminary definitions.
// Those used to be provided by the BFC.Base.Types.h header file. They are now
// replicated all over the place :-)

#if ! defined( JBS_DLL_DEFS )
#  define JBS_DLL_DEFS
#  define JBS_DLL_NONE
#  if defined( PLATFORM_WIN32 )
#    define JBS_DLL_EXPORT __declspec( dllexport )
#    define JBS_DLL_IMPORT __declspec( dllimport )
#  else
#    define JBS_DLL_EXPORT
#    define JBS_DLL_IMPORT
#  endif
#endif

// [KOMA 2006-10-09: smarter in/export handling on Win32]

#if ! defined( BFC_MT_DLL )
#  if defined( BFC_MT_EXPORTS )
#    define BFC_MT_DLL JBS_DLL_EXPORT
#  else
#    define BFC_MT_DLL JBS_DLL_IMPORT
#  endif
#endif

// ============================================================================

// Now, act as an all-in-one header for this DLL.

//#include "BFC.MT.MyClass.h"

// ============================================================================

#endif // _BFC_MT_DLL_H_

// ============================================================================

