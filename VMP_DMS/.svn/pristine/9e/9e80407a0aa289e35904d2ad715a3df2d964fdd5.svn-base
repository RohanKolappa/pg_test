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
//	TBFC.Base.Property.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_Property_H_
#define _TBFC_Base_Property_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.Base.FastCS.h"

#include "TBFC.TL.Array.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Simple to use Property class, associating a string value to a
///	string tag, with optional value locking.
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL Property {

public :

	/// \brief Creates a new Property.
	///
	/// This new Property has no tag and no value.

	Property(
	);

	/// \brief Creates a new Property whose tag is \a __tag.
	///
	/// This new Property has no value associated to tag \a __tag.
	///
	/// \param __tag
	///	The tag of this new Property.

	Property(
		const	Buffer		& __tag
	);

	/// \brief Creates a new Property whose tag is \a __tag, and whose
	///	value is \a __val.
	///
	/// \param __tag
	///	The tag of this new Property.
	///
	/// \param __val
	///	The value of this new Property. This value is automatically
	///	locked.

	Property(
		const	Buffer		& __tag,
		const	Buffer		& __val
	);

	/// \brief Creates a new Property whose content is copied from
	///	\a __other.
	///
	/// All fields from \a __other are copied (tag, value, AND
	///	value lock flag).
	///
	/// \param __other
	///	The other Property to copy.

	Property(
		const	Property	& __other
	);

	/// \brief Destroys this object.

	~Property(
	);

	/// \brief Replaces the content of this object by a copy of \a __other.
	///
	/// All fields from \a __other are copied (tag, value, AND
	///	value lock flag).
	///
	/// \param __other
	///	The other Property to copy.

	Property & operator = (
		const	Property	& __other
	);

	/// \brief Sets the tag of this object to \a __tag.
	///
	/// No previous tag may already have been set!
	///
	/// \param __tag
	///	The tag to associate to this object.
	///
	/// \throws Locked
	///	If a tag has already been set.

	void setTag(
		const	Buffer		& __tag
	);

	/// \brief Returns the tag associated to this object.

	const Buffer & getTag(
	) const;

	/// \brief Sets the value of this object to \a __val.
	///
	/// The actual value of this object should not be locked.
	///
	/// \param __val
	///	The value to associate to this object.
	///
	/// \throws Locked
	///	If the current value has been locked.

	void setVal(
		const	Buffer		& __val
	);

	/// \brief Definitely locks the value associated to this object.
	///
	/// Once the value has been locked, it won't be possible to unlock
	/// it to modify it later!

	void lockVal(
	);

	/// \brief Returns true iff the current value has been locked.

	Bool isValLocked(
	) const;

	/// \brief Returns the current value associated to this object.

	const Buffer & getVal(
	) const;

	BASE_CLASS_GEN_EXCEPTION( "TBFC.Property" );

	CLASS_EXCEPTION( Locked, "Locked" );

protected :

private :

	mutable FastCS	critSec;	///< Object content protection.
	Buffer		propTag;	///< Property name.
	Buffer		propVal;	///< Property value.
	Bool		valLock;	///< Property value locked ?

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

};

// ============================================================================

/// \brief Array of Property objects.
///
/// \ingroup TBFC_Base

typedef Array< Property > PropertyBag;

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_Property_H_

// ============================================================================

