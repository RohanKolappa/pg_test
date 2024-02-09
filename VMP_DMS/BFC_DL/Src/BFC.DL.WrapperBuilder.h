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
//	BFC.DL.WrapperBuilder.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_WrapperBuilder_H_
#define _BFC_DL_WrapperBuilder_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.WrapperBuilderPtr.h"

// ============================================================================

#include "BFC.DL.Builder.h"
#include "BFC.DL.Type.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief Specialized Builder used for classes which wrap some other class.
///
/// \tparam TObject
///	Name of the class this WrapperBuilder will instantiate. This class
///	has to implement a constructor which takes exactly one parameter of
///	a type inheriting ObjectPtr.
///
/// \ingroup BFC_DL

template < typename TObject >
class WrapperBuilder : public Builder {

public :

	/// \brief Creates a new WrapperBuilder, capable of instantiating the
	///	class described by \a pType, and wrapping the class
	///	instantiated by \a pBldr.

	WrapperBuilder(
			BuilderPtr	pBldr
	) :
		Builder	( TObject::getClassType()->clone( pBldr->getType()->getData() ) ),
		wrapped	( pBldr ) {
	}

	/// \brief Returns true if this WrapperBuilder can make object of type
	///	\a pUUId, or if the wrapped builder can make such objects.

	virtual Bool canMake(
		const	UUId &		pUUId
	) const;

	/// \brief Creates a new wrapped object using the wrapped builder, and
	///	creates a new object by calling a 1-parameter constructor of
	///	the template parameter, passing the new wrapped object as
	///	parameter.

	virtual ObjectPtr makeObject(
	);

protected :

private :

	BuilderPtr	wrapped;

	/// \brief Forbidden default constructor.

	WrapperBuilder(
	);

	/// \brief Forbidden copy constructor.

	WrapperBuilder(
		const	WrapperBuilder &
	);

	/// \brief Forbidden copy operator.

	WrapperBuilder & operator = (
		const	WrapperBuilder &
	);

};

// ============================================================================

template < typename TObject >
Bool WrapperBuilder< TObject >::canMake(
	const	UUId &		pUUId ) const {

	return ( getType()->getUUId() == pUUId
		|| wrapped->canMake( pUUId ) );

}

template < typename TObject >
ObjectPtr WrapperBuilder< TObject >::makeObject() {

	return new TObject( wrapped->makeObject() );

}

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_WrapperBuilder_H_

// ============================================================================

