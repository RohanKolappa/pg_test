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
//	BFC.Net.UDPSocket.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_UDPSocket_H_
#define _BFC_UDPSocket_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

#include "BFC.Net.UDPSocketPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Net.IPAddrPtr.h"
#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.NetIfacePtr.h"
#include "BFC.Net.SockAddrPtr.h"
#include "BFC.Net.Socket.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief A simple UDP socket implementation.
///
/// This class implements the basic services anyone expects from an UDP
/// socket.
///
/// \ingroup BFC_Net

class BFC_Net_DLL UDPSocket : public Socket {

public :

	UDPSocket(
		const	Bool		pReuseAddr = true,
		const	Bool		pAddPInfo = true
	);

	virtual ~UDPSocket(
	);

	static Uint16 anyPort(
	) {
		return 0;
	}

	/// \brief Opens this object for sending datagrams exclusively to the
	///	destination identified by \a pDstAddr and \a pDstPort, and
	///	binds it local port \a pLocPort if not equal to anyPort().

	void openWrite(
		const	IPv4Address &	pDstAddr,
		const	Uint16		pDstPort,
		const	Uint16		pLocPort = anyPort(),
		const	Bool		enableBroadcast = false
	);

	/// \brief Opens this object for sending datagrams exclusively to the
	///	destination identified by \a pDstAddr and \a pDstPort, and
	///	binds it local port \a pLocPort if not equal to anyPort().

	void openWrite(
			SockAddrCPtr	pDestAddr,
		const	Uint16		pLocPort = anyPort(),
		const	Bool		enableBroadcast = false
	);

	/// \brief Opens this object and binds it to the local "any" address,
	///	and local port \a pLocPort.
	///
	/// Every time a datagram is read from this object, the sender
	/// address and sender port will be copied to the destination address
	/// and port to use when writing datagrams.

	void openRead(
		const	Uint16		pLocPort
	);

	/// \brief Opens this object and binds it to the local address
	///	\a pLocAddr and local port \a pLocPort.
	///
	/// Every time a datagram is read from this object, the sender
	/// address and sender port will be copied to the destination address
	/// and port to use when writing datagrams.

	void openRead(
		const	IPv4Address &	pLocAddr,
		const	Uint16		pLocPort
	);

	/// \brief Initializes this object to perform typical read/write
	///	multicast operations.
	///
	/// This method does the following:
	/// -	bind this socket to all addresses and \a pLocalPort,
	/// -	join multicast group specified by \a pLocAddr,
	/// -	set default destination address to (\a pLocAddr, \a pLocPort).

	void openMCast(
		const	IPAddrCPtr &	pMCastAddr,
		const	Uint16		pMCastPort
	);

	/// \brief Initializes this object to perform typical read/write
	///	multicast operations.
	///
	/// This method does the following:
	/// -	set default destination address to \a pMCstAddr,
	/// -	bind this socket to all addresses and a random port,
	/// -	join multicast group specified by \a pMCstAddr on the system
	///	default inteface, or on the interface specified by
	///	\a pNetIface.

	void openMCast(
			SockAddrCPtr	pMCstAddr,
			NetIfaceCPtr	pNetIface = NetIfaceCPtr()
	);

	virtual void open(
	);

	virtual void close(
	);

	/// \brief Sets the SockAddr of this UDPSocket.
	///
	/// When a UDPSocket is created, it exists in a name space (address
	/// family) but has no address assigned to it. Calling this method
	/// assigns the address specified to by \a pSelfSockAddr to this
	/// object. Traditionally, this operation is called "assigning a name
	/// to a socket", or "binding a socket".
	///
	/// It is normally necessary to assign a local address using this
	/// method before an UDPSocket may receive packets.
	///
	/// \param pSelfSockAddr
	///	The SockAddr of this UDPSocket.

	void setSelfSockAddr(
			SockAddrCPtr	pSelfSockAddr
	);

	/// \brief Returns the SockAddr of this UDPSocket.
	///
	/// \returns
	///	The SockAddr of this UDPSocket.

	SockAddrCPtr getSelfSockAddr(
	) const {
		return selfSockAddr;
	}

	/// \brief Sets the default destination SockAddr of all outgoing
	///	packets.
	///
	/// \param pDfltSockAddr
	///	The default destination SockAddr of all next outgoing packets.

	void setDfltSockAddr(
			SockAddrCPtr	pDfltSockAddr
	) {
		dfltSockAddr = pDfltSockAddr;
	}

	/// \brief Returns the default destination SockAddr of all outgoing
	///	packets.
	///
	/// \returns
	///	The default destination SockAddr of all next outgoing packets.

	SockAddrCPtr getDfltSockAddr(
	) const {
		return dfltSockAddr;
	}

	/// \brief Sets the destination SockAddr of the next outgoing packet.
	///
	/// \param pNextSockAddr
	///	The destination SockAddr of the next outgoing packet.

	void setNextSockAddr(
			SockAddrCPtr	pNextSockAddr
	) {
		nextSockAddr = pNextSockAddr;
	}

	/// \brief Returns the destination SockAddr of the next outgoing
	///	packet.
	///
	/// \returns
	///	The destination SockAddr of the next outgoing packet.

	SockAddrCPtr getNextSockAddr(
	) const {
		return nextSockAddr;
	}

	/// \brief Returns the source SockAddr of the last incoming packet.
	///
	/// \returns
	///	The source SockAddr of the last incoming packet.

	SockAddrCPtr getLastSockAddr(
	) const {
		return lastSockAddr;
	}

	virtual Buffer getBytes(
	);

	virtual void putBytes(
		const	Buffer &	input
	);

	void putDGram(
		const	Buffer &	pData,
			SockAddrCPtr	pAddr
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

	void setTTL(
		const	Uint32		ttl
	);

	void setLoop(
		const	Bool		pYesNo
	);

	void setRecvBufSize(
		const	Uint32		pSize
	);

	void setBroadcast(
		const	Bool		pYesNo
	);

protected :

	void setReuseAddr(
		const	Bool		pYesNo
	);

	void setAddPacketInfo(
		const	Bool		pYesNo
	);

	void joinGroup(
			IPAddrCPtr	pAddress,
			IPAddrCPtr	pInterface
	);

	void leaveGroup(
			IPAddrCPtr	pAddress,
			IPAddrCPtr	pInterface
	);

private :

	Bool		reuseAddr;	///< Try to reuse address when binding ?
	Bool		addPInfo;	///< Try to add packet info ?
	IPAddrCPtr	mcstAddr;	///< Multicast group we joined, if any.
	SockAddrCPtr	selfSockAddr;	///< Local socket "address" (i.e. address + port).
	SockAddrCPtr	dfltSockAddr;	///< Default destination address.
	SockAddrCPtr	nextSockAddr;	///< Target address of next packet.
	SockAddrCPtr	lastSockAddr;	///< Sender address of last received packet.

	Uint64		offset;

	/// \brief Forbidden copy constructor.

	UDPSocket(
		const	UDPSocket &
	);

	/// \brief Forbidden copy operator.

	UDPSocket& operator = (
		const	UDPSocket &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_UDPSocket_H_

// ============================================================================

