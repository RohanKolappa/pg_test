// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::IO".
// 
// "BFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.IO.MemoryInputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_MemoryInputStream_H_
#define _BFC_IO_MemoryInputStream_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

#include "BFC.IO.FinalDevice.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

BFC_PTR_DECL( BFC_IO_DLL, MemoryInputStream )

// ============================================================================

/// \brief A FinalDevice to read streams from memory.
///
/// \ingroup BFC_IO

class BFC_IO_DLL MemoryInputStream : public FinalDevice {

public :

	/// \brief Creates a new MemoryInputStream, fed by Buffer \a content.
	///
	/// This method associates this Device to an existing Buffer stored
	/// in memory.

	MemoryInputStream(
		const	Buffer&		content
	);

	virtual void open(
	);

	virtual void close(
	);

	/// \brief Appends \a contentToAdd to the internal source buffer.

	void addContent(
		const	Buffer&		contentToAdd
	);

	virtual Buffer getBytes(
	);

	virtual Uint64 seek(
		const	Uint64		offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

protected :

private :

	// internal state

	Buffer			buffer;

	const Uchar		*address;
	Uint32			size;
	Uint32			cursor;

	// others...

	MemoryInputStream(
		const	MemoryInputStream&
	);

	MemoryInputStream& operator = (
		const	MemoryInputStream&
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_MemoryInputStream_H_

// ============================================================================

