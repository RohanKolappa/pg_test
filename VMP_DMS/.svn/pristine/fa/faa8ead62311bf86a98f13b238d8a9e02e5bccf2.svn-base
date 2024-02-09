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
//	BFC.Net.Socket.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Socket_H_
#define _BFC_Socket_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.SocketPtr.h"

// ============================================================================

#include "BFC.IO.FinalDevice.h"

#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.MACAddress.h"
#include "BFC.Net.PacketPtr.h"

#include "BFC.TL.BufferArray.h"

#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

class SocketData;

// ============================================================================

/// \brief Base class of UDPSocket, TCPSocket, ....
///
/// \ingroup BFC_Net

class BFC_Net_DLL Socket : public IO::FinalDevice {

protected :

	Socket(
			SocketData *	pSocketData = 0
	);

public :

	virtual ~Socket(
	);

#if defined( PLATFORM_WIN32 )

	/// \brief [Win32] Initializes "wsock32.dll".
	///
	/// You don't need to use this method yourself, as it is automatically
	/// called upon loading of the BFC_Net.dll!

	static Bool initWinsock(
	);

	/// \brief [Win32] Deinitializes "wsock32.dll".
	///
	/// You don't need to use this method yourself, as it is automatically
	/// called upon unloading of the BFC_Net.dll!

	static void exitWinsock(
	);

#else

	static BufferArray getDevices(
	);

	static MACAddress getMACAddress(
		const Buffer		& deviceName
	);

	static IPv4Address getIPAddress(
		const Buffer		& deviceName
	);

#endif

	virtual PacketPtr getPacket(
	) = 0;

	virtual void putPacket(
			PacketCPtr	pPacket
	) = 0;

	/// \brief Don't mess with this one... You've been warned!

	int getSD(
	) const;

protected :

	SocketData * getSocketData(
	) const {
		return ld;
	}

	void setRxTimeout(
		const	Time::Delta &	pDelta
	);

	void resetRxTimeout(
	) {
		setRxTimeout( Time::Delta( 0, 0 ) );
	}

	void setTxTimeout(
		const	Time::Delta &	pDelta
	);

	void resetTxTimeout(
	) {
		setTxTimeout( Time::Delta( 0, 0 ) );
	}

	void checkCode(
		const	Int32		pCode
	);

	static void throwSocketException(
		const	Buffer &	pPrefix,
		const	Bool		pStrErrno = true
	);

private :

	SocketData *	ld;

#if defined( PLATFORM_WIN32 )
	static Bool	initialized;
#endif

	/// \brief Forbidden copy constructor.

	Socket(
		const	Socket &
	);

	/// \brief Forbidden copy operator.

	Socket & operator = (
		const	Socket &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Socket_H_

// ============================================================================

