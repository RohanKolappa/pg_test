// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::Light".
//
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
// 
// Filename:
//	BFC.TL.Uint32Array.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_Uint32Array_H_
#define _BFC_TL_Uint32Array_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {

BFC_PTR_DECL( BFC_Light_DLL, Uint32Array )

} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

class BFC_Light_DLL Uint32Array : virtual public SObject {

public :

	Uint32Array(const Uint32 = 0);
	Uint32Array(const Uint32Array&);
	virtual ~Uint32Array();
	Uint32Array& operator = (const Uint32Array&);

	void add(
		const	Uint32		s
	) {
		resize( user + 1 );
		data[ user - 1 ] = s;
	}

	void operator += (
		const	Uint32		s
	) {
		add( s );
	}

	void operator += (
		const	Uint32Array &	pTbl
	) {
		for ( Uint32 i = 0 ; i < pTbl.getSize() ; i++ ) {
			add( pTbl[ i ] );
		}
	}

	void pop(const Uint32 index);
	void insert(const Uint32 index, const Uint32 value);
	void kill();
	void resize(const Uint32);
	Bool isEmpty() const {return (user == 0);}
	Uint32 size() const {return user;}

	Uint32 getSize(
	) const {
		return user;
	}

	Uint32 elt(
		const	Uint32		l
	) const {
		if (l >= user) {
			throw IndexOutOfRange();
		}
		return data[l];
	}

	Uint32 & elt(
		const	Uint32		l
	) {
		if (l >= user) {
			throw IndexOutOfRange();
		}
		return data[l];
	}

	Uint32 operator [] (
		const	Uint32		i
	) const {
		return elt( i );
	}

	Uint32 & operator [] (
		const	Uint32		i
	) {
		return elt( i );
	}

	Uint32 getElt(
		const	Uint32		i
	) const {
		return elt( i );
	}

	Uint32 & getElt(
		const	Uint32		i
	) {
		return elt( i );
	}

	BASE_CLASS_GEN_EXCEPTION("Uint32Array");

	CLASS_EXCEPTION(IndexOutOfRange, "Index out of range");
	CLASS_EXCEPTION(Overflow, "Overflow");

protected :

	Uint32 *	data;	// data
	Uint32		phys;	// allocated and pointed to by 'data'
	Uint32		user;	// visible by the user

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_Uint32Array_H_

// ============================================================================

