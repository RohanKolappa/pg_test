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
//	BFC.DL.Counter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Counter_H_
#define _BFC_DL_Counter_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.CounterPtr.h"

// ============================================================================

#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL Counter : virtual public SObject {

public :

	/// \brief Creates a new Counter, with initial value set to 0.

	Counter(
	) :
		val	( 0 )
	{
	}

	/// \brief Atomically increments this object.

	void inc(
	);

	/// \brief Atomically decrements this object.

	void dec(
	);

	/// \brief Returns the value of this object.

	Uint32 get(
	) {
		return val;
	}

protected :

private :

	FastCS	cs;	///< Access serializer.
	Uint32	val;	///< Counter value.

	/// \brief Forbidden copy constructor.

	Counter(
		const	Counter &
	);

	/// \brief Forbidden copy operator.

	Counter & operator = (
		const	Counter &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Counter_H_

// ============================================================================

