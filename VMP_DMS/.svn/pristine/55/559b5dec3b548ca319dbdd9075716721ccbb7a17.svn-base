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
//	TBFC.Base.PropertyUser.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_PropertyUser_H_
#define _TBFC_Base_PropertyUser_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.Property.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Class inherited by all classes that use a PropertyBag.
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL PropertyUser {

public :

	/// \brief Creates a new PropertyUser, whose property bag is empty.

	PropertyUser(
	) {
	}

	/// \brief Creates a new PropertyUser, using a copy of the property
	///	bag of \a __other.

	PropertyUser(
		const	PropertyUser	& __other
	) : propertyBag( __other.propertyBag ) {
	}

	/// \brief Destroys this object.

	virtual ~PropertyUser(
	);

	/// \brief Replaces the property bag of this object by a copy of the
	///	property bag of \a __other.

	PropertyUser & operator = (
		const	PropertyUser	& __other
	) {
		if ( this != & __other ) {
			propertyBag = __other.propertyBag;
		}
		return * this;
	}

	/// \brief Returns the value of the property whose tag is
	///	\a __propertyName.
	///
	/// \throws NoSuchProperty
	///	If there is no property tagged \a __propertyName.

	const Buffer & getProperty(
		const	Buffer		& __propertyName
	) const;

	/// \brief Sets the value of the property whose tag is
	///	\a __propertyName to \a __propertyValue.
	///
	/// \throws NoSuchProperty
	///	If there is no property tagged \a __propertyName.
	///
	/// \throws Property::Locked
	///	If the value of property \a __propertyName has been locked.

	void setProperty(
		const	Buffer		& __propertyName,
		const	Buffer		& __propertyValue
	);

	/// \brief Returns a read-only reference to the property bag of this
	///	object.

	const PropertyBag & getPropertyBag(
	) const {
		return propertyBag;
	}

	/// \brief Returns a read-write reference to the property bag of this
	///	object.

	PropertyBag & getPropertyBag(
	) {
		return propertyBag;
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "TBFC.PropertyUser" );

	CLASS_EXCEPTION( NoSuchProperty, "No such property" );

protected :

private :

	PropertyBag	propertyBag;	///< Perperty bag.

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_PropertyUser_H_

// ============================================================================

