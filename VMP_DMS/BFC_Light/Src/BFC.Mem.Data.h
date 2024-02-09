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
//	BFC.Mem.Data.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_Data_H_
#define _BFC_Mem_Data_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Mem.AllocatorPtr.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Represents a memory block, which can be at least accessed in
///	read-only mode, but can also optionally be accessed in read-write
///	mode.
///
/// An instance of this class represents a chunk of contiguous bytes in memory.
///
/// Those bytes can always be accessed in read-only mode, and optionally in
/// read-write mode, depending on the way the object was created.
///
/// The 4 possible methods that are available to create a new Data object are:
/// -	create an empty Data (not associated memory block),
/// -	attach a new Data to a read-only memory block,
/// -	attach a new Data to a pre-allocated writeable memory block,
/// -	let the new Data allocate a new memory block itself.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Data {

public :

	/// \brief Creates a new empty memory block.
	///
	/// If memory has to be allocated (see resize()) later, then a suitable
	/// allocator will be chosen at that time.

	Data(
	);

	/// \brief Creates a new memory block which is at least \a pLength bytes
	///	long.
	///
	/// This constructor creates a new memory block and tries to allocate
	/// at least \a pLength bytes, using the preferred Allocator for this
	/// particular block size.
	///
	/// \param pLength
	///	The requested minimum block size. The actual number of bytes
	///	that were allocated is returned by getPhyLength(), while
	///	the value of this parameter is returned by getLogLength().
	///
	/// \throws OutOfMemory
	///	If the memory allocation failed.

	Data(
		const	Uint32		pLength
	);

	/// \brief Attaches a new Data object to some read-only memory block.
	///
	/// This constructor creates a new Data object, and attaches it to
	/// some read-only memory block, starting at address \a pCstPtr, and
	/// which is logically pLogLen bytes long, and physically \a pPhyLen
	/// bytes long.
	///
	/// \param pCstPtr
	///	Base address of the read-only memory block.
	///
	/// \param pLogLen
	///	Logical number of bytes in this block.
	///
	/// \param pPhyLen
	///	Physical number of bytes in this block.
	///
	/// \param pDoCopy
	///	Use a writeable copy of the read-only memory block instead
	///	(default: false) ?

	Data(
		const	Uchar *		pCstPtr,
		const	Uint32		pLogLen,
		const	Uint32		pPhyLen,
		const	Bool		pDoCopy // = false
	);

	/// \brief Attaches a new Data object to a preallocated writeable
	///	memory block.
	///
	/// This contructor creates a new Data object, and attaches it to
	/// a preallocated memory block, which starts at address \a pVarPtr,
	/// and which is exactly \a pPhyLen bytes long.
	///
	/// The memory allocator that was used to allocate this block can
	/// be specified in \a pAllocator. If this parameter is not given,
	/// then it won't be possible to auto-delete it when this Data object
	/// is destroyed, and any resize operation will automatically result
	/// in a deep copy being created. Otherwise, the specified
	/// Allocator will be used to resize and release
	/// the preallocated memory.
	///
	/// \param pVarPtr
	///	Base address of the preallocated memory block.
	///
	/// \param pPhyLen
	///	Number of bytes in this block.
	///
	/// \param pAllocator
	///	Allocator that was used to allocate this memory block, or
	///	none if this information is not available.

	Data(
			Uchar *		pVarPtr,
		const	Uint32		pPhyLen,
			Allocator *	pAllocator
	);

	/// \brief Copy constructor.
	///
	/// This constructor creates a copy of \a pOther. If \a pOther is
	/// attached to a read-only memory block, the new Data object is
	/// attached to the same memory block. Otherwise, a deep copy of the
	/// data of \a pOther is made.
	///
	/// \param pOther
	///	The Data object to copy.

	Data(
		const	Data &		pOther
	);

	/// \brief Copy constructor.
	///
	/// This constructor creates a new Data object, whose length is at
	/// least \a pLength bytes, and whose initial content is a deep copy
	/// of the first N bytes of \a pOther, where N is the minimum between
	/// \a pLength and the logical length of \a pOther.
	///
	/// \param pOther
	///	The Data object to [partially] copy.
	///
	/// \param pLength
	///	The minimum length of the new Data object.

	Data(
		const	Data &		pOther,
		const	Uint32		pLength
	);

	/// \brief Copy constructor.
	///
	/// This constructor constructs a new Data object, which is at least
	/// \a pLength1 + \a pLength2 bytes long, and whose content
	/// is the concatenation of
	/// the first \a pLength1 characters of the string pointed to by
	/// \a pCstSrc1, and the first \a pLength2 characters of the
	/// string pointed to by \a pCstSrc2.
	///
	/// The logical length of the new Data object is set to the sum of
	/// \a pLength1 and \a pLength2.

	Data(
		const	Uchar *		pCstSrc1,
		const	Uint32		pLength1,
		const	Uchar *		pCstSrc2,
		const	Uint32		pLength2
	);

	/// \brief Destroys this object.
	///
	/// This destructor automatically frees the memory block this
	/// object was attached to, if an allocator has been used to allocate
	/// it.

	/* virtual */ ~Data(
	);

	/// \brief Resizes this object.
	///
	/// This method resizes this object so that it becomes at least
	/// \a pLength bytes long.
	///
	/// If \a pLength is less or equal to the actual amount of allocated
	/// memory, then this method does nothing!
	///
	/// If this Data object was empty, then an allocator is first chosen
	/// based on the desired length. This allocator is then used to
	/// allocate at least the requested amount of memory.
	///
	/// If this Data object was not empty, and \a pLength is 0, then this
	/// call is equivalent to a call to the kill() method.
	///
	/// The content is always preserved, up to N bytes, where N is the
	/// minimum between the old length and \a pLength.
	///
	/// \param pLength
	///	The new minimum length of this object.
	///
	/// \throws OutOfMemory
	///	The system is running out of virtual memory.

	void resize(
		const	Uint32		pLength
	);

	/// \brief Detaches this object from any memory block it was attached
	///	to.
	///
	/// This method detaches this Data object from any memory block it was
	/// attached to, and frees this memory if an Allocator has been used
	/// to allocate it.

	void kill(
	);

	/// \brief Returns the logical length of this object.

	Uint32 getLogLength(
	) const {
		return logLength;
	}

	/// \brief Returns the number of bytes that could be read and/or
	///	written, starting from the address given by getCstPtr() or
	///	getVarPtr().

	Uint32 getPhyLength(
	) const {
		return phyLength;
	}

	/// \brief Returns the address of the first byte that could be read,
	///	of 0 if no byte is readable.

	const Uchar * getPhyCstPtr(
	) const {
		return ( phyLength ? phyCstPtr : 0 );
	}

	/// \brief Returns the address of the first byte that could be written,
	///	of 0 if no byte is writeable.

	Uchar * getPhyVarPtr(
	) {
		return ( phyLength ? phyVarPtr : 0 );
	}

	/// \brief Returns true iff the memory block attached to this object
	///	is writeable.

	Bool hasWriteAccess(
	) const {
		return ( logLength && phyVarPtr );
	}

	/// \brief Ensures that the memory block attached to this object is
	///	writeable.
	///
	/// Any copy of the value returned by a previous call to getCstPtr()
	/// or getVarPtr() should be reactualized after a call to this method!

	void forceWriteAccess(
	);

	Allocator * releaseData(
			Uchar * &	pPhyVarPtr,
			Uint32 &	pPhyLength,
		const	Bool		pRetrievable
	);

protected :

private :

	Uint32		logLength;	///< Logical length (in bytes).
	Uint32		phyLength;	///< Physical length (in bytes).
	const Uchar *	phyCstPtr;	///< Block start address (read-only).
	Uchar *		phyVarPtr;	///< Block start address (read-write).
	Allocator *	allocator;	///< Currently used allocator.

	//	len	cst	var	alloc
	//	---	---	---	-----
	//	0	x	x	0	empty
	//	>0	>0	0	0	c string, no delete
	//	>0	>0	>0	0	preallocated, no delete
	//	>0	>0	>0	!=0	auto-delete

	Data & operator = (
		const	Data		& pOther
	);

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_Data_H_

// ============================================================================

