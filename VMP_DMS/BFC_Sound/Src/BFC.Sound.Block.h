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
//	BFC.Sound.Block.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_Block_H_
#define _BFC_Sound_Block_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.BlockPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Mem.Block.h"
#include "BFC.Mem.Reader.h"
#include "BFC.Mem.Writer.h"

#include "BFC.Sound.Format.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL Block : virtual public SObject {

public :

	/// \brief Creates a new Block.

	Block(
			FormatCPtr	pFormat,
		const	Uint32		pLength = 0
	);

	/// \brief Creates a new Block.

	Block(
			FormatCPtr	pFormat,
		const	Buffer &	pPacked
	);

	/// \brief Copy constructor.

	Block(
		const	Block &		pOther
	);

	/// \brief Copy operator.

	Block & operator = (
		const	Block &		pOther
	);

	void kill(
	);

	Uint32 getLength(
	) const {
		return length;
	}

	FormatCPtr getFormat(
	) const {
		return format;
	}

	/// \brief Returns the data as a packed (i.e. interlaced) Buffer.

	Buffer getData(
	) const;

	/// \brief Sets the data as a packed (i.e. interlaced) Buffer.

	void setData(
		const	Buffer &	pPacked
	);

	/// \brief Returns the data of channel \a pIndex as a Buffer.

	Buffer getData(
		const	Uint32		pIndex
	) const;

	/// \brief Returns the data of channel \a pIndex as a Mem::Block.

	const Mem::Block & getBlock(
		const	Uint32		pIndex
	) const;

	/// \brief Sets the data of channel \a pIndex as a Buffer.

	void setData(
		const	Uint32		pIndex,
		const	Buffer &	pData
	);

	/// \brief Sets the data of channel \a pIndex as a Mem::Block.

	void setBlock(
		const	Uint32		pIndex,
		const	Mem::Block &	pData
	);

	/// \brief Returns a Mem::Reader reading the data as a packed buffer.

	Mem::Reader getReader(
	) const;

	/// \brief Returns a Mem::Writer writing the data as a packed buffer.

	Mem::Writer getWriter(
	);

	/// \brief Returns a Mem::Reader reading the data of channel \a pIndex.

	Mem::Reader getReader(
		const	Uint32		pIndex
	) const;

	/// \brief Returns a Mem::Writer writing the data of channel \a pIndex.

	Mem::Writer getWriter(
		const	Uint32		pIndex
	);

	BlockCPtr getLeft(
		const	Uint32		pLength
	) const;

	BlockCPtr getMid(
		const	Uint32		pOffset,
		const	Uint32		pLength
	) const;

	BlockCPtr cut(
		const	Uint32		pLength
	);

	void add(
			BlockCPtr	pBlock
	);

protected :

	void moveToPacked(
	) const;

	void moveToPlanar(
	) const;

	void ensurePacked(
	) const;

	void ensureBlock(
		const	Uint32		pIndex
	) const;

private :

	Uint32			length;	///< Number of samples per channel.
	FormatCPtr		format;	///< Format description.
	mutable Bool		planar;	///< Use ( planar ? blocks : packed ).
	mutable Mem::Block	packed;	///< Packed version of data.
	mutable Array< Mem::Block >
				blocks;	///< Unpacked version of data.

	/// \brief Forbidden default constructor.

	Block(
	);

};

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_Block_H_

// ============================================================================

