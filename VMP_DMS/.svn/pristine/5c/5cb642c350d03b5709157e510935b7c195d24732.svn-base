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
//	TBFC.IO.MemoryOutputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_MemoryOutputStream_H_
#define _TBFC_Light_MemoryOutputStream_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

#include "TBFC.IO.FinalDevice.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

TBFC_PTR_DECL( TBFC_Light_DLL, MemoryOutputStream )

// ============================================================================

/// \brief A FinalDevice to write streams to memory.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL MemoryOutputStream : public FinalDevice {

public :

	/// \brief Constructs an new MemoryOutputStream.

	MemoryOutputStream(
	);

	virtual ~MemoryOutputStream(
	);

	virtual void open(
	);

	virtual void close(
	);

	Buffer getContent(
	) const;

	virtual void putBytes(
		const	Buffer&		input
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

	// internal state...

	Buffer			buffer;
	Uint32			offset; // insertion point in content

	// others...

	MemoryOutputStream(
		const	MemoryOutputStream&
	);

	MemoryOutputStream& operator = (
		const	MemoryOutputStream&
	);

};

// ============================================================================

} // namespace IO
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_MemoryOutputStream_H_

// ============================================================================

