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
//	TBFC.IO.BtpDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_BtpDevice_H_
#define _TBFC_Light_BtpDevice_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {
namespace IO {
TBFC_PTR_DECL(TBFC_Light_DLL,BtpDevice)
}
}

// ============================================================================

#include "TBFC.Base.Exception.h"

#include "TBFC.IO.ByteInputStream.h"
#include "TBFC.IO.ByteOutputStream.h"
#include "TBFC.IO.Device.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

/// \brief BTP (Buffer-based Transport Protocol) protocol implementation.
///
/// This class implements de BTP protocol.
/// When outputting buffers to the proxied device, an additional header is
/// appended at the beginning of the buffer, as represented below.
///
/// If n == sizeof(int) on this host, sends:
///
/// \verbatim
///		------------------------------------------------
///	. pos:	| 1 | 2    ...   n+1 |                         |
///		------------------------------------------------
///	. val:	| * |  input.length  |          input          |
///		--|---------------------------------------------
///		  `--- sizeof(int)
/// \endverbatim
///
/// Note that `input.length' is sent in little endian format.
///
/// When inputting buffers, the opposite action is performed.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL BtpDevice : public Device {

public :

	BtpDevice(
			ByteInputStreamPtr	idevice
	);

	BtpDevice(
			ByteOutputStreamPtr	odevice
	);

	virtual ~BtpDevice(
	);

	virtual void open(
	);

	virtual void close(
	);

	virtual Buffer getBytes(
	);

	virtual void putBytes(
		const	Buffer&			input
	);

	virtual void flush(
	);

	virtual Uint64 seek(
		const	Uint64			offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

	virtual Bool atEOS(
	) const;

protected :

private :

	// internal state...

	Uint64			index;
	ByteInputStreamPtr	idevice;
	ByteOutputStreamPtr	odevice;

	// others...

	BtpDevice(
		const	BtpDevice&
	);

	BtpDevice& operator = (
		const	BtpDevice&
	);

};

// ============================================================================

} // namespace IO
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_BtpDevice_H_

// ============================================================================

