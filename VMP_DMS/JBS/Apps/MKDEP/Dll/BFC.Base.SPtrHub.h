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
//	BFC.Base.SPtrHub.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_SPtrHub_H_
#define _BFC_SPtrHub_H_

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

class SObject;

// ============================================================================

class SPtrHub {

public :

	SPtrHub(
			SObject		*o
	);

	virtual ~SPtrHub(
	);

	void incPtr(
		const	Bool			mst
	);

	/// Returns true iff the hub should now be destroyed by the SPtr.

	Bool decPtr(
		const	Bool			mst
	);

	void incMst();
	void decMst();

	/// Returns true iff the hub should now be destroyed by the SObject.

	Bool decObj(
	);

	const char *typeName() const;

	Bool operator == (
		const	SObject	*ptr
	) const {return (t == ptr);}

	Bool operator != (
		const	SObject	*ptr
	) const {return (t != ptr);}

	Bool operator == (
		const	SPtrHub&	o
	) const {return (t == o.t);}

	Bool operator != (
		const	SPtrHub&	o
	) const {return (t != o.t);}

	operator Bool () const {return (t != 0);}

	Bool operator ! () const {return (t == 0);}

	SObject *getRoot() const {return t;}

	Uint32 getRefCount() const {return ca;}
	Uint32 getMstCount() const {return cm;}

protected :

private :

	// internal state...

	SObject			*t;
	Uint32			ca;	// all
	Uint32			cm;	// masters

	static Uint32		currCount;
	static Uint32		peakCount;

	// others...

	SPtrHub(
	);

	SPtrHub(
		const	SPtrHub&
	);

	SPtrHub& operator = (
		const	SPtrHub&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_SPtrHub_H_

// ============================================================================

