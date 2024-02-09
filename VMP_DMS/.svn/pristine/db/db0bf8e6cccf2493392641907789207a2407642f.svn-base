// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Debug".
// 
// "TBFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Debug.ThreadFinder.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_ThreadFinder_H_
#define _TBFC_Light_ThreadFinder_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Debug.ThreadFinderPtr.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.SObject.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Base class of classes that can return the name of the current
///	working thread.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL ThreadFinder : virtual public SObject {

public :

	/// \brief Creates a new ThreadFinder.

	ThreadFinder(
	);

	/// \brief Destroys this object.

	virtual ~ThreadFinder(
	);

	/// \brief Returns an empty string.

	virtual const Buffer & getThreadName(
	) const;

protected :

private :

	Buffer	noName;		///< Empty string.

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	ThreadFinder(
		const	ThreadFinder&
	);

	ThreadFinder& operator = (
		const	ThreadFinder&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_ThreadFinder_H_

// ============================================================================

