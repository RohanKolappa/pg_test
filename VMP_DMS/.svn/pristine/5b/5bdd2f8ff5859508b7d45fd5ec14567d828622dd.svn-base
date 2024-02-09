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
//	TBFC.TL.ListBase.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_ListBase_H_
#define _TBFC_TL_ListBase_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.FastCS.h"

#include "TBFC.TL.ListCell.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Class implementing most of the behaviour of the List class.
///
/// \ingroup TBFC_TL

class TBFC_Light_DLL ListBase {

public :

	/// \brief Creates a new ListBase.

	ListBase(
	);

	/// \brief Destroys this object.

	virtual ~ListBase(
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

	/// \brief Creates a new Cell.

	ListCell * privGetNew(
			void	* data
	);

	/// \brief Destroys a Cell.

	void privDelete(
			ListCell	* __cell
	);

	/// \brief Removes cell \a __cell from this object.
	///
	/// The 'next' and 'prev' fields are set to 0 in \a __cell.
	///
	/// \returns The successor of \a __cell, if any.

	ListCell * privPop(
			ListCell	* __cell
	);

	/// \brief Inserts \a __newCell before \a __curCell.
	///
	/// If this object is empty, then \a __curCell must be 0. Otherwise,
	/// \a __curCell must point to a valid cell.

	void privPushBefore(
			ListCell	* __curCell,
			ListCell	* __newCell
	);

	/// \brief Inserts \a __newCell after \a __curCell.
	///
	/// If this object is empty, then \a __curCell must be 0. Otherwise,
	/// \a __curCell must point to a valid cell.

	void privPushAfter(
			ListCell	* __curCell,
			ListCell	* __newCell
	);

	ListCell * privGetFirst(
	) const {
		return first;
	}

	ListCell * privGetLast(
	) const {
		return last;
	}

private :

	static Uint32	globalId;	///< Global list id counter.

	FastCS		fastCS;
	Uint32		id;		///< Unique id.
	ListCell	* first;	///< First element.
	ListCell	* last;		///< Last element.
	Uint32		nbr;		///< Number of elements.

	ListBase(
		const	ListBase	& __other
	);

	ListBase & operator = (
		const	ListBase	& __other
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_ListBase_H_

// ============================================================================

