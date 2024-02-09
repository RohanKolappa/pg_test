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
//	BFC.Net.TCPSocket.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_TCPSocket_H_
#define _BFC_TCPSocket_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.TCPSocketPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.SockAddr.h"
#include "BFC.Net.Socket.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

class SocketData;

// ============================================================================

/// \brief A simple TCP client socket implementation.
///
/// \ingroup BFC_Net
///
/// This class implements the basic services anyone expects from a TCP
/// client socket, although some enhanced features are also available
/// (KeepAlive option, ...).

class BFC_Net_DLL TCPSocket : public Socket {

public :

	/// \brief Creates a TCP socket that will connect to \a addr at port
	///	\a port when open()ed.

	TCPSocket(
		const	IPv4Address &		addr,
		const	Uint16			port
	);

	/// \brief Creates a TCP socket that will connect to peer \a pPeerAddr.

	TCPSocket(
			SockAddrCPtr	pPeerAddr,
			SockAddrCPtr	pSelfAddr = SockAddrCPtr()
	);

private :

	friend class TCPSocketProvider;

	TCPSocket(
			SocketData *		localData,
		const	Uint16			peerPort,	// HBO
		const	IPv4Address &		peerAddr,
		const	Uint16			selfPort,	// HBO
		const	IPv4Address &		selfAddr
	);

public :

	/// \brief Destroys this object.

	virtual ~TCPSocket(
	);

#if ! defined( PLATFORM_WIN32 )

	/// \brief This method gives access to the kernel configuration.
	///
	/// This method modifies the values used by the kernel to implement
	/// the RFCxxxx KeepAlive mechanism.
	///
	/// \note You must be super-user to call this method.
	///
	/// \note As of writing this doc, the kernel doesn't support time
	///	< 75 seconds.
	///
	/// \throws MustBeRoot
	///
	/// \throws OperationNotSupported
	///
	/// \throws InternalError

	static void setKernelKeepAliveOptions(
		const	Uint32		probes,
		const	Uint32		time
	);

	/// \brief Reads kernel "KeepAlive" configuration parameters.
	///
	/// \throws OperationNotSupported
	///
	/// \throws InternalError

	static void getKernelKeepAliveOptions(
			Uint32 &	probes,
			Uint32 &	time
	);

#endif

	/// \brief Connects to the server specified in the ctor.
	///
	/// \throws AlreadyConnected
	///	If this socket has already been open()ed.
	///
	/// \throws BadAddress
	///	If the IPv4 address set in the ctor is not valid.
	///
	/// \throws CantCreateSocket
	///	If the system refused to create a new TCP socket.
	///
	/// \throws CantOpen
	///	If the connection could not be made.
	///
	/// \throws CantSystemGetSockName
	///	If our own parameters could not been found after a successful
	///	connection has been made.

	virtual void open(
	);

	/// \brief Disconnects from the server.
	///
	/// Any pending operation is cancelled, meaning any thread blocked in
	/// the getBytes() or the putBytes() method is unblocked.
	///
	/// \throws NotConnected
	///	If this socket has not been open()ed.

	virtual void close(
	);

	/// \brief [Not for Win32] Turns the KeepAlive mechanism on or off.
	///
	/// \throws NotConnected
	///	If this socket has not been open()ed.
	///
	/// \throws CantSystemSetSockOpt
	///	System error.

	void setKeepAlive(
		const	Bool		enabled
	);

	/// \brief Turns the TCP_NODELAY mechanism on or off.
	///
	/// \throws NotConnected
	///	If this socket has not been open()ed.
	///
	/// \throws CantSystemSetSockOpt
	///	System error.

	void setNoDelay(
		const	Bool		enabled
	);

	void setNonBlocking(
		const	Bool		enabled
	);

	Uint16 getPeerPort(
	) const;

	IPv4Address getPeerAddr(
	) const;

	SockAddrCPtr getPeerSock(
	) const {
		return peerSock;
	}

	Uint16 getSelfPort(
	) const;

	IPv4Address getSelfAddr(
	) const;

	SockAddrCPtr getSelfSock(
	) const {
		return selfSock;
	}

	Bool hasData(
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

	virtual PacketPtr getPacket(
	);

	virtual void putPacket(
			PacketCPtr	pPacket
	);

protected :

private :

	SockAddrCPtr	selfSock;	///< Self IP address & port.
	SockAddrCPtr	peerSock;	///< Peer IP address & port.
	Uint64		offset;

	/// \brief Forbidden default constructor.

	TCPSocket(
	);

	/// \brief Forbidden copy constructor.

	TCPSocket(
		const	TCPSocket &
	);

	/// \brief Forbidden copy operator.

	TCPSocket & operator = (
		const	TCPSocket &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_TCPSocket_H_

// ============================================================================

