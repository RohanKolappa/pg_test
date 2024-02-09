// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "TBFC::Light".
//
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
// 
// Filename:
//	TBFC.TL.Uint32Array.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_Uint64Array_H_
#define _TBFC_TL_Uint64Array_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {

TBFC_PTR_DECL( TBFC_Light_DLL, Uint64Array )

} // namespace TBFC

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"

// ============================================================================

namespace TBFC {

// ============================================================================

class TBFC_Light_DLL Uint64Array : virtual public SObject {

public :

	Uint64Array(const Uint32 = 0);
	Uint64Array(const Uint64Array&);
	virtual ~Uint64Array();
	virtual Uint64Array& operator = (const Uint64Array&);
	virtual void operator += (const Uint64 s) {
		resize(user + 1);
		data[user - 1] = s;
	}
	virtual void pop(const Uint32 index);
	virtual void insert(const Uint32 index, const Uint64 value);
	virtual void kill();
	virtual void resize(const Uint32);
	virtual Bool isEmpty() const {return (user == 0);}
	virtual Uint32 size() const {return user;}
	virtual Uint64& operator [] (const Uint32 i) {return elt(i);}
	virtual Uint64 operator [] (const Uint32 i) const {return elt(i);}
	virtual Uint64& elt(const Uint32 l) {
		if (l >= user) {
			throw IndexOutOfRange();
		}
		return data[l];
	}
	virtual Uint64 elt(const Uint32 l) const {
		if (l >= user) {
			throw IndexOutOfRange();
		}
		return data[l];
	}

	BASE_CLASS_GEN_EXCEPTION("Uint64Array");

	CLASS_EXCEPTION(IndexOutOfRange, "Index out of range");
	CLASS_EXCEPTION(Overflow, "Overflow");

protected :

	Uint64		*data;	// data
	Uint32		phys;	// allocated and pointed to by 'data'
	Uint32		user;	// visible by the user

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_Uint64Array_H_

// ============================================================================

