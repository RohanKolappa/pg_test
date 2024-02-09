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
//	BFC.Base.SObject.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_SObject_H_
#define _BFC_SObject_H_

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

class SPtrHub;

// ============================================================================

/// Base class of all objects pointed to by smart pointers (see SPtr).

/// An 'SObject' ('S' stands for 'Smart') is the base class a SPtr
/// object can point to.
///
/// \ingroup Common_Base

class SObject {

public :

	SObject(
	);

	virtual ~SObject(
	);

protected :

	Uint32 getRefCount() const;
	Uint32 getMstCount() const;

private :

	friend class SPtr;

	SPtrHub *getHub(
	);

	SPtrHub	*sPtrHub;

	static Uint32		currCount;
	static Uint32		peakCount;

	SObject(const SObject&);
	SObject& operator = (const SObject&);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_SObject_H_

// ============================================================================


