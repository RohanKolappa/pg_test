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
//	TBFC.Mem.Data.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Mem_Data_H_
#define _TBFC_Mem_Data_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Mem.AllocatorPtr.h"

// ============================================================================

namespace TBFC {
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
/// -	create an empty Data (no associated memory block),
/// -	attach a new Data to a read-only memory block,
/// -	attach a new Data to a pre-allocated writeable memory block,
/// -	let the new Data allocate a new memory block itself.
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Data {

public :

	/// \brief Creates a new empty memory block.
	///
	/// If memory has to be allocated (see resize()) later, then a suitable
	/// allocator will be chosen at that time.

	Data(
	);

	/// \brief Creates a new memory block which is at least \a __len bytes
	///	long.
	///
	/// This constructor creates a new memory block and tries to allocate
	/// at least \a __len bytes, using the preferred Allocator for this
	/// particular block size.
	///
	/// \param __len
	///	The requested minimum block size. The actual number of bytes
	///	that were allocated is returned by getPhyLength(), while
	///	the value of this parameter is returned by getLogLength().
	///
	/// \throws OutOfMemory
	///	If the memory allocation failed.

	Data(
		const	Uint32		__len
	);

	/// \brief Attaches a new Data object to some read-only memory block.
	///
	/// This constructor creates a new Data object, and attaches it to
	/// some read-only memory block, starting at address \a __cstPtr, and
	/// which is logically __logLen bytes long, and physically \a __phyLen
	/// bytes long.
	///
	/// \param __cstPtr
	///	Base address of the read-only memory block.
	///
	/// \param __logLen
	///	Logical number of bytes in this block.
	///
	/// \param __phyLen
	///	Physical number of bytes in this block.
	///
	/// \param __doCopy
	///	Use a writeable copy of the read-only memory block instead
	///	(default: false) ?

	Data(
		const	Uchar *		__cstPtr,
		const	Uint32		__logLen,
		const	Uint32		__phyLen,
		const	Bool		__doCopy // = false
	);

	/// \brief Attaches a new Data object to a preallocated writeable
	////	memory block.
	///
	/// This contructor creates a new Data object, and attaches it to
	/// a preallocated memory block, which starts at address \a __varPtr,
	/// and which is exactly \a __phyLen bytes long.
	///
	/// The memory allocator that was used to allocate this block can
	/// be specified in \a __allocator. If this parameter is not given,
	/// then it won't be possible to auto-delete it when this Data object
	/// is destroyed, and any resize operation will automatically result
	/// in a deep copy being created. Otherwise, the specified
	/// Allocator will be used to resize and release
	/// the preallocated memory.
	///
	/// \param __varPtr
	///	Base address of the preallocated memory block.
	///
	/// \param __phyLen
	///	Number of bytes in this block.
	///
	/// \param __allocator
	///	Allocator that was used to allocate this memory block, or
	///	none if this information is not available.

	Data(
			Uchar *		__varPtr,
		const	Uint32		__phyLen,
			AllocatorPtr	__allocator
	);

	/// \brief Copy constructor.
	///
	/// This constructor creates a copy of \a __other. If \a __other is
	/// attached to a read-only memory block, the new Data object is
	/// attached to the same memory block. Otherwise, a deep copy of the
	/// data of \a __other is made.
	///
	/// \param __other
	///	The Data object to copy.

	Data(
		const	Data &		__other
	);

	/// \brief Copy constructor.
	///
	/// This constructor creates a new Data object, whose length is at
	/// least \a __len bytes, and whose initial content is a deep copy
	/// of the first N bytes of \a __other, where N is the minimum between
	/// \a __len and the logical length of \a __other.
	///
	/// \param __other
	///	The Data object to [partially] copy.
	///
	/// \param __len
	///	The minimum length of the new Data object.

	Data(
		const	Data &		__other,
		const	Uint32		__len
	);

	/// \brief Copy constructor.
	///
	/// This constructor constructs a new Data object, which is at least
	/// \a __len1 + \a __len2 bytes long, and whose content
	/// is the concatenation of
	/// the first \a __len1 characters of the string pointed to by
	/// \a __src1, and the first \a __len2 characters of the
	/// string pointed to by \a __src2.
	///
	/// The logical length of the new Data object is set to the sum of
	/// \a __len1 and \a __len2.

	Data(
		const	Uchar *		__src1,
		const	Uint32		__len1,
		const	Uchar *		__src2,
		const	Uint32		__len2
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
	/// \a __len bytes long.
	///
	/// If \a __len is less or equal to the actual amount of allocated
	/// memory, then this method does nothing!
	///
	/// If this Data object was empty, then an allocator is first chosen
	/// based on the desired length. This allocator is then used to
	/// allocate at least the requested amount of memory.
	///
	/// If this Data object was not empty, and \a __len is 0, then this
	/// call is equivalent to a call to the kill() method.
	///
	/// The content is always preserved, up to N bytes, where N is the
	/// minimum between the old length and \a __len.
	///
	/// \param __len
	///	The new minimum length of this object.
	///
	/// \throws OutOfMemory
	///	The system is running out of virtual memory.

	void resize(
		const	Uint32		__len
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

	AllocatorPtr releaseData(
			Uchar * &	__phyVarPtr,
			Uint32 &	__phyLength,
		const	Bool		__retrievable
	);

protected :

private :

	Uint32		logLength;	///< Logical length (in bytes).
	Uint32		phyLength;	///< Physical length (in bytes).
	const Uchar *	phyCstPtr;	///< Block start address (read-only).
	Uchar *		phyVarPtr;	///< Block start address (read-write).
	AllocatorPtr	allocator;	///< Currently used allocator.

	//	len	cst	var	alloc
	//	---	---	---	-----
	//	0	x	x	0	empty
	//	>0	>0	0	0	c string, no delete
	//	>0	>0	>0	0	preallocated, no delete
	//	>0	>0	>0	!=0	auto-delete

	Data & operator = (
		const	Data		& __other
	);

};

// ============================================================================

} // namespace Mem
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Mem_Data_H_

// ============================================================================

