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
//	BFC.IO.AutoCloser.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_AutoCloser_H_
#define _BFC_IO_AutoCloser_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.IO.Device.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_IO

class BFC_IO_DLL AutoCloser {

public :

	/// \brief Creates a new AutoCloser.

	AutoCloser(
			DevicePtr	pDevice
	) :
		device( pDevice ) {
	}

	/// \brief Destroys this object.

	~AutoCloser(
	);

	/// \brief Detaches the attached device, 

	void forget(
	) {
		device.kill();
	}

protected :

private :

	DevicePtr	device;

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_AutoCloser_H_

// ============================================================================

