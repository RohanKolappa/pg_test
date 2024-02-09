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
// FileName:
//	BFC.Common.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Common_H_
#define _BFC_Common_H_

// ============================================================================

/// \defgroup Common BFC (JFG's Foundation Classes) Library
///
/// \brief A set of base classes (foundation classes).
///
/// The BFC library is made up of several modules.
/// - Base module: Base classes used almost everywhere.
/// - Containers module: STL-like container classes.
/// - Messages module: messaging system classes (producer, consumer, ...).
/// - I/O module: stream-oriented classes.
/// - Multi-threading module: classes to implement multi-threaded apps.
///
/// All those modules are fully described somewhere else.
///
/// Note for the reader: you should start by reading the documentation
/// coming with the Base module, then follow by reading the Multi-threading
/// module, and terminate by the Containers, Messages, and IO modules.

// ============================================================================

/// \defgroup Common_Base Base module.
///
/// \ingroup Common
///
/// \brief Base classes used almost everywhere.
///
/// The Base module provides the elements on which other modules of the
/// BFC library are built.
///
/// All classes and types defined in the Base module fall in one of the
/// following categories:
/// -	Simple types:
///	-	BFC::Uint32, BFC::Int16, ...
///	-	BFC::Bool
///	-	BFC::Uchar
///	-	BFC::Double
///	-	BFC::Buffer
/// -	C++ language related features:
///	-	BFC::Exception : base class of all BFC exceptions
/// -	Smart pointers:
///	-	BFC::SObject
///	-	BFC::SPtr
///	-	BFC::SPtrHub
/// -	Run-time debugging:
///	-	BFC::DObject
///	-	BFC::Debugger
///	-	BFC::ColoredBuffer
///	-	BFC::Console
///	-	BFC::ConsoleColor
/// -	Time management:
///	-	BFC::Time::Clock
///	-	BFC::Time::Delta
///	-	BFC::Time
/// -	Tools:
///	-	BFC::BufferParser
///	-	BFC::Math
///	-	BFC::Utils
///	-	BFC::System
///	-	BFC::Log
///
/// To help create Dynamic Libraries (DLL), BFC also defines 2 macros
/// (JBS_DLL_EXPORT and JBS_DLL_IMPORT). See relevant portion of the documentation
/// of the BFC.Base.Types.h main header file.

// ============================================================================

/// \defgroup Common_Containers STL-like containers module.
///
/// \ingroup Common
///
/// \brief STL-like container classes (and other templates).

// ============================================================================

/// \defgroup Common_Messages Messages module.
///
/// \ingroup Common
///
/// \brief Messaging system classes.

// ============================================================================

/// \defgroup Common_IO Streaming I/O module.
///
/// \ingroup Common
///
/// \brief Classes providing a stream-oriented I/O layer.

// ============================================================================

/// \defgroup Common_Threads Multi-threading module.
///
/// \ingroup Common
///
/// \brief Classes to implement multi-threaded applications.
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
/// The synchronization classes are organized in 4 layers, ranging from the
/// most simple, to the most complex:
/// - OS layer: this layer contains classes wrapping the objects and functions
///	provided by the OS in compact C++ classes:
///	- BFC::SysSyncEvent: (Win32 only) A generic synchronization event.
///	- BFC::SysCS: (Win32 only) A critical section.
///	- BFC::SysMutex: (Linux only) A MUTual EXclusion object.
///	- BFC::SysCondition: (Linux only) A condition variable.
///	- BFC::SysSemaphore: (Linux only) A semaphore.
/// - Emulation layer: this layer implements an OS-independent version of each
///	of the classes found in the OS layer, using the direct implementation
///	found in this OS layer, of emulating it using some other classes:
///	- BFC::EmulSynchronizer: A generic synchronization object.
///	- BFC::EmulMutex: A MUTual EXclusion object.
///	- BFC::EmulCondition: A condition variable.
///	- BFC::EmulSemaphore: A semaphore
///	- BFC::EmulCS: A fast critical section.
///	.
///	Note that all those objects are NOT thread-aware, meaning that their
///	blocking functions (mainly all the lock() and wait() methods) CANNOT BE
///	interrupted using the BFC::MT::Thread::stop() method.
///	You should avoid using those classes!
/// - Safe layer: this layer provides thread-aware versions of some of the
///	classes above:
///	- BFC::MT::Mutex
///	- BFC::MT::Condition
///	.
///	The blocking methods of those classes throw the ThreadStopped
///	exception when the thread of execution they are called from is being
///	stopped using the BFC::MT::Thread::stop() method.
///	Additionnally, all methods perform error checking (owner for mutex,
///	...).

// ============================================================================

#endif // _BFC_Common_H_

// ============================================================================

