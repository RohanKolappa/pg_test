// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.RTPHandler.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "VMP.RTP.RTPHandler.h"
#include "VMP.RTP.TPacket.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, RTPHandler )

// ============================================================================

RTP::RTPHandler::RTPHandler() {

	setObjectName( "VMP.RTP.RTPHandler" );

	reset();

}

RTP::RTPHandler::~RTPHandler() {

	if ( socket ) {
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "Can't auto-close!" );
		}
	}

}

// ============================================================================

void RTP::RTPHandler::useRTCP(
		RTCPHandlerPtr		__rtcpHandler ) {

	rtcpHandler = __rtcpHandler;

}

// ============================================================================

void RTP::RTPHandler::open(
	const	Uint16		port ) {

	socket = new Net::UDPSocket;

	socket->openRead( port );

}

void RTP::RTPHandler::open(
	const	Net::IPv4Address&	addr,
	const	Uint16		port ) {

	socket = new Net::UDPSocket;

	socket->openRead( addr, port );

}

void RTP::RTPHandler::openMCast(
	const	Net::IPAddrCPtr &
				addr,
	const	Uint16		port ) {

	socket = new Net::UDPSocket;

	socket->openMCast( addr, port );

}

void RTP::RTPHandler::open(
		Net::UDPSocketPtr	s ) {

	socket = s;

}

void RTP::RTPHandler::reset() {

	nextSeqNbr = (Uint16)-1;
	SSRC = (Uint32)-1;
	PT = (Uint32)-1;

}

void RTP::RTPHandler::close() {

	if ( ! socket ) {
		return;
	}

	socket->close();

	socket.kill();

}

// ============================================================================

Buffer RTP::RTPHandler::getBytes() {

	return getPacket()->data;

}

BytesFramePtr RTP::RTPHandler::getFrame() {

	TPacketCPtr	p = getPacket();

	BytesFramePtr	b = new BytesFrame( p->data );

	b->setPTS( p->TS );

	return b;

}

RTP::TPacketCPtr RTP::RTPHandler::getPacket() {

//	msgDbg( "getBytes(): reading..." );

	Buffer		buf;

	try {
		buf = socket->getBytes();
	}
	catch ( IO::EndOfStream & ) {
		throw EndOfStream();
	}
	catch ( Exception & e ) {
		msgExc( e, "getBytes(): can't read!" );
		socket.kill();
		throw BrokenSession();
	}

	const Uchar *	ptr = buf.getCstPtr();
	Uint32		len = buf.getLength();

	if ( len < 12 ) {
		throw InternalError( "RTP header length < 12!" );
	}

	Uchar c = *ptr;

	if ( ( c & 0xC0 ) != 0x80 ) {
		throw InternalError( "RTP not version 2!" );
	}

	if ( c & 0x20 ) {
		throw InternalError( "Padding not supported!" );
	}

	if ( c & 0x10 ) {
		throw InternalError( "Extension not supported!" );
	}

	Uint32		cc	= ( ( Uint32 )( c & 0x0F ) << 2 );
	Uint32		ssrc	= *( const Uint32 * )( ptr + 8 );
	Uint32		pt	= ( ptr[ 1 ] & 0x7F );

	if ( SSRC == ( Uint32 )-1 ) {
		SSRC = ssrc;
		PT = pt;
		msgDbg( "getBytes(): acquired SSRC: "
			+ Buffer( System::swap( SSRC ), Buffer::Base16, 8 )
			+ ", pt: " + Buffer( pt ) );
		if ( rtcpHandler ) {
			rtcpHandler->setSSRC( SSRC );
		}
	}
	else if ( SSRC != ssrc ) {
		throw InternalError( "Multiple SSRCs on this addr/port!" );
	}
	else if ( PT != pt ) {
		throw InternalError( "Multiple PTs on this addr/port!" );
	}

	Bool	mFlag = ( ptr[ 1 ] & 0x80 ) != 0;
	Uint16	seqNbr = ( ( Uint16 )( ptr[ 2 ] ) << 8 )
		       | ( ( Uint16 )( ptr[ 3 ] )      );
	Uint32	ts	= ( ( Uint32 )( ptr[ 4 ] ) << 24 )
			| ( ( Uint32 )( ptr[ 5 ] ) << 16 )
			| ( ( Uint32 )( ptr[ 6 ] ) <<  8 )
			| ( ( Uint32 )( ptr[ 7 ] )       );

	if ( nextSeqNbr == ( Uint16 ) - 1 ) {
		msgDbg( "getBytes(): acquired seq_nbr: " + Buffer( seqNbr ) );
		nextSeqNbr = seqNbr + 1;
	}
	else if ( nextSeqNbr != seqNbr ) {
		msgWrn( "lost sync! (diff: " + Buffer( seqNbr - nextSeqNbr ) + ")" );
		nextSeqNbr = seqNbr + 1;
	}
	else {
		nextSeqNbr++;
	}

	len -= 12;
	ptr += 12;

	if ( len < cc ) {
		throw InternalError( "RTP header length < 12 + CSRCs!" );
	}

	len -= cc;
	ptr += cc;

	TPacketPtr	res = new TPacket;

	res->M		= mFlag;
	res->PT		= PT;
	res->SN		= seqNbr;
	res->TS		= Time::Clock( ( Uint64 )ts * 100 / 9 );
	res->SSRC	= SSRC;
	res->data	= buf( buf.getLength() - len );

	return res;

}

// ============================================================================

