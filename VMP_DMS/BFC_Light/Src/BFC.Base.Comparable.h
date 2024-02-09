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
//	BFC.Base.Comparable.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_Comparable_H_
#define _BFC_Base_Comparable_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.ComparablePtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Interface implemented by classes having a total ordering.
///
/// This interface imposes a total ordering on the objects of each class that
/// implements it. This ordering is referred to as the class's <i>natural
/// ordering</i>, and the class's <tt>compareTo</tt> method is referred to as
/// its <i>natural comparison method</i>.<p>
///
/// \ingroup BFC_Base

class BFC_Light_DLL Comparable : virtual public SObject {

public :

	/// \brief Compares this object with \a pOther for order.
	/// 
	/// The implementor must ensure <tt>sgn( x->compareTo( y ) ) ==
	/// -sgn( y->compareTo( x ) )</tt> for all <tt>x</tt> and <tt>y</tt>.
	/// This implies that <tt>x->compareTo( y )</tt> must throw an
	/// exception iff <tt>y->compareTo( x )</tt> throws an exception.
	/// 
	/// The implementor must also ensure that the relation is transitive:
	/// <tt>( x->compareTo( y ) > 0 && y->compareTo( z ) > 0 )</tt> implies
	/// <tt>x->compareTo( z ) > 0</tt>.
	/// 
	/// Finally, the implementor must ensure that <tt>x->compareTo( y ) ==
	/// 0 </tt> implies that <tt>sgn( x->compareTo( z ) ) == sgn(
	/// y->compareTo( z ) )</tt>, for all <tt>z</tt>.
	/// 
	/// \param pOther
	///	The object to be compared.
	///
	/// \returns
	///	A negative integer, zero, or a positive integer as this object
	///	is less than, equal to, or greater than \a pOther.
	///
	/// \throws NullPointer
	///	If \a pOther is 0.
	///
	/// \throws BadCast
	///	If \a pOther's type prevents it from being compared to this object.

	virtual Int32 compareTo(
			SCPtr		pOther
	) const = 0;

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_Comparable_H_

// ============================================================================

