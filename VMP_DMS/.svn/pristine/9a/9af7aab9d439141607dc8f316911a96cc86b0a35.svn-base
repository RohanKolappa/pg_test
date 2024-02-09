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
//	BFC.MT.Callable.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Callable_H_
#define _BFC_MT_Callable_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.CallablePtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief Represents a task that returns a result and may throw an exception.
///
/// Implementors define a single method with no arguments called call().
///
/// The Callable interface is similar to Tasklet, in that both are designed for
/// classes whose instances are potentially executed by another thread. A
/// Tasklet, however, does not return a result and cannot throw a checked
/// exception.
///
/// The Executors class contains utility methods to convert from other common
/// forms to Callable classes.
///
/// \ingroup BFC_MT

class BFC_MT_DLL Callable : virtual public SObject {

public :

	/// \brief Computes a result, or throws an exception if unable to do
	///	so.
	///
	/// \returns
	///	Computed result.
	///
	/// \throws Exception
	///	If unable to compute a result.

	virtual SPtr call(
	) = 0;

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Callable_H_

// ============================================================================

