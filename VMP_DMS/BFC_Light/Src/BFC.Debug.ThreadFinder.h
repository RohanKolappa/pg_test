// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Debug".
// 
// "BFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Debug.ThreadFinder.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_ThreadFinder_H_
#define _BFC_Light_ThreadFinder_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Debug.ThreadFinderPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Base class of classes that can return the name of the current
///	working thread.
///
/// \ingroup BFC_Light

class BFC_Light_DLL ThreadFinder : virtual public SObject {

public :

	/// \brief Returns an empty string.

	virtual const Buffer & getThreadName(
	) const;

	virtual Bool selfShouldStop(
	) const;

protected :

private :

	Buffer	noName;		///< Empty string.

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Light_ThreadFinder_H_

// ============================================================================

