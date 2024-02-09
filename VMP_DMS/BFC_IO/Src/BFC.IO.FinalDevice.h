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
//	BFC.IO.FinalDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_FinalDevice_H_
#define _BFC_IO_FinalDevice_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.IO.Device.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

/// \brief Empty class, used as parent for all final devices (i.e. not
///	chainable).
///
/// The FinalDevice objects are those that interact directly with some external
/// objects (File, MemoryInputStream, ...), in opposition to the ProxyDevice
/// objects, which interact with another Device (ByteInputStream, ...).
///
/// \ingroup BFC_IO

class BFC_IO_DLL FinalDevice : public Device {

public :

	FinalDevice(
	) :
		blockSize( 1 << 16 ) {
	}

	virtual void setBlockSize(
		const	Uint32		pSize
	);

	virtual Uint32 getBlockSize(
	) const;

	virtual const Time::Clock & getAbsGetTimeout(
	) const;

	virtual void setAbsGetTimeout(
		const	Time::Clock &	pAbsTime
	);

	virtual const Time::Clock & getAbsPutTimeout(
	) const;

	virtual void setAbsPutTimeout(
		const	Time::Clock &	pAbsTime
	);

private :

	Uint32		blockSize;	///< Preferred read size.
	Time::Clock	getTimeout;
	Time::Clock	putTimeout;

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_FinalDevice_H_

// ============================================================================

