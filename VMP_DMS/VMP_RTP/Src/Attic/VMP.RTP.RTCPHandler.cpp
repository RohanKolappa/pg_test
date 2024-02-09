// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.RTCPHandler.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.System.h"
#include "BFC.Math.Utils.h"
#include "BFC.Net.UDPSocket.h"

#include "VMP.RTP.RTCPHandler.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, RTCPHandler )

// ============================================================================

RTP::RTCPHandler::RTCPHandler() {

	setObjectName( "VMP.RTP.RTCPHandler" );

}

RTP::RTCPHandler::~RTCPHandler() {

	if ( isRunning() ) {
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "Can't auto-close!" );
		}
	}

}

// ============================================================================

void RTP::RTCPHandler::open(
	const	Net::IPv4Address&	serverAddr,
	const	Uint16			serverPort,
	const	Uint16			clientPort ) {

	socket = new Net::UDPSocket;

	socket->openWrite( serverAddr, serverPort, clientPort );

	start();

	self = Math::rand( (Uint32)-1 );

}

void RTP::RTCPHandler::open(
		Net::UDPSocketPtr	s ) {

	socket = s;

	start();

	self = Math::rand( (Uint32)-1 );

}

void RTP::RTCPHandler::close() {

	try {
		stop();
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't stop internal thread!" );
	}

	try {
		Exception *exc = join();
		if ( exc ) {
			msgExc( *exc, "Internal thread exception!" );
			delete exc;
		}
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't join internal thread!" );
	}

	socket.kill();

}

// ============================================================================

void RTP::RTCPHandler::setSSRC(
	const	Uint32		__ssrc ) {

	ssrc = __ssrc;

}

// ============================================================================

void RTP::RTCPHandler::run() {

	while ( ! shouldStop() ) {

		// Forge packet, and send it...

		socket->putBytes( forgePacket() );

		// Sleep a while...

// FIXME: Be RFC 3550 compliant!

		System::sleep( 5 );

	}

}

// ============================================================================

Buffer RTP::RTCPHandler::forgePacket() {

	char packet[ 52 + 1 ] =
	"\x81"			// Version: 2, Reception report count: 1
	"\xC9"			// Type: receiver report (201)
	"\x00\x07"		// Length: 7
	"\x00\x00\x00\x00"	// My SSRC: XXXX
				// Source 1
	"\x00\x00\x00\x00"	// Server SSRC: XXXX
	"\x00"			// Fraction lost: 0 / 256
	"\x00\x00\x00"		// Cumulative number of packets lost: 0
	"\x00\x00"		// Sequence number cycles count: 0
	"\x00\x00"		// Highest sequence number received: XXXX
	"\x00\x00\x06\x44"	// Interarrival jitter: 1604
	"\x00\x00\x00\x00"	// Last ST timestamp: XXXX
	"\x00\x00\x00\x00"	// Delay since last SR timestamp: XXXX

	"\x81"			// Version: 2, Reception report count: 1
	"\xCA"			// Type: source description (202)
	"\x00\x04"		// Length: 4
				// NPacket 1, My SSRC
	"\x00\x00\x00\x00"	// My SSRC: XXXX
				// SDES items
	"\x01"			// Type: CNAME (user and domain) (1)
	"\x08"			// Length: 8
	"< r00t >"		// Text:
	"\x00"			// Type: END (0)
	"\x00"			// <padding>
	;

	*(Uint32 *)( packet +  4 ) = self;
	*(Uint32 *)( packet +  8 ) = ssrc;
	*(Uint32 *)( packet + 36 ) = self;

	return Buffer( (const Uchar *)packet, 52 );

}

// ============================================================================

