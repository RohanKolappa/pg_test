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
//	BFC.MemBlock.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_MemBlock_H_
#define _BFC_MemBlock_H_

// ============================================================================

#include "BFC.MemAllocator.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// Represents a contiguous area of memory.

/// Objects of this class are associated with contiguous pieces of memory,
/// allocated by the MemAllocator, or directly by the user.
///
/// \ingroup Common_Base

class MemBlock {

public :

	/// Creates an empty block of memory.

	MemBlock(
	);

	/// No-op!!!

	/// This destructor does nothing! Any previously allocated memory
	/// will NOT be released!!!
	/// You have to call the 'free' method yourself first!

	~MemBlock(
	);

	/// Attaches this object to a user-supplied block of memory.

	/// This method attaches the memory block starting at \a addr, and of
	/// length \a len bytes, to this object.
	/// Any previous content is overriden without warning, thus
	/// potentially leading to memory leaks!
	///
	/// Calling the 'free' method on this object will force using the
	/// delete[] operator on \a addr.

	void use(
			Uchar	*addr,
			Uint32	len
	);

	/// Allocates a new chunk of memory.

	/// This method uses the MemAllocator to allocate a new chunk of
	/// memory, which will be at least \a newuser bytes long (the actual
	/// length is given by calling the 'getSize' method.
	///
	/// Calling the 'free' method on this object will then use the
	/// MemAllocator to release the block of memory.

	Uchar *alloc(
		const	Uint32		newuser
	);

	/// Deallocates the memory.

	void free(
	);

	Uchar	*getAddr() const {return addr;}
	Uint32	getSize() const {return size;}

protected :

private :

	// internal state...

	friend class MemAllocator;

	Uchar		*addr;	// meaningful iff size != 0
	Uint32		size;	// asked by user
	Bool		adel;	// delete with delete[]...

	Uint32		expo;	// s.t. 2^^expo == size
	Uint32		sidx;	// index in stratum

	MemAllocatorPtr	allocator;

	// others...

	MemBlock(
		const	MemBlock&	o
	);

	MemBlock& operator = (
		const	MemBlock&	o
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_MemBlock_H_

// ============================================================================

