// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	Sys.MT.ThreadData.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _Sys_MT_ThreadData_H_
#define _Sys_MT_ThreadData_H_

// ============================================================================

#include "Sys.MT.DLL.h"

// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <windows.h>
#else
#include <pthread.h>
#endif

// ============================================================================

namespace Sys {
namespace MT {

// ============================================================================

/// \brief System-specific thread data.
///
/// \ingroup Sys_MT

class Sys_MT_DLL ThreadData {

public :

	ThreadData(
	);

	~ThreadData(
	);

#if defined( PLATFORM_WIN32 )
	HANDLE			id;	///< System-specific thread handle.
#else
	pthread_t		id;	///< System-specific thread handle.
	pthread_attr_t		attr;	///< System-specific thread attributes.
#endif

	int			pid;	///< Process id.
	int			tid;	///< Thread id.

};

// ============================================================================

} // namespace MT
} // namespace Sys

// ============================================================================

#endif // _Sys_MT_ThreadData_H_

// ============================================================================

