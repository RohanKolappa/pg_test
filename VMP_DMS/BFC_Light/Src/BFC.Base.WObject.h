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
//	BFC.Base.WObject.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_WObject_H_
#define _BFC_Base_WObject_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtrTmpl.h"

// ============================================================================

namespace BFC {

BFC_PTR_TMPL( WObject )

} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Base

template < typename T >
class WObject : virtual public SObject {

public :

	WObject(
	) {
	}

	WObject(
		const	WObject< T > &	pOther
	) :
		t	( pOther.t )
	{
	}

	WObject(
		const	T &		pOther
	) :
		t	( pOther )
	{
	}

	WObject< T > & operator = (
		const	WObject< T > &	pOther
	) {
		t = pOther.t;
	}

	WObject< T > & operator = (
		const	T &		pOther
	) {
		t = pOther;
	}

	operator T & (
	) {
		return t;
	}

	operator const T & (
	) const {
		return t;
	}

protected :

private :

	T	t;

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_WObject_H_

// ============================================================================

