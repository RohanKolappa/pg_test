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
//	TBFC.TL.ArrayBase.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_ArrayBase_H_
#define _TBFC_TL_ArrayBase_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Types.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Class implementing most of the behaviour of the Array class.
///
/// \ingroup TBFC_TL

class TBFC_Light_DLL ArrayBase {

public :

	/// \brief Creates a new ArrayBase of size \a __size.

	ArrayBase(
		const	Uint32		__size
	);

	/// \brief Destroys this ArrayBase.

	virtual ~ArrayBase(
	);

	/// \brief Returns true iff this object is empty.

	Bool isEmpty(
	) const {
		return ( nbr == 0 );
	}

	/// \brief Returns the number of elements in this object.

	Uint32 getSize(
	) const {
		return nbr;
	}

	/// \brief Synonym of getSize().
	///
	/// This method is kept to maintain backward compatibility only.
	/// Don't use it anymore.

	Uint32 size(
	) const {
		return getSize();
	}

protected :

	void checkIndex(
		const	Uint32		__idx,
		const	Bool		__incl
	) const;

	static Uint32 getLen(
		const	Uint32		__nbr
	);

	void insertHole(
		const	Uint32		__idx,
		const	Uint32		__len
	);

	void deleteHole(
		const	Uint32		__idx,
		const	Uint32		__len
	);

	void **		ptr;	///< Pointers array.
	Uint32		nbr;	///< Number of elements.
	Uint32		len;	///< Length of 'ptr'.

private :

	ArrayBase(
	);

	ArrayBase(
		const	ArrayBase	& __other
	);

	ArrayBase & operator = (
		const	ArrayBase	& __other
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_ArrayBase_H_

// ============================================================================

