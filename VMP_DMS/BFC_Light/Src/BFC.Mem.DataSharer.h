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
//	BFC.Mem.DataSharer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_DataSharer_H_
#define _BFC_Mem_DataSharer_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.FastCS.h"

#include "BFC.Mem.AllocatorPtr.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

class Data;
class DataProxy;

// ============================================================================

/// \brief Base class of all classes that share a common Data/DataProxy pair.
///
/// This class implements a rudimentary "copy-on-write" mechanism, allowing
/// multiple instances to share a common Data block, and defer any deep copy
/// operation until one of the instances has to modify the data block.
///
/// This class also implements a logical viewing mechanism ("windowing"),
/// allowing multiple instances to share a common data block, but to present
/// different parts of this block to the user.
///
/// This class doesn't manage anything else than the lengths (both physical
/// and logical) of the data; the content management is implemented in
/// subclasses.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL DataSharer {

public :

	/// \brief Creates a new DataSharer, not associated to any data block.

	DataSharer(
	) :
		dataProxy( 0 ),
		blkLength( 0 ),
		logOffset( 0 ),
		logLength( 0 ),
		logCstPtr( 0 ) {
	}

	/// \brief Creates a new DataSharer, attached to memory block
	///	\a pDataBlock.
	///
	/// \param pDataBlock
	///	The new shared data block. May not already be used.

	DataSharer(
			Data *		pDataBlock
	);

	/// \brief Creates a new DataSharer, sharing a data block with
	///	\a pOther, and presenting the same portion to the user.

	DataSharer(
		const	DataSharer &	pOther
	);

	/// \brief Creates a new DataSharer, sharing a data block with
	///	\a pOther, and presenting the subportion starting at
	///	\a pBeg to the user.

	DataSharer(
		const	DataSharer &	pOther,
		const	Uint32		pBeg
	);

	/// \brief Creates a new DataSharer, sharing a data block with
	///	\a pOther, and presenting the subportion starting at
	///	\a pBeg and of length \a pLen to the user.

	DataSharer(
		const	DataSharer &	pOther,
		const	Uint32		pBeg,
		const	Uint32		pLen
	);

	/// \brief Destroys this object.
	///
	/// If this object was the last user of the shared data block, then
	/// this data block is deleted.

	/* virtual */ ~DataSharer(
	) {
		if ( dataProxy ) {
			deleteData();
		}
	}

	/// \brief Assigns a copy of \a pOther to this object.

	DataSharer & operator = (
		const	DataSharer &	pOther
	);

	/// \brief Returns the offset of the start of the visible part within
	///	the shared block.

	Uint32 getLogOffset(
	) const {
		return logOffset;
	}

	/// \brief Sets tje offset of the start of the visible part within the
	///	shared block, modifying the logical length accordingly.

	void setLogOffset(
		const	Uint32		pNewOff
	);

	/// \brief Returns the length of the user-visible part of the shared
	///	data block.

	Uint32 getLogLength(
	) const {
		return logLength;
	}

	/// \brief Sets the logical length of the user-visible part of the
	///	shared data block to \a pNewLen.
	///
	/// \param pNewLen
	///	The new logical length.

	void setLogLength(
		const	Uint32		pNewLen
	);

	/// \brief Returns a read-only pointer to the start of the user-visible
	///	part of the shared data block.

	const Uchar * getLogCstPtr(
	) const {
		return logCstPtr;
	}

	/// \brief Returns a read-only pointer to the character at position
	///	\a pOffset in the user-visible part of the shared data block.

	const Uchar * getLogCstPtr(
		const	Uint32		pOffset
	) const {
		if ( pOffset >= logLength ) {
			genOutOfRange( pOffset );
		}
		return ( logCstPtr + pOffset );
	}

	/// \brief Returns a writeable pointer to the start of the user-visible
	///	part of the shared data block.
	///
	/// If this object was not the only user of the shared data block, or
	/// if the shared data block was not writeable, then this
	/// block is first copied (deep copy) and the returned pointer points
	/// to this private copy.

	Uchar * getLogVarPtr(
	);

	/// \brief Returns a writeable pointer to the character at position
	///	\a pOffset in the user-visible part of the shared data block.
	///
	/// If this object was not the only user of the shared data block, or
	/// if the shared data block was not writeable, then this
	/// block is first copied (deep copy) and the returned pointer points
	/// to this private copy.

	Uchar * getLogVarPtr(
		const	Uint32		pOffset
	) {
		if ( pOffset >= logLength ) {
			genOutOfRange( pOffset );
		}
		return ( getLogVarPtr() + pOffset );
	}

	/// \brief Returns the logical length of the shared data block.

	Uint32 getBlkLength(
	) const {
		return blkLength;
	}

	/// \brief Sets the minimum length of the shared data block to
	///	\a pBlkLength.
	///
	/// The logical length of the user visible part is not affected.

	void setBlkLength(
		const	Uint32		pBlkLength
	);

	/// \brief Returns the physical length of the shared data block.

	Uint32 getPhyLength(
	) const;

	/// \brief Attaches this object to a new data block.
	///
	/// If this object was already associated to a data block, and if it
	/// was the only user of this block, then this block is first deleted.
	///
	/// \param pDataBlock
	///	The new shared data block. May not already be used.
	///
	/// \param pLogOffset
	///	Start of the user-visible part within \a pDataBlock.
	///
	/// \param pLogLength
	///	Length of the user-visible part within \a pDataBlock.

	void attachData(
			Data *		pDataBlock,
		const	Uint32		pLogOffset,
		const	Uint32		pLogLength
	);

	/// \brief Deletes the content of this object.
	///
	/// This method irrevocably deletes the content of this object.
	///
	/// If this content was shared, no actual memory deallocation
	/// occurs.

	void deleteData(
	);

	AllocatorPtr releaseData(
			Uchar * &	pPhyVarPtr,
			Uint32 &	pPhyLength,
		const	Bool		pRetrievable
	);

protected :

private :

	void genOutOfRange(
		const	Uint32		pOffset
	) const;

	mutable FastCS	criticalS;	///< Critical section.
	DataProxy *	dataProxy;	///< Shared data proxy.
	Uint32		blkLength;	///< Cached block length.
	Uint32		logOffset;	///< Logical part offset.
	Uint32		logLength;	///< Logical part length.
	const Uchar *	logCstPtr;	///< Cached start address (RO).

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_DataSharer_H_

// ============================================================================

