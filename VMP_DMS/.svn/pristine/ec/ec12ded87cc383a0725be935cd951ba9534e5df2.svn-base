// ============================================================================
// 
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
//	TBFC.Mem.Block.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Mem_Block_H_
#define _TBFC_Mem_Block_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.FastCS.h"

#include "TBFC.Mem.AllocatorPtr.h"
#include "TBFC.Mem.Branch.h"
#include "TBFC.Mem.Data.h"

// ============================================================================

namespace TBFC {
namespace Mem {

// ============================================================================

/// \brief Frontend class, giving access to shared chunks of memory.
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Block {

public :

	/// \brief Creates a new empty Block of data.

	Block(
	) :
		branchPtr( new Branch( new Data ) ) {
	}

	/// \brief Creates a new Block, which is logically \a __len bytes long.
	///
	/// \param __len
	///	The requested block size.

	Block(
		const	Uint32		__len
	) :
		branchPtr( new Branch( new Data( __len ) ) ) {
	}

	/// \brief Attaches a new Block to a preallocated chunk of memory.
	///
	/// This contructor creates a new Block, and attaches it to
	/// a preallocated memory block, starting at address \a __ptr, and which
	/// is exactly \a __len bytes long.
	///
	/// The memory allocator that was used to allocate this chunk can
	/// be specified using \a allocator. If this parameter is not given,
	/// then it won't be possible to auto-delete it when this Block
	/// is destroyed, and any resize operation will automatically result
	/// in a deep copy being created. Otherwise, the specified
	/// Allocator will be used to resize and/or release
	/// the preallocated memory when it becomes unreferenced by any Block.
	///
	/// \param __ptr
	///	Base address of the preallocated memory block.
	///
	/// \param __len
	///	Number of bytes in this block.
	///
	/// \param __allocator
	///	Allocator that was used to allocate this memory block, or
	///	none if this information is not available.

	Block(
			Uchar		* __ptr,
		const	Uint32		__len,
			AllocatorPtr	__allocator
	) :
		branchPtr( new Branch( new Data( __ptr, __len, __allocator ) ) ) {
	}

	/// \brief Attaches a new Block to some read-only memory block.
	///
	/// This constructor creates a new Block, and attaches it to
	/// some read-only memory block, starting at address \a __ptr, and which
	/// is \a __len bytes long.
	///
	/// If \a __copy is true, the new Block is attached to a copy of
	/// the read-only memory block instead.

	Block(
		const	Uchar		* __ptr,
		const	Uint32		__len,
		const	Bool		__copy = false
	) :
		branchPtr( new Branch( new Data( __ptr, __len, __len, __copy ) ) ) {
	}

	/// \brief Creates a new Block, which becomes owner of the Data block
	///	pointed to by \a __data.

	Block(
			Data		* __data
	) :
		branchPtr( new Branch( __data ) ) {
	}

	/// \brief Creates a new Block, sharing a copy-on-write chunk of
	///	memory with \a __other.
	///
	/// If \a __other is actually being written to (by means of a Writer),
	/// then this new Block automatically creates a copy ("snapshot") of
	/// the memory block.

	Block(
		const	Block		& __other
	) :
		branchPtr( new Branch( * __other.branchPtr ) ) {
	}

	/// \brief Creates a new Block, sharing a copy-on-write chunk of
	///	memory with \a __other.
	///
	/// If \a __other is actually being written to (by means of a Writer),
	/// then this new Block automatically creates a copy ("snapshot") of
	/// the memory block.

	Block(
		const	Block		& __other,
		const	Uint32		__beg,
		const	Uint32		__len
	) :
		branchPtr( new Branch( * __other.branchPtr, __beg, __len ) ) {
	}

	/// \brief Creates a new Block, sharing a copy-on-write chunk of
	///	memory with \a __buffer.

	Block(
		const	Buffer		& __buffer
	) :
		branchPtr( new Branch( __buffer ) ) {
	}

	/// \brief Destroys this object, deleting the supervised chunk of
	///	memory if possible.
	///
	/// The conditions that need to be met for a chunk of memory to be
	/// deleted are:
	/// -	the allocation method has to be known. This is the case if the
	///	memory was allocated by this object, or if some Allocator has
	///	been mentioned in the appropriate constructor.
	/// -	the chunk of memory is not used anymore. This is the case
	///	if there are no Reader or Writer using this
	///	chunk of memory.

	/* virtual */ ~Block(
	);

	/// \brief Suppress the reference to the memory block (if any).

	void kill(
	);

	/// \brief Detaches this Block from the chunk of memory it actually
	///	supervises (maybe deleting it first), and attaches it to the
	///	copy-on-write chunk of memory supervised by \a __other.

	Block & operator = (
		const	Block		& __other
	);

	/// \brief Returns the length of the shared data block.

	Uint32 getLength(
	) const;

	/// \brief Returns a Buffer attached to the chunk of memory of this
	///	Block if this block is not actually being written to, or to a
	///	copy of this chunk of memory if this block is being written to.

	Buffer toBuffer(
	) const;

protected :

private :

	friend class Reader;
	friend class Writer;

	mutable FastCS	criticalS;
	Branch		* branchPtr;

};

// ============================================================================

} // namespace Mem
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Mem_Block_H_

// ============================================================================

