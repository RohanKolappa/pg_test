// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.SPtrHub.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_SPtrHub_H_
#define _BFC_Base_SPtrHub_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.FastCS.h"

// ============================================================================

namespace BFC {

// ============================================================================

class SObject;

// ============================================================================

/// \brief Proxy for a SObject, shared by possibly multiple SPtr objects.
///
/// A SPtrHub records how many SPtr objects currently point to it (reference
/// counting).
///
/// You should never have to create a SPtrHub yourself, as one is always
/// automatically created in the constructor of the SObject!
///
/// \ingroup BFC_Base

class BFC_Light_DLL SPtrHub {

public :

	/// \brief Creates a new SPtrHub, pointing to the smart object
	///	\a pPtrObj.
	///
	/// The counters of SPtr objects are set to 0.

	SPtrHub(
			SObject *	pPtrObj
	) :
#if defined( PLATFORM_LINUX )
		dmngle	( 0 ),
#endif
		ptrObj	( pPtrObj ),
		nbrAll	( 0 ),
		nbrMst	( 0 ),
		thrSfe	( true ) {
	}

#if defined( PLATFORM_LINUX )

	/// \brief Destroys this object.

	~SPtrHub(
	);

#endif

	/// \brief Increments the number of SPtr objects
	///	and, if \a mst is true, the number of Master SPtr
	///	objects.

	void incPtr(
		const	Bool		mst
	);

	/// \brief Decrements the number of SPtr objects and, if \a mst is
	///	true, the number of Master SPtr objects.
	///
	/// If \a mst is true and if the number of Master SPtr objects reaches
	/// 0, then the SObject pointed to by this hub (if any) is destroyed.
	///
	/// \param mst
	///	Indicated whether the call is originating from a Master SPtr
	///	(true) or a Slave SPtr (false).
	///
	/// \returns True iff the hub should be destroyed by the SPtr calling
	///	this method. Note that in this case, the SObject has already
	///	been destroyed! What's left is an "empty" hub!

	Bool decPtr(
		const	Bool		mst
	);

	/// \brief Increments the number of Master SPtr objects.

	void incMst(
	);

	/// \brief Decrements the number of Master SPtr objects.

	void decMst(
	);

	/// \brief Detaches this object from the SObject it points to (if any).
	///
	/// This method should only be called from the SObject's destructor.
	///
	/// \returns True iff the hub isn't used anymore and should be
	///	destroyed by the SObject.

	Bool decObj(
	);

	/// \brief Returns the RTTI typeid of the SObject pointed to by this
	///	hub, or NULL if there is no such object.

	const char * typeName(
	) const;

	/// \brief Returns true iff this hub points to SObject \a ptr.

	Bool operator == (
		const	SObject *	ptr
	) const {
		return ( ptrObj == ptr );
	}

	/// \brief Returns true iff this hub doesn't point to SObject \a ptr.

	Bool operator != (
		const	SObject *	ptr
	) const {
		return ( ptrObj != ptr );
	}

	/// \brief Returns true iff this hub points to the same SObject than
	///	\a other.

	Bool operator == (
		const	SPtrHub &	other
	) const {
		return ( ptrObj == other.ptrObj );
	}

	/// \brief Returns true iff this hub points to another SObject than
	///	\a other.

	Bool operator != (
		const	SPtrHub &	other
	) const {
		return ( ptrObj != other.ptrObj );
	}

	/// \brief Returns true iff this hub points to a SObject.

	operator Bool (
	) const {
		return ( ptrObj != 0 );
	}

	/// \brief Returns true iff this hub doesn't point to any SObject.

	Bool operator ! (
	) const {
		return ( ptrObj == 0 );
	}

	/// \brief Returns the SObject this hub points to.

	SObject * getRoot(
	) const {
		return ptrObj;
	}

	/// \brief Returns the number of SPtr objects pointing to this hub.

	Uint32 getRefCount(
	) const {
		return nbrAll;
	}

	/// \brief Returns the number of Master SPtr objects pointing to this
	///	hub.

	Uint32 getMstCount(
	) const {
		return nbrMst;
	}

	Bool isThreadSafe(
	) const {
		return thrSfe;
	}

	Bool setThreadSafe(
		const	Bool		sfe
	);

protected :

private :

#if defined( PLATFORM_LINUX )
	mutable char *	dmngle;		///< Allocated by typename demangler.
#endif
	SObject *	ptrObj;		///< SObject we point to.
	Uint32		nbrAll;		///< Number of SPtr objects.
	Uint32		nbrMst;		///< Number of Master SPtr objects.
	Bool		thrSfe;		///< Thread safe.
	FastCS		fastCS;	 	///< Local data protection.

	/// \brief Forbidden default constructor.

	SPtrHub(
	);

	/// \brief Forbidden copy constructor.

	SPtrHub(
		const	SPtrHub &
	);

	/// \brief Forbidden copy operator.

	SPtrHub & operator = (
		const	SPtrHub &
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_SPtrHub_H_

// ============================================================================

