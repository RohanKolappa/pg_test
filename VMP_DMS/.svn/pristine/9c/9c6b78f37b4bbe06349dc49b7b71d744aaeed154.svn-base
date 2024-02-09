// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.RTSPHandler.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.BufferIO.h"
#include "BFC.Base.BufferParser.h"
#include "BFC.Base.FastCSLocker.h"

#include "BFC.Math.Utils.h"

#include "VMP.RTP.HTTPAnswer.h"
#include "VMP.RTP.RTSPHandler.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, RTSPHandler )

// ============================================================================

RTP::RTSPHandler::RTSPHandler() {

	setObjectName( "VMP.RTP.RTSPHandler" );

}

RTP::RTSPHandler::~RTSPHandler() {



}

// ============================================================================

BytesSessionPtr RTP::RTSPHandler::open(
	const	Buffer &		rtspUrl,
		Net::UDPSocketPtr &	rtpSocket,
		Net::UDPSocketPtr &	rtcpSocket ) {

//	msgDbg( "Opening: " + rtspUrl );

	// Extract IP from URL...

	if ( ! rtspUrl.beginsWith( "rtsp://" ) ) {
		throw InvalidData( "Invalid URL!" );
	}

	Buffer addrPortBuf = BufferParser::tokenize( rtspUrl( 7 ), "/" )[ 0 ];

//	msgDbg( "Address: " + addrPortBuf );

	BufferArray addrTokens = BufferParser::tokenize( addrPortBuf, ":" );

	Buffer	addrBuf;
	Uint16	srvPort;

	if ( addrTokens.size() == 1 ) {
		addrBuf = addrTokens[ 0 ];
		srvPort = RTSPDefaultPort;
	}
	else if ( addrTokens.size() == 2 ) {
		addrBuf = addrTokens[ 0 ];
		srvPort = addrTokens[ 1 ].toUint16();
	}
	else {
		throw InvalidData( "Invalid server addr/port!" );
	}

	Net::IPv4Address srvAddr = addrBuf;

//	if ( ! srvAddr.isOK() ) {
//		throw InvalidData( "Invalid IPv4 address!" );
//	}

	// Open TCP connection to RTSP server...

	Net::TCPSocketPtr tmpSocket = new Net::TCPSocket( srvAddr, srvPort );

	try {
		tmpSocket->open();
	}
	catch ( Exception& e ) {
		throw InvalidData( e, "Can't connect to server!" );
	}

	Uint32	CSeq = 10;

	// Do an "OPTIONS"...

	tmpSocket->putBytes(
		"OPTIONS " + rtspUrl + " RTSP/1.0\r\n"
		"CSeq: " + Buffer( ++CSeq ) + "\r\n"
		"User-Agent: VMP RTP/RTSP Node\r\n"
		"\r\n"
	);

	Buffer answer;

	answer = tmpSocket->getBytes();

//	msgDbg( "\"OPTIONS\" answer:" );
//	msgDbg( answer );

	// Do a "DESCRIBE"...

	tmpSocket->putBytes(
		"DESCRIBE " + rtspUrl + " RTSP/1.0\r\n"
		"CSeq: " + Buffer( ++CSeq ) + "\r\n"
		"Accept: application/sdp\r\n"
		"User-Agent: VMP RTP/RTSP Node\r\n"
		"\r\n"
	);

	answer = tmpSocket->getBytes();

//	msgDbg( "\"DESCRIBE\" answer:" );
//	msgDbg( answer );

	BytesSessionPtr res = parseDescribeAnswer( answer );

	// Do a "SETUP" until we get a suitable ports configuration...

	rtpSocket.kill();	// You never know...
	rtcpSocket.kill();

	while ( ! rtpSocket || ! rtcpSocket ) {

		Uint16	tmpPort0;	// Candidate for RTP
		Uint16	tmpPort1;	// Candidate for RTCP

		{
			FastCSLocker locker( cltLock );
			if ( cltPort < 1024 ) {
				cltPort = 1024;
			}
			tmpPort0 = cltPort++;
			tmpPort1 = cltPort++;
		}

		tmpSocket->putBytes(
			"SETUP " + rtspUrl + " RTSP/1.0\r\n"
			"CSeq: " + Buffer( ++CSeq ) + "\r\n"
			"Transport: RTP/AVP;unicast;client_port="
				+ Buffer( tmpPort0 )
				+ "-"
				+ Buffer( tmpPort1 )
				+ "\r\n"
			"User-Agent: VMP RTP/RTSP Node\r\n"
			"\r\n"
		);

		answer = tmpSocket->getBytes();

//		msgDbg( "\"SETUP\" answer:" );
//		msgDbg( answer );

		Uint16	srvPort0;
		Uint16	srvPort1;
		Uint16	cltPort0;
		Uint16	cltPort1;

		parseSetupAnswer( answer, srvPort0, srvPort1, cltPort0, cltPort1 );

		Net::UDPSocketPtr tmpRtpSocket = new Net::UDPSocket;
		Net::UDPSocketPtr tmpRtcpSocket = new Net::UDPSocket;

		try {
//			msgDbg( "Opening RTP/RTCP on "
//				+ Buffer( cltPort0 )
//				+ "-"
//				+ Buffer( cltPort1 ) );
			tmpRtpSocket->openRead( cltPort0 );
			tmpRtcpSocket->openWrite( srvAddr, srvPort1, cltPort1 );
			// If we got so far, it means binding worked :-)
			rtpSocket = tmpRtpSocket;
			rtcpSocket = tmpRtcpSocket;
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't init RTP/RTCP connections!" );
		}

	}

	// Do a "PLAY"...

	tmpSocket->putBytes(
		"PLAY " + rtspUrl + " RTSP/1.0\r\n"
		"CSeq: " + Buffer( ++CSeq ) + "\r\n"
		"Session: " + tagSession + "\r\n"
		"Range: npt=0.000-\r\n"
		"User-Agent: VMP RTP/RTSP Node\r\n"
		"\r\n"
	);

	answer = tmpSocket->getBytes();

//	msgDbg( "\"PLAY\" answer:" );
//	msgDbg( answer );

	socket = tmpSocket;

	return res;

}

void RTP::RTSPHandler::close() {



}

// ============================================================================

void RTP::RTSPHandler::parseSetupAnswer(
	const	Buffer &	buf,
		Uint16 &	srvPort0,
		Uint16 &	srvPort1,
		Uint16 &	cltPort0,
		Uint16 &	cltPort1 ) {

	BufferArray lines = BufferParser::tokenize( buf, "\r\n" );

//	if ( lines.size() < 4 ) {
//		throw InvalidData( "Underflow in setup!" );
//	}

	if ( lines[ 0 ] != "RTSP/1.0 200 OK" ) {
		throw InvalidData( "Invalid SETUP answer!" );
	}

	// Analyze Session...
	// Search for session first
        BufferArray tokens;
	for ( Uint32 line = 0; line < lines.size(); line++ ) {
		if ( lines[ line ].beginsWith( "Session: " ) ) {
			Buffer value = lines[ line ]( 9 );
			tokens = BufferParser::tokenize( value, ";" );
			break;
		}
	}

	if ( tokens.isEmpty() ) {
		throw InvalidData( "No session in SETUP!" );
	}

	tagSession = tokens [ 0 ];

//No need to check on Timeout as this value is optional (and not used in current
//implementation
//	if ( tokens.size() != 2 ) {
//		throw InvalidData( "Invalid session in setup!" );
//	}
//
//	if ( ! tokens[ 1 ].beginsWith( "timeout=" ) ) {
//		throw InvalidData( "No timeout in session in setup!" );
//	}

	// Analyze Transport...
        // Search for transport line first
        tokens.kill();
        for ( Uint32 line = 0; line < lines.size(); line++ ) {
                if ( lines[ line ].beginsWith( "Transport: " ) ) {
                        Buffer value = lines[ line ]( 11 );
                        tokens = BufferParser::tokenize( value, ";" );
                        break;
                }
        }

	if ( tokens.isEmpty() ) {
		throw InvalidData( "No transport in SETUP!" );
	}

	while ( ! tokens.isEmpty() ) {
		Buffer t = tokens[ 0 ];
		if ( t == "RTP/AVP"
		  || t == "unicast"
		  || t == "mode=play") {
		}
		else if ( t.beginsWith( "destination=" ) ) { 
		}
		else if ( t.beginsWith( "source=" ) ) {
		}
		else if ( t.beginsWith( "client_port=" ) ) {
			// Ex: client_port=1234-1235
			BufferArray ports = BufferParser::tokenize( t( 12 ), "-" );
			cltPort0 = ports[ 0 ].toUint16();
			cltPort1 = ports[ 1 ].toUint16();
//			msgDbg( "ClientPorts: "
//				+ Buffer( cltPort0 )
//				+ " --- "
//				+ Buffer( cltPort1 ) );
		}
		else if ( t.beginsWith( "server_port=" ) ) {
			Buffer ports = t( 12 );
			BufferArray pa = BufferParser::tokenize(
				ports, "-" );
			if ( pa.size() != 2 ) {
				throw InvalidData( "Invalid server ports!" );
			}
			srvPort0 = pa[0].toUint16();
			srvPort1 = pa[1].toUint16();
//			msgDbg( "ServerPorts: "
//				+ Buffer( srvPort0 )
//				+ " --- "
//				+ Buffer( srvPort1 ) );
			if ( srvPort1 != srvPort0 + 1 ) {
				throw InvalidData( "Invalid server ports range!" );
			}
		}
		else {
			throw InvalidData( "Syntax error in transport!" );
		}
		tokens.pop( 0 );
	}

}

BytesSessionPtr RTP::RTSPHandler::parseDescribeAnswer(
	const	Buffer &	buf ) {

	// Analyze answer...

	HTTPAnswer	answer;

	answer.consume( buf );

	Bool		found = false;

	for ( Uint32 i = 0 ; i < answer.getOptions().getSize() ; i++ ) {
		if ( answer.getOptions()[ i ].toLower()
				== "content-type: application/sdp" ) {
			found = true;
			break;
		}
	}

	if ( ! found ) {
		msgWrn( "Application/sdp ?" );
	}

	// The payload is thus an SDP message. Analyze it...

	SDPPtr		sdp = new SDP;
	BytesSessionPtr	oBSession = sdp->decode( answer.getPayload() );

//	msgDbg( "Program: " + oBSession->toBuffer() );

	return oBSession;

}

// ============================================================================

const Uint16 RTP::RTSPHandler::RTSPDefaultPort = 554;

FastCS	RTP::RTSPHandler::cltLock;
Uint16	RTP::RTSPHandler::cltPort = 1234 + 2 * Math::rand( ( Uint32 )20000 );

// ============================================================================

