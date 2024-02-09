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
//	TBFC.IO.ProxyDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_ProxyDevice_H_
#define _TBFC_Light_ProxyDevice_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SPtr.h"

#include "TBFC.IO.Device.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

TBFC_PTR_DECL( TBFC_Light_DLL, ProxyDevice )

// ============================================================================

/// \brief Base class of all 'proxy-like' devices (protocol implementers, ...).
///
/// The ProxyDevice objects are those that interact with another Device
/// (ByteInputStream, ...), in opposition to the FinalDevice objects, which
/// interact directly with some external objects (File, MemoryInputStream, ...).
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL ProxyDevice : public Device {

public :

	/// \brief Creates a new ProxyDevice, attached to \a device.

	ProxyDevice(
			DevicePtr	device
	);

	/// \brief Destroys this object.

	virtual ~ProxyDevice(
	);

	virtual void open(
	);

	virtual void close(
	);

	virtual void flush(
	);

protected :

	DevicePtr		device;

private :

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
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_ProxyDevice_H_

// ============================================================================

