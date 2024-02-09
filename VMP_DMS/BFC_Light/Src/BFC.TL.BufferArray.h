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
//	BFC.TL.BufferArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_BufferArray_H_
#define _BFC_TL_BufferArray_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A CompArray of Buffer objects.
///
/// \ingroup BFC_TL

class BFC_Light_DLL BufferArray : public CompArray< Buffer > {

public :

	/// \brief Creates a new BufferArray of size \a pSize (default 0).

	BufferArray(
		const	Uint32		pSize = 0
	) : CompArray< Buffer >( pSize ) {
	}

	/// \brief Creates a new BufferArray, filled with strings from the
	///	NULL terminated string array \a pTable.
	///
	/// The strings are NOT copied!!! See Buffer( const char * ) and
	/// friends.

	BufferArray(
		const	Char **		pTable
	);

	/// \brief Creates a new BufferArray, filled with the first \a pNum
	///	strings from the string array \a pTable.

	BufferArray(
		const	Uint32		pNum,
		const	Char **		pTable
	);

	/// \brief Creates a new BufferArray, filled with the first \a pNum
	///	strings from the string array \a pTable.

	BufferArray(
			int		pNum,
			char **		pTable
	);

	BufferArray(
		const	CompArray< Buffer > &	pOther
	) :
		CompArray< Buffer >( pOther ) {
	}

	BufferArray(
		const	BufferArray &	pOther
	) :
		SObject(),
		CompArray< Buffer >( pOther ) {
	}

	BufferArray & operator = (
		const	BufferArray &	pOther
	) {
		CompArray< Buffer >::operator = ( pOther );
		return *this;
	}

	Buffer join(
		const	Uchar		pChar = ' '
	) const;

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_BufferArray_H_

// ============================================================================

