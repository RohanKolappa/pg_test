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
//	BFC.DL.Type.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Type_H_
#define _BFC_DL_Type_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.TypePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "BFC.DL.DescrPtr.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL Type : virtual public SObject {

public :

	/// \brief Creates a new Type.

	Type(
		const	UUId &		pUUId,
		const	Buffer &	pName,
		const	TypeCPtr &	pPrnt,
		const	DescrCPtr &	pData = DescrCPtr()
	) :
		uuid	( pUUId ),
		name	( pName ),
		prnt	( pPrnt ),
		data	( pData )
	{
	}

	TypeCPtr clone(
		const	DescrCPtr &	pData = DescrCPtr()
	) const {
		return new Type(
			uuid,
			name,
			prnt,
			pData ? pData : data );
	}

	Bool inherits(
		const	TypeCPtr &	pOther
	) const;

	const UUId & getUUId(
	) const {
		return uuid;
	}

	const Buffer & getName(
	) const {
		return name;
	}

	const TypeCPtr & getParent(
	) const {
		return prnt;
	}

	const DescrCPtr & getData(
	) const {
		return data;
	}

protected :

private :

	UUId		uuid;	///< Class UUId.
	Buffer		name;	///< Class name.
	TypeCPtr	prnt;	///< Class parent.
	DescrCPtr	data;	///< Extra class metadata.

	/// \brief Forbidden default constructor.

	Type(
	);

	/// \brief Forbidden copy constructor.

	Type(
		const	Type &
	);

	/// \brief Forbidden copy operator.

	Type & operator = (
		const	Type &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Type_H_

// ============================================================================

