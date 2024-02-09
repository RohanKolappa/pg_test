// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC".
// 
// "TBFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Mem.DataSharer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Mem_DataSharer_H_
#define _TBFC_Mem_DataSharer_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.FastCS.h"

#include "TBFC.Mem.AllocatorPtr.h"

// ============================================================================

namespace TBFC {
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
/// \ingroup TBFC_Mem

class TBFC_Light_DLL DataSharer {

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
	///	\a __dataBlock.
	///
	/// \param __dataBlock
	///	The new shared data block. May not already be used.

	DataSharer(
			Data *		__dataBlock
	);

	/// \brief Creates a new DataSharer, sharing a data block with
	///	\a __other, and presenting the same portion to the user.

	DataSharer(
		const	DataSharer &	__other
	);

	/// \brief Creates a new DataSharer, sharing a data block with
	///	\a __other, and presenting the subportion starting at
	///	\a __beg to the user.

	DataSharer(
		const	DataSharer &	__other,
		const	Uint32		__beg
	);

	/// \brief Creates a new DataSharer, sharing a data block with
	///	\a __other, and presenting the subportion starting at
	///	\a __beg and of length \a __len to the user.

	DataSharer(
		const	DataSharer &	__other,
		const	Uint32		__beg,
		const	Uint32		__len
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

	/// \brief Assigns a copy of \a __other to this object.

	DataSharer & operator = (
		const	DataSharer &	__other
	);

	/// \brief Returns the offset of the start of the visible part within
	///	the shared block.

	Uint32 getLogOffset(
	) const {
		return logOffset;
	}

	/// \brief Returns the length of the user-visible part of the shared
	///	data block.

	Uint32 getLogLength(
	) const {
		return logLength;
	}

	/// \brief Sets the logical length of the user-visible part of the
	///	shared data block to \a __newlen.
	///
	/// \param __newlen
	///	The new logical length.

	void setLogLength(
		const	Uint32		__newlen
	);

	/// \brief Returns a read-only pointer to the start of the user-visible
	///	part of the shared data block.

	const Uchar * getLogCstPtr(
	) const {
		return logCstPtr;
	}

	/// \brief Returns a read-only pointer to the character at position
	///	\a __offset in the user-visible part of the shared data block.

	const Uchar * getLogCstPtr(
		const	Uint32		__offset
	) const {
// FIXME: intelligently preprocess this test...
		if ( __offset >= logLength ) {
			genOutOfRange( __offset );
		}
		return ( logCstPtr + __offset );
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
	///	\a __offset in the user-visible part of the shared data block.
	///
	/// If this object was not the only user of the shared data block, or
	/// if the shared data block was not writeable, then this
	/// block is first copied (deep copy) and the returned pointer points
	/// to this private copy.

	Uchar * getLogVarPtr(
		const	Uint32		__offset
	) {
// FIXME: intelligently preprocess this test...
		if ( __offset >= logLength ) {
			genOutOfRange( __offset );
		}
		return ( getLogVarPtr() + __offset );
	}

	/// \brief Returns the logical length of the shared data block.

	Uint32 getBlkLength(
	) const {
		return blkLength;
	}

	/// \brief Sets the minimum length of the shared data block to
	///	\a __minLength.
	///
	/// The logical length of the user visible part is not affected.

	void setBlkLength(
		const	Uint32		__blkLength
	);

	/// \brief Returns the physical length of the shared data block.

	Uint32 getPhyLength(
	) const;

	/// \brief Attaches this object to a new data block.
	///
	/// If this object was already associated to a data block, and if it
	/// was the only user of this block, then this block is first deleted.
	///
	/// \param __dataBlock
	///	The new shared data block. May not already be used.
	///
	/// \param __logOffset
	///	Start of the user-visible part within \a __dataBlock.
	///
	/// \param __logLength
	///	Length of the user-visible part within \a __dataBlock.

	void attachData(
			Data *		__dataBlock,
		const	Uint32		__logOffset,
		const	Uint32		__logLength
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
			Uchar * &	__phyVarPtr,
			Uint32 &	__phyLength,
		const	Bool		__retrievable
	);

protected :

private :

	void genOutOfRange(
		const	Uint32		__offset
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
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Mem_DataSharer_H_

// ============================================================================

