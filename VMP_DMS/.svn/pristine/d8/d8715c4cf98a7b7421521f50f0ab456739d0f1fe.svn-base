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
//	BFC.IO.ProxyDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_ProxyDevice_H_
#define _BFC_IO_ProxyDevice_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SPtr.h"

#include "BFC.IO.Device.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

BFC_PTR_DECL( BFC_IO_DLL, ProxyDevice )

// ============================================================================

/// \brief Base class of all 'proxy-like' devices (protocol implementers, ...).
///
/// The ProxyDevice objects are those that interact with another Device
/// (ByteInputStream, ...), in opposition to the FinalDevice objects, which
/// interact directly with some external objects (File, MemoryInputStream, ...).
///
/// \ingroup BFC_IO

class BFC_IO_DLL ProxyDevice : public Device {

public :

	/// \brief Creates a new ProxyDevice, attached to \a device.

	ProxyDevice(
			DevicePtr	device
	);

	/// \brief Destroys this object.
	///
	/// If the peer was still open, and this object was responsible for
	/// opening it, then the peer is automatically closed.

	virtual ~ProxyDevice(
	);

	virtual void open(
	);

	virtual void close(
	);

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

	virtual void flush(
	);

protected :

	/// \brief Returns our peer device.

	DevicePtr getPeer(
	) const {
		return peer;
	}

private :

	void closePeer(
	);

	DevicePtr	peer;	///< Peer device.
	Bool		clse;

	// others...

	ProxyDevice(
		const	ProxyDevice&
	);

	ProxyDevice& operator = (
		const	ProxyDevice&
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_ProxyDevice_H_

// ============================================================================

