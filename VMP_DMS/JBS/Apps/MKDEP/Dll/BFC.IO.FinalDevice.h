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
//	BFC.IO.FinalDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_FinalDevice_H
#define _BFC_IO_FinalDevice_H

// ============================================================================

#include "BFC.Base.SPtr.h"
#include "BFC.IO.Device.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

BFC_PTR_DECL(JBS_DLL_NONE,FinalDevice)

// ============================================================================

/// \brief Empty class, used as parent for all final devices (i.e. not
///	chainable).
///
/// The FinalDevice objects are those that interact directly with some external
/// objects (File, MemoryInputStream, ...), in opposition to the ProxyDevice
/// objects, which interact with another Device (ByteInputStream, ...).
///
/// \ingroup Common_IO

class FinalDevice : public Device {

public :

	FinalDevice(
	);

	virtual ~FinalDevice(
	);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("BFC.IO.FinalDevice");

	CLASS_EXCEPTION(CantSystemClose, "Can't call ::close()");
	CLASS_EXCEPTION(CantSystemConnect, "Can't call ::connect()");
	CLASS_EXCEPTION(CantSystemFstat, "Can't call ::fstat()");
	CLASS_EXCEPTION(CantSystemOpen, "Can't call ::open()");
	CLASS_EXCEPTION(CantSystemRead, "Can't call ::read()");
	CLASS_EXCEPTION(CantSystemRecv, "Can't call ::recv()");
	CLASS_EXCEPTION(CantSystemRemove, "Can't call ::remove()");
	CLASS_EXCEPTION(CantSystemSeek, "Can't call ::seek()");
	CLASS_EXCEPTION(CantSystemSend, "Can't call ::send()");
	CLASS_EXCEPTION(CantSystemSocket, "Can't call ::socket()");
	CLASS_EXCEPTION(CantSystemStat, "Can't call ::stat()");
	CLASS_EXCEPTION(CantSystemStatfs, "Can't call ::statfs()");
	CLASS_EXCEPTION(CantSystemTell, "Can't call ::tell()");
	CLASS_EXCEPTION(CantSystemWrite, "Can't call ::write()");

protected :

private :

	// others...

	FinalDevice(
		const	FinalDevice&
	);

	FinalDevice& operator = (
		const	FinalDevice&
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_FinalDevice_H

// ============================================================================

