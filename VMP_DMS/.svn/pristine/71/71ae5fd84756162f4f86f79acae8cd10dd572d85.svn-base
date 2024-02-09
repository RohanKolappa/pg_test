// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::IO".
// 
// "TBFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.IO.MemoryInputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_MemoryInputStream_H_
#define _TBFC_Light_MemoryInputStream_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

#include "TBFC.IO.FinalDevice.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

TBFC_PTR_DECL( TBFC_Light_DLL, MemoryInputStream )

// ============================================================================

/// \brief A FinalDevice to read streams from memory.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL MemoryInputStream : public FinalDevice {

public :

	/// \brief Creates a new MemoryInputStream, fed by Buffer \a content.
	///
	/// This method associates this Device to an existing Buffer stored
	/// in memory.

	MemoryInputStream(
		const	Buffer&		content
	);

	virtual ~MemoryInputStream(
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
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_MemoryInputStream_H_

// ============================================================================

