// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.Mem.DataProxy.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_DataProxy_H_
#define _BFC_Mem_DataProxy_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.FastCS.h"

#include "BFC.Mem.Data.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Acts as a proxy for a Data object, counting how many branches
///	(i.e. users) are actually attached to it.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL DataProxy {

public :

	/// \brief Creates a new DataProxy object, attached to the data block
	///	pointed to by \a __data.
	///
	/// The counter of branches (i.e. users) is initially set to 1!

	DataProxy(
			Data *		__data
	) :
		dataPtr( __data ),
		counter( 1 ) {
	}

	/// \brief Destroys this object, after having deleted the data block
	///	this object was attached to.

	/* virtual */ ~DataProxy(
	);

	/// \brief Resizes the data block pointed to by this DataProxy object.
	///
	/// If there is only one branch using this
	/// DataProxy object, then the Data object pointed to by this DataProxy
	/// object is resized to the new length \a __newLen.
	///
	/// If there are 2 or more branches using this DataProxy object, then
	/// this object is first cloned (resulting in a deep copy of the Data
	/// being made), and then this cloned version is resized to the new
	/// length \a __newLen.
	///
	/// If a clone version is made, then the counter of the original (this
	/// object) is first decremented, then the counter of the clone is
	/// incremented.
	///
	/// \returns The resulting resized DataProxy object.

	DataProxy * resize(
		const	Uint32		__newLen
	);

	/// \brief Moves or copies \a __srcLength bytes starting at
	///	\a __srcOffset to \a __dstOffset.
	///
	/// In any case, the returned block should be at
	/// least \a __minLength bytes long.

	DataProxy * memmove(
		const	Uint32		__dstOffset,
		const	Uint32		__srcOffset,
		const	Uint32		__srcLength,
		const	Uint32		__minLength
	);

	/// \brief Increments the number of users.

	void incCounter(
	);

	/// \brief Decrements the number of users, and returns the new value
	///	of this counter.

	Uint32 decCounter(
	);

	/// \brief Returns the actual number of users.

	Uint32 getCounter(
	) {
		return counter;
	}

	/// \brief Called when a branch wants to get exclusive access to
	///	this object, probably to modify the data it points to.
	///
	/// If this structure has to be cloned because there are more than
	/// one branch using it, then the following occurs atomically:
	/// -	the number of branches using this object is decremented
	/// -	a new DataProxy object is created, which uses a deep copy of our
	///	Data object
	/// -	this new DataProxy object has its number of branches incremented
	/// -	the newly created DataProxy object is returned.
	///
	/// Otherwise, if the data has no write access, then the Data object
	/// is converted to a writeable object, and this object is returned.
	///
	/// Otherwise, nothing happens, and this object is returned.
	///
	/// In the first 2 cases, the address of the data may have
	/// changed, so any previous cached pointers should not be relied
	/// upon after having called this method.
	///
	/// \returns A pointer to a DataProxy object which is writeable, and is
	///	used by no one else than the caller of this method.

	DataProxy * ensureSingleUser(
	);

	/// \brief Ensures that the data block pointed to by this object is
	///	writeable.
	///
	/// If the data block is writeable, nothing happens, and this object is
	/// returned.
	///
	/// Otherwise, if this object is shared (i.e. there are 2 or more
	/// users), then a deep copy of this object is created, and the copy is
	/// returned.
	///
	/// Otherwise, the data block is made writeable, and this object is
	/// returned.

	DataProxy * ensureLocalCopy(
	);

	/// \brief Returns the logical length of the data block.

	Uint32 getLogLength(
	) const {
		return ( dataPtr->getLogLength() );
	}

	/// \brief Returns the physical length of the data block.

	Uint32 getPhyLength(
	) const {
		return ( dataPtr->getPhyLength() );
	}

	/// \brief Returns the start address of the data block as a read only
	///	pointer.

	const Uchar * getPhyCstPtr(
	) const {
		return ( dataPtr->getPhyCstPtr() );
	}

	/// \brief Returns the start address of the data block as a writeable
	///	pointer.
	///
	/// Don't call this method if you're not the only user of this proxy.

	Uchar * getPhyVarPtr(
	) {
		return ( dataPtr->getPhyVarPtr() );
	}

	AllocatorPtr releaseData(
			Uchar * &	__phyVarPtr,
			Uint32 &	__phyLength,
		const	Bool		__retrievable
	);

protected :

private :

	FastCS		critSec;	///< counter protection
	Data *		dataPtr;	///< memory block
	Uint32		counter;	///< total number of users

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DataProxy(
	);

	DataProxy(
		const	DataProxy		&
	);

	DataProxy& operator = (
		const	DataProxy		&
	);

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_DataProxy_H_

// ============================================================================

