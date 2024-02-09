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
//	BFC.Mem.Block.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_Block_H_
#define _BFC_Mem_Block_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.FastCS.h"

#include "BFC.Mem.AllocatorPtr.h"
#include "BFC.Mem.Branch.h"
#include "BFC.Mem.Data.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Frontend class, giving access to shared chunks of memory.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Block {

public :

	/// \brief Creates a new empty Block of data.

	Block(
	) :
		branchPtr( new Branch( new Data ) ) {
	}

	/// \brief Creates a new Block, which is logically \a pLength bytes long.
	///
	/// \param pLength
	///	The requested block size.

	Block(
		const	Uint32		pLength
	) :
		branchPtr( new Branch( new Data( pLength ) ) ) {
	}

	/// \brief Attaches a new Block to a preallocated chunk of memory.
	///
	/// This contructor creates a new Block, and attaches it to
	/// a preallocated memory block, starting at address \a pVarPtr, and which
	/// is exactly \a pLength bytes long.
	///
	/// The memory allocator that was used to allocate this chunk can
	/// be specified using \a allocator. If this parameter is not given,
	/// then it won't be possible to auto-delete it when this Block
	/// is destroyed, and any resize operation will automatically result
	/// in a deep copy being created. Otherwise, the specified
	/// Allocator will be used to resize and/or release
	/// the preallocated memory when it becomes unreferenced by any Block.
	///
	/// \param pVarPtr
	///	Base address of the preallocated memory block.
	///
	/// \param pLength
	///	Number of bytes in this block.
	///
	/// \param pAllocator
	///	Allocator that was used to allocate this memory block, or
	///	none if this information is not available.

	Block(
			Uchar *		pVarPtr,
		const	Uint32		pLength,
			Allocator *	pAllocator
	) :
		branchPtr( new Branch( new Data( pVarPtr, pLength, pAllocator ) ) ) {
	}

	/// \brief Attaches a new Block to some read-only memory block.
	///
	/// This constructor creates a new Block, and attaches it to
	/// some read-only memory block, starting at address \a pCstPtr, and which
	/// is \a pLength bytes long.
	///
	/// If \a pDoCopy is true, the new Block is attached to a copy of
	/// the read-only memory block instead.

	Block(
		const	Uchar *		pCstPtr,
		const	Uint32		pLength,
		const	Bool		pDoCopy = false
	) :
		branchPtr( new Branch( new Data( pCstPtr, pLength, pLength, pDoCopy ) ) ) {
	}

	/// \brief Creates a new Block, which becomes owner of the Data block
	///	pointed to by \a pData.

	Block(
			Data *		pData
	) :
		branchPtr( new Branch( pData ) ) {
	}

	/// \brief Creates a new Block, sharing a copy-on-write chunk of
	///	memory with \a pOther.
	///
	/// If \a pOther is actually being written to (by means of a Writer),
	/// then this new Block automatically creates a copy ("snapshot") of
	/// the memory block.

	Block(
		const	Block &		pOther
	) :
		branchPtr( new Branch( * pOther.branchPtr ) ) {
	}

	/// \brief Creates a new Block, sharing a copy-on-write chunk of
	///	memory with \a pOther.
	///
	/// If \a pOther is actually being written to (by means of a Writer),
	/// then this new Block automatically creates a copy ("snapshot") of
	/// the memory block.

	Block(
		const	Block &		pOther,
		const	Uint32		pOffset,
		const	Uint32		pLength
	) :
		branchPtr( new Branch( * pOther.branchPtr, pOffset, pLength ) ) {
	}

	/// \brief Creates a new Block, sharing a copy-on-write chunk of
	///	memory with \a pBuffer.

	Block(
		const	Buffer &		pBuffer
	) :
		branchPtr( new Branch( pBuffer ) ) {
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
	///	copy-on-write chunk of memory supervised by \a pOther.

	Block & operator = (
		const	Block &		pOther
	);

	Bool operator == (
		const	Block &		other
	) const;

	Bool operator != (
		const	Block &		other
	) const;

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
	Branch *	branchPtr;

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_Block_H_

// ============================================================================

