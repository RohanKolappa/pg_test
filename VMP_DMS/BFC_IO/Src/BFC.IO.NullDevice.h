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
//	BFC.IO.NullDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_NullDevice_H_
#define _BFC_IO_NullDevice_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace IO {

BFC_PTR_DECL( BFC_IO_DLL, NullDevice )

} // namespace IO
} // namespace BFC

// ============================================================================

#include "BFC.IO.FinalDevice.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_IO

class BFC_IO_DLL NullDevice : public FinalDevice {

public :

	/// \brief Creates a new NullDevice.

	NullDevice(
	);

	virtual void open(
	);

	virtual void close(
	);

	virtual Buffer getBytes(
	);

	virtual void putBytes(
		const	Buffer &	input
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

	/// \brief Forbidden copy constructor.

	NullDevice(
		const	NullDevice &
	);

	/// \brief Forbidden copy operator.

	NullDevice & operator = (
		const	NullDevice &
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_NullDevice_H_

// ============================================================================

