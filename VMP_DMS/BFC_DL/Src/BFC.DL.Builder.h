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
//	BFC.DL.Builder.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Builder_H_
#define _BFC_DL_Builder_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.BuilderPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "BFC.DL.ObjectPtr.h"
#include "BFC.DL.TypePtr.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief Abstract base class of all classes capable of instantiating classes
///	inheriting Object.
///
/// The following piece of code illustrates how to use this class.
/*!
\code
	// Implement a base class...

	class BaseClass : virtual public DL::Object {
	public :
		BaseClass(
		);
	};

	extern const TypeCPtr baseType;

	DL::BuilderPtr baseBuilder = new DL::SimpleBuilder< BaseClass >(
			baseType );

	// Implement a wrapper class...

	class WrapperClass : virtual public DL::Object {
	public :
		WrapperClass(
				BaseClassPtr	pBaseObject
		);
	};

	extern const DL::TypeCPtr wrapperType;

	DL::BuilderPtr wrapperBuilder = new DL::WrapperBuilder< WrapperClass >(
			wrapperType,
			baseBuilder );

	// Now, instantiate objects...

	DL::ObjectPtr wrapperObject = wrapperBuilder->makeObject();

\endcode
*/
/// \ingroup BFC_DL

class BFC_DL_DLL Builder : virtual public SObject {

protected :

	/// \brief Creates a new Builder, capable of instantiating the class
	///	described by \a pType.

	Builder(
		const	TypeCPtr &	pType
	) :
		type	( pType )
	{
	}

public :

	/// \brief Returns the Type of the class instantiated by this
	///	Builder.
	///
	/// \returns
	///	Type descriptor for this Builder.

	const TypeCPtr & getType(
	) const {
		return type;
	}

	/// \brief Returns true iff this Builder can instantiate the class
	///	univoquely specified by \a pUUId.

	virtual Bool canMake(
		const	UUId &		pUUId
	) const = 0;

	/// \brief Instantiates the class described by getType().
	///
	/// \returns
	///	The newly instantiated object.

	virtual ObjectPtr makeObject(
	) = 0;

protected :

private :

	TypeCPtr	type;	///< Type descriptor.

	/// \brief Forbidden default constructor.

	Builder(
	);

	/// \brief Forbidden copy constructor.

	Builder(
		const	Builder &
	);

	/// \brief Forbidden copy operator.

	Builder & operator = (
		const	Builder &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Builder_H_

// ============================================================================

