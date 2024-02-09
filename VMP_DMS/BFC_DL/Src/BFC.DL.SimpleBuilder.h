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
//	BFC.DL.SimpleBuilder.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_SimpleBuilder_H_
#define _BFC_DL_SimpleBuilder_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.SimpleBuilderPtr.h"

// ============================================================================

#include "BFC.DL.Builder.h"
#include "BFC.DL.Type.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief Specialized Builder used for classes which implement the default
///	constructor.
///
/// \tparam TObject
///	Name of the class this SimpleBuilder will instantiate. This class
///	has to implement at least the default constructor.
///
/// \ingroup BFC_DL

template < typename TObject >
class SimpleBuilder : public Builder {

public :

	/// \brief Creates a new SimpleBuilder, capable of instantiating the
	///	class described by \a pType.

	SimpleBuilder(
	) :
		Builder	( TObject::getClassType() )
	{
	}

	virtual Bool canMake(
		const	UUId &		pUUId
	) const;

	/// \brief Creates a new object by calling the default constructor of
	///	the template parameter.

	virtual ObjectPtr makeObject(
	);

protected :

private :

	/// \brief Forbidden copy constructor.

	SimpleBuilder(
		const	SimpleBuilder &
	);

	/// \brief Forbidden copy operator.

	SimpleBuilder & operator = (
		const	SimpleBuilder &
	);

};

// ============================================================================

template < typename TObject >
Bool SimpleBuilder< TObject >::canMake(
	const	UUId &		pUUId ) const {

	return ( getType()->getUUId() == pUUId );

}

template < typename TObject >
ObjectPtr SimpleBuilder< TObject >::makeObject() {

	return new TObject;

}

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_SimpleBuilder_H_

// ============================================================================

