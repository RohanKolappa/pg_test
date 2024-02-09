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
//	BFC.Net.UDPSocket.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.IO.WouldBlock.h"

#include "BFC.Net.IPv4Addr.h"
#include "BFC.Net.Packet.h"
#include "BFC.Net.SignalIgnorer.h"
#include "BFC.Net.SockAddr.h"
#include "BFC.Net.SocketData.h"
#include "BFC.Net.UDPSocket.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, UDPSocket )

// ============================================================================

Net::UDPSocket::UDPSocket(
	const	Bool		pReuseAddr,
	const	Bool		pAddPInfo ) :

	reuseAddr	( pReuseAddr ),
	addPInfo	( pAddPInfo ) {

	setObjectName( "BFC.Net.UDPSocket" );

}

Net::UDPSocket::~UDPSocket() {

	safeClose();

}

// ============================================================================

void Net::UDPSocket::openWrite(
	const	IPv4Address &	pDstAddr,
	const	Uint16 		pDstPort,
	const	Uint16		pSrcPort,
	const	Bool		pEnableBCast ) {

	open();

	// Now bind this socket to the user-specified port, or to
	// any port if none specified (pSrcPort == 0).

	setSelfSockAddr( new SockAddr( pSrcPort ) );

	setDfltSockAddr( new SockAddr( new IPv4Addr( pDstAddr.getUint() ), pDstPort ) );

	if ( pEnableBCast ) {
		setBroadcast( true );
	}

	if ( pDstAddr.isMulticast() ) {
		setTTL( 1 );
		setLoop( true );
	}

}

void Net::UDPSocket::openWrite(
		SockAddrCPtr	pDestAddr,
	const	Uint16		pSrcPort,
	const	Bool		pEnableBCast ) {

	open();

	// Now bind this socket to the user-specified port, or to
	// any port if none specified (pSrcPort == 0).

	setSelfSockAddr( new SockAddr( pSrcPort ) );

	setDfltSockAddr( pDestAddr );

	if ( pEnableBCast ) {
		setBroadcast( true );
	}

	if ( pDestAddr->getAddr()->isMulticast() ) {
		setTTL( 1 );
		setLoop( true );
	}

}

void Net::UDPSocket::openRead(
	const	Uint16		pSrcPort ) {

	open();

	setSelfSockAddr( new SockAddr( pSrcPort ) );

}

void Net::UDPSocket::openRead(
	const	IPv4Address &	addr,
	const	Uint16		port ) {

	if ( ! addr.isMulticast() ) {
		throw InvalidArgument( "Not a multicast address!" );
	}

	open();

	setSelfSockAddr( new SockAddr( port ) );

	// Now, attach to the multicast group...

	mcstAddr = new IPv4Addr( addr.getUint() );

	joinGroup( mcstAddr, IPAddr::getAnyLocal() );

}

void Net::UDPSocket::openMCast(
	const	IPAddrCPtr &	pMCastAddr,
	const	Uint16		pMCastPort ) {

	if ( ! pMCastAddr->isMulticast() ) {
		throw InvalidArgument( "Not a multicast address!" );
	}

	open();

	setSelfSockAddr( new SockAddr( pMCastAddr, pMCastPort ) );

	mcstAddr = pMCastAddr;

	joinGroup( pMCastAddr, IPAddr::getAnyLocal() );

	setDfltSockAddr( new SockAddr( pMCastAddr, pMCastPort ) );

	setTTL( 1 );
	setLoop( true );

}

void Net::UDPSocket::openMCast(
		SockAddrCPtr	pMCstAddr,
		NetIfaceCPtr	pNetIface ) {

	if ( ! pMCstAddr->getAddr()->isMulticast() ) {
		throw InvalidArgument( "Not a multicast address!" );
	}

	open();

	setSelfSockAddr( pMCstAddr );

	mcstAddr = pMCstAddr->getAddr();

	if ( pNetIface ) {
		joinGroup( pMCstAddr->getAddr(), pNetIface->getAddress() );
	}
	else {
		joinGroup( pMCstAddr->getAddr(), IPAddr::getAnyLocal() );
	}

	setDfltSockAddr( pMCstAddr );

	setTTL( 1 );
	setLoop( true );

}

// ============================================================================

void Net::UDPSocket::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	getSocketData()->openSD( SocketData::DGram );

	if ( ! getSocketData()->isValid() ) {
		throwSocketException( "create()" );
	}

	setReuseAddr( reuseAddr );
	setRecvBufSize( 1 << 20 );
	setAddPacketInfo( addPInfo );

	offset = 0;
	setAccess( ReadAccess | WriteAccess );

}

void Net::UDPSocket::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( mcstAddr ) {
		leaveGroup( mcstAddr, IPAddr::getAnyLocal() );
		mcstAddr.kill();
	}

	getSocketData()->closeSD();

	resetAccess();

}

// ============================================================================

void Net::UDPSocket::setSelfSockAddr(
		SockAddrCPtr	pSelfSockAddr ) {

	if ( selfSockAddr ) {
		throw InternalError( "Already bound!" );
	}

	getSocketData()->bindSD( pSelfSockAddr );

	selfSockAddr = getSocketData()->getSockName();

}

// ============================================================================

Buffer Net::UDPSocket::getBytes() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasReadAccess() ) {
		throw NoReadAccess();
	}

	if ( ! selfSockAddr ) {
		throw InternalError( "Not bound!" );
	}

	if ( hasGetTimeout() ) {
		Time::Delta d = getRelGetTimeout();
		if ( d <= 0 ) {
			throw IO::WouldBlock();
		}
		setRxTimeout( d );
	}
	else {
		resetRxTimeout();
	}

	const Uint32		toread = 0x10000;
	Uchar			buffer[ toread ];

	sockaddr_in		peerAddr;
	SocketLenType		pAddrLen = sizeof( peerAddr );

	int count = ::recvfrom(
		getSocketData()->getSD(),
		( Char * )buffer,
		( int )toread,
		0,
		( sockaddr * )&peerAddr,
		&pAddrLen );

	checkCode( count );

	lastSockAddr = new SockAddr(
		new IPv4Addr( ( Uint32 )peerAddr.sin_addr.s_addr ),
		ntohs( peerAddr.sin_port ) );

	offset += ( Uint64 )( Uint32 )count;

	return Buffer( buffer, count, true );

}

void Net::UDPSocket::putBytes(
	const	Buffer &	input ) {

	if ( ! nextSockAddr ) {
		if ( dfltSockAddr ) {
			nextSockAddr = dfltSockAddr;
		}
		else if ( lastSockAddr ) {
			nextSockAddr = lastSockAddr;
		}
		else {
			throw InternalError( "No destination specified!" );
		}
	}

	putDGram( input, nextSockAddr );

	nextSockAddr.kill();

}

void Net::UDPSocket::putDGram(
	const	Buffer &	input,
		Net::SockAddrCPtr
				pAddr ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasWriteAccess() ) {
		throw NoWriteAccess();
	}

	if ( ! pAddr->getAddr()->isIPv4() ) {
		throw InternalError( "Only IPv4 for now pls!" );
	}

	if ( hasPutTimeout() ) {
		Time::Delta d = getRelPutTimeout();
		if ( d <= 0 ) {
			throw IO::WouldBlock();
		}
		setTxTimeout( d );
	}
	else {
		resetTxTimeout();
	}

	const Uchar *	buffer = input.getCstPtr();
	const Uint32	towrite = input.getLength();

	sockaddr_in	target;

	target.sin_family	= AF_INET;
	target.sin_port		= htons( pAddr->getPort() );
	target.sin_addr.s_addr	= IPv4AddrCPtr( pAddr->getAddr() )->getUint32();

	int count;

	{
		SignalIgnorer signalIgnorer;
		count = ::sendto(
				getSocketData()->getSD(),
				( const Char * )buffer,
				towrite,
				0,
				( const sockaddr * )&target,
				sizeof( target ) );
	}

	checkCode( count );

	if ( ( Uint32 )count != towrite ) {
		safeClose();
		throw IO::BrokenPipe();
	}

	offset += ( Uint64 )( Uint32 )count;

	if ( ! selfSockAddr ) {
		selfSockAddr = getSocketData()->getSockName();
		msgWrn( "putBytes(): auto-bound to " + selfSockAddr->toBuffer() + "!" );
	}

}

// ============================================================================

Uint64 Net::UDPSocket::seek(
	const	Uint64		/* offset */ ) {

	throw NoRandomAccess();

}

Uint64 Net::UDPSocket::tell() const {

	return offset;

}

Uint64 Net::UDPSocket::length() const {

	return 0;

}

// ============================================================================

Net::PacketPtr Net::UDPSocket::getPacket() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasReadAccess() ) {
		throw NoReadAccess();
	}

	if ( ! selfSockAddr ) {
		throw InternalError( "Not bound!" );
	}

	if ( hasGetTimeout() ) {
		Time::Delta d = getRelGetTimeout();
		if ( d <= 0 ) {
			throw IO::WouldBlock();
		}
		setRxTimeout( d );
	}
	else {
		resetRxTimeout();
	}

	sockaddr_in		peerAddr;
	const Uint32		toread = 0x10000;
	Uchar			buffer[ toread ];
	SockAddrCPtr		tgtSAddr;
	int			count;

	if ( addPInfo ) {

		iovec			ioVector;
               
                ioVector.iov_base		= buffer;
		ioVector.iov_len		= toread;

		char			ctrlMesg[ 0x100 ];
		msghdr			mesgHder;

		mesgHder.msg_name		= &peerAddr;
		mesgHder.msg_namelen		= sizeof( peerAddr );
		mesgHder.msg_iov		= &ioVector;
		mesgHder.msg_iovlen		= 1;
		mesgHder.msg_control		= ctrlMesg;
		mesgHder.msg_controllen		= sizeof( ctrlMesg );
		mesgHder.msg_flags		= 0;

		count = ::recvmsg(
			getSocketData()->getSD(),
			&mesgHder,
			0 );

		checkCode( count );

		cmsghdr *		cmsg;
		in_pktinfo *		pcktInfo = 0;

		for ( cmsg = CMSG_FIRSTHDR( &mesgHder ) ; cmsg ; cmsg = CMSG_NXTHDR( &mesgHder, cmsg ) ) {
			if ( cmsg->cmsg_level == IPPROTO_IP
			  && cmsg->cmsg_type == IP_PKTINFO ) {
				pcktInfo = ( in_pktinfo * )CMSG_DATA( cmsg );
				break;
			}
		}

		if ( pcktInfo ) {
			tgtSAddr = new SockAddr(
				new IPv4Addr( ( Uint32 )pcktInfo->ipi_addr.s_addr ),
				selfSockAddr->getPort() );
		}

	}
	else {

		SocketLenType		pAddrLen = sizeof( peerAddr );

		count = ::recvfrom(
			getSocketData()->getSD(),
			( Char * )buffer,
			( int )toread,
			0,
			( sockaddr * )&peerAddr,
			&pAddrLen );

		checkCode( count );

	}

	lastSockAddr = new SockAddr(
		new IPv4Addr( ( Uint32 )peerAddr.sin_addr.s_addr ),
		ntohs( peerAddr.sin_port ) );

	offset += ( Uint64 )( Uint32 )count;

	Buffer		pcktData( buffer, count, true );

	return new Packet( pcktData, lastSockAddr, tgtSAddr ? tgtSAddr : selfSockAddr );

}

void Net::UDPSocket::putPacket(
		PacketCPtr	pPacket ) {

	if ( pPacket->getPeerAddr() ) {
		putDGram( pPacket->getBuffer(), pPacket->getPeerAddr() );
	}
	else {
		putBytes( pPacket->getBuffer() );
	}

}

// ============================================================================

void Net::UDPSocket::setTTL(
	const	Uint32		ttl ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	// If there are any socket option, ...

	Uint32 optval = ttl;

	if ( ::setsockopt(
			getSocketData()->getSD(),
#if defined( PLATFORM_DARWIN )
			IPPROTO_IP,
			IP_TTL,
#else
			IPPROTO_IP,
			IP_MULTICAST_TTL,
#endif
			( const char * )&optval,
			sizeof( optval ) ) == -1 ) {
		throwSocketException( "setsockopt( IP_TTL )" );
	}

}

void Net::UDPSocket::setLoop(
	const	Bool		pYesNo ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	Uint32 optval = ( pYesNo ? 1 : 0 );

	if ( ::setsockopt(
			getSocketData()->getSD(),
#if defined( PLATFORM_DARWIN )
			IPPROTO_IP,
			IP_LOOP,
#else
			IPPROTO_IP,
			IP_MULTICAST_LOOP,
#endif
			( const char * )&optval,
			sizeof( optval ) ) == -1 ) {
		throwSocketException( "setsockopt( IP_LOOP )" );
	}

}

// ============================================================================

void Net::UDPSocket::setRecvBufSize(
	const	Uint32		pSize ) {

	// [2006-04-27 SLAS]: Too much packets are lost when using the default
	// receive buffer size on Windows.

	int optval = pSize;

	if ( ::setsockopt(
			getSocketData()->getSD(),
			SOL_SOCKET,
			SO_RCVBUF,
			( const char * )&optval,
			sizeof( optval ) ) == SocketError ) {
		msgWrn( "Can't increase receive buffer size!" );
		msgWrn( ::strerror( getErrno() ) );
	}

}

void Net::UDPSocket::setBroadcast(
	const	Bool		pYesNo ) {

	int optval = ( pYesNo ? 1 : 0 );

	if ( ::setsockopt(
			getSocketData()->getSD(),
			SOL_SOCKET,
			SO_BROADCAST,
			( const char * )&optval,
			sizeof( optval ) ) == SocketError ) {
		msgWrn( "Can't enable broadcast!" );
		msgWrn( ::strerror( getErrno() ) );
	}

}

void Net::UDPSocket::setReuseAddr(
	const	Bool		pYesNo ) {

	int optval = ( pYesNo ? 1 : 0 );

	if ( ::setsockopt(
			getSocketData()->getSD(),
			SOL_SOCKET,
			SO_REUSEADDR,
			( const char * )&optval,
			sizeof( optval ) ) == SocketError ) {
		msgWrn( "Can't enable reuse address!" );
		msgWrn( ::strerror( getErrno() ) );
	}

}

void Net::UDPSocket::setAddPacketInfo(
	const	Bool		pYesNo ) {

	int optval = ( pYesNo ? 1 : 0 );

	if ( ::setsockopt(
			getSocketData()->getSD(),
			IPPROTO_IP,
			IP_PKTINFO,
			( const char * )&optval,
			sizeof( optval ) ) == SocketError ) {
		msgWrn( "Can't add packet info!" );
		msgWrn( ::strerror( getErrno() ) );
	}

}

// ============================================================================

void Net::UDPSocket::joinGroup(
		IPAddrCPtr	pAddress,
		IPAddrCPtr	pInterface ) {

	if ( ! pAddress ) {
		throw InvalidArgument( "Missing address!" );
	}

	if ( ! pAddress->isMulticast() ) {
		throw InvalidArgument( "Not a multicast address!" );
	}

	if ( pAddress->isIPv4() ) {

		Uint32	addr = IPv4AddrCPtr( pAddress )->getUint32();
		Uint32	ifce = IPv4AddrCPtr( pInterface )->getUint32();

#if defined( PLATFORM_WIN32 )

		struct ip_mreq mreq;

		mreq.imr_multiaddr.s_addr = addr;
		mreq.imr_interface.s_addr = ifce;

		int err;

		if ( ( err = ::setsockopt(
				getSocketData()->getSD(),
				IPPROTO_IP,
				IP_ADD_MEMBERSHIP,
				( const char * )&mreq,
				sizeof( struct ip_mreq ) ) ) < 0 ) {
			throwSocketException( "setsockopt( IP_ADD_MEMBERSHIP )" );
		}

#elif defined( PLATFORM_DARWIN )

		struct ip_mreq mreq;

		mreq.imr_multiaddr.s_addr = addr;
		mreq.imr_interface.s_addr = ifce;

		if ( ::setsockopt(
				getSocketData()->getSD(),
				IPPROTO_IP,
				IP_ADD_MEMBERSHIP,
				( const void * )&mreq,
				sizeof( struct ip_mreq ) ) == -1 ) {
			throw CantSetSockOpt( ::strerror( getErrno() ) );
		}

#else

		struct ip_mreqn mreq;

		mreq.imr_multiaddr.s_addr = addr;
		mreq.imr_address.s_addr = ifce;
		mreq.imr_ifindex = 0;

		if ( ::setsockopt(
				getSocketData()->getSD(),
				SOL_IP,
				IP_ADD_MEMBERSHIP,
				( const void * )&mreq,
				sizeof( struct ip_mreqn ) ) == -1 ) {
			throwSocketException( "setsockopt( IP_ADD_MEMBERSHIP )" );
		}

#endif

	}

	else if ( pAddress->isIPv6() ) {
		throw NotImplemented( "IPv6 not implemented yet!" );
	}

	else {
		throw InvalidArgument( "Family not supported!" );
	}

}

void Net::UDPSocket::leaveGroup(
		IPAddrCPtr	pAddress,
		IPAddrCPtr	pInterface ) {

	if ( ! pAddress ) {
		throw InvalidArgument( "Missing address!" );
	}

	if ( ! pAddress->isMulticast() ) {
		throw InvalidArgument( "Not a multicast address!" );
	}

	if ( pAddress->isIPv4() ) {

		Uint32	addr = IPv4AddrCPtr( pAddress )->getUint32();
		Uint32	ifce = IPv4AddrCPtr( pInterface )->getUint32();

#if defined( PLATFORM_WIN32 )

		struct ip_mreq mreq;

		mreq.imr_multiaddr.s_addr = addr;
		mreq.imr_interface.s_addr = ifce;

		::setsockopt(
			getSocketData()->getSD(),
			IPPROTO_IP,
			IP_DROP_MEMBERSHIP,
			( const char * )&mreq,
			sizeof( struct ip_mreq ) );

#elif defined( PLATFORM_DARWIN )

		struct ip_mreq mreq;

		mreq.imr_multiaddr.s_addr = addr;
		mreq.imr_interface.s_addr = ifce;

		::setsockopt(
			getSocketData()->getSD(),
			IPPROTO_IP,
			IP_DROP_MEMBERSHIP,
			( const void * )&mreq,
			sizeof( struct ip_mreq ) );

#else

		struct ip_mreqn mreq;

		mreq.imr_multiaddr.s_addr = addr;
		mreq.imr_address.s_addr = ifce;
		mreq.imr_ifindex = 0;

		::setsockopt(
			getSocketData()->getSD(),
			SOL_IP,
			IP_DROP_MEMBERSHIP,
			( const void * )&mreq,
			sizeof( struct ip_mreqn ) );

#endif

	}

	else if ( pAddress->isIPv6() ) {
		throw NotImplemented( "IPv6 not implemented yet!" );
	}

	else {
		throw InvalidArgument( "Family not supported!" );
	}

}

// ============================================================================

