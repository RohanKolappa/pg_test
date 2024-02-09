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
//	BFC.Base.Aliasing.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_Aliasing_H_
#define _BFC_Base_Aliasing_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Light

template < typename T >
void setValue(
		Uchar *		p,
	const	T		v
) {

	union U {
		Uchar	src[ sizeof( T ) ];
		T	dst;
	} * u = ( U * )p;

	u->dst = v;

}

/// \brief [no brief description]
///
/// \ingroup BFC_Light

template < typename T >
void getValue(
		Uchar *		p,
		T &		v
) {

	union U {
		Uchar	src[ sizeof( T ) ];
		T	dst;
	} * u = ( U * )p;

	v = u->dst;

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_Aliasing_H_

// ============================================================================

