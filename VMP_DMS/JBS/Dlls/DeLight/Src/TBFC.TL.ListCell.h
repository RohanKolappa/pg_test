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
//	TBFC.TL.ListCell.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_ListCell_H_
#define _TBFC_TL_ListCell_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Types.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Class implementing a cell of a doubly linked list.
///
/// \ingroup TBFC_TL

class TBFC_Light_DLL ListCell {

public :

	ListCell(
		const	Uint32		__list,
			void		* __data
	) : list( __list ), data( __data ), prev( 0 ), next( 0 ) {
	}

	~ListCell(
	) {
	}

	Uint32		list;		///< Unique id of list.
	void		* data;		///< Data pointer.
	ListCell	* prev;		///< Previous element in linked list
	ListCell	* next;		///< Next element in linked list

private :

	/// \brief Forbidden default constructor.

	ListCell(
	);

	/// \brief Forbidden copy constructor.

	ListCell(
		const	ListCell	&
	);

	/// \brief Forbidden copy operator.

	ListCell& operator = (
		const	ListCell	&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_ListCell_H_

// ============================================================================

