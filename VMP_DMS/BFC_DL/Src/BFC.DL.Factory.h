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
//	BFC.DL.Factory.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Factory_H_
#define _BFC_DL_Factory_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.FactoryPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "BFC.DL.BuilderPtr.h"
#include "BFC.DL.DescrPtr.h"
#include "BFC.DL.ObjectPtr.h"
#include "BFC.DL.TypePtr.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief Base class of all classes capable of instantiating classes
///	inheriting CObject by using a Builder.
///
/// The reason why this class delegates the process of concrete instantiation
/// to some Builder is to permit users to inherit directly from Factory, for
/// example to provide additional meta-information, while still having the
/// choice how to actually build objects. The alternatives would be to inherit
/// from templates classes (like SimpleBuilder), which caused problems on some
/// platforms, or use some diamond inheritance, with all the problems such a
/// solution implies.
///
/// \ingroup BFC_DL

class BFC_DL_DLL Factory : virtual public SObject {

public :

	/// \brief Creates a new Factory, which will delegate the instantiation
	///	process to \a pBuilder.

	Factory(
			BuilderPtr	pBuilder
	) :
		builder	( pBuilder )
	{
	}

	/// \brief Returns the builder associated to this Factory.
	///
	/// \returns
	///	Builder associated to this Factory.

	BuilderPtr getBuilder(
	) const {
		return builder;
	}

	/// \brief Returns the Type of the class instantiated by this
	///	Factory.
	///
	/// \returns
	///	Type descriptor for this Factory.

	virtual const TypeCPtr & getType(
	) const;

	DescrCPtr getDescr(
	) const;

	/// \brief Returns true iff this Factory can instantiate the class
	///	univoquely specified by \a pUUId.

	virtual Bool canMake(
		const	UUId &		pUUId
	) const;

	/// \brief Instantiates the class described by getType().
	///
	/// \returns
	///	The newly instantiated object.

	virtual ObjectPtr makeObject(
	);

protected :

private :

	BuilderPtr	builder;

	/// \brief Forbidden default constructor.

	Factory(
	);

	/// \brief Forbidden copy constructor.

	Factory(
		const	Factory &
	);

	/// \brief Forbidden copy operator.

	Factory & operator = (
		const	Factory &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Factory_H_

// ============================================================================

