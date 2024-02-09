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
//	VMP.RTP.WriterBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Msg.Funcs.h"

#include "BFC.MT.Locker.h"

#include "VMP.L3.BackendRegisterer.h"

#include "VMP.RTP.WriterBackend.h"
#include "VMP.RTP.WriterUserInterface.vui.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

const DL::TypeCPtr & RTP::WriterBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "3e388606-3d18-4018-8353-61c861c24004" ),
		"VMP.RTP.WriterBackend",
		L3::ConsumerBackend::getClassType(),
		new UI::UObjectDescr(
			"RTP Writer",
			"Devices",
			"Simple RTP writer",
			RTP::writerUserInterface
		)
	);

	return i;

}

// ============================================================================

RTP::WriterBackend::WriterBackend() :

	L3::ConsumerBackend
		( getClassType(), BytesSession::getClassType() ),
	cond	( mutex ) {

	socket		= new Net::UDPSocket;

	dynRtspBuf	= new Dyn::VBuffer( "rtsp" );
	dynAddrBuf	= new Dyn::VBuffer( "addr" );
	dynPortBuf	= new Dyn::VBuffer( "port" );

	dynButtSgn	= new Dyn::VSignal( "connect" );

	dynPtBuffr	= new Dyn::VBuffer( "pt" );
	dynSsrcBuf	= new Dyn::VBuffer( "ssrc" );
	dynCsrcBuf	= new Dyn::VBuffer( "csrc" );

	addDynVar( dynRtspBuf );
	addDynVar( dynAddrBuf );
	addDynVar( dynPortBuf );

	addCallback( this, &WriterBackend::buttonClickedCallback );

	dynButtSgn->addObserver( this );

}

RTP::WriterBackend::~WriterBackend() {

	delAllDynVars();

	delCallbacks();

}

// ============================================================================

//SessionPtr RTP::WriterBackend::getSessionCallback() {
//
//	Locker	locker( mutex );
//
//	while ( ! rtpHandler ) {
//		cond.wait();
//	}
//
//	valid = true;
//
//	rtpHandler->reset();
//
//	return session;
//
//}
//
//void RTP::WriterBackend::endSessionCallback() {
//
//	resetUI();
//
//}
//
//FramePtr RTP::WriterBackend::getFrameCallback(
//	const	BFC::Uint64		/* index */ ) {
//
//	Locker	locker( mutex );
//
//	if ( ! valid ) {
//		throw BrokenSession();
//	}
//
//	FramePtr res;
//
//	try {
//		res = rtpHandler->getBytes();
//	}
//	catch ( ThreadStopped & ) {
//		throw;
//	}
//	catch ( Exception & e ) {
//		resetUI();
//		throw BrokenSession( e );
//	}
//
//	return res;
//
//}

// ============================================================================

void RTP::WriterBackend::initSession(
		SessionPtr		/* session */ ) {

//	socket->openWrite(addr,port);

}

void RTP::WriterBackend::exitSession() {

//	socket->close();

}

void RTP::WriterBackend::putFrame(
		FramePtr		frame ) {

	BytesFramePtr bf = frame;

//	Buffer data_to_send=bf->getData();
//	int packetsize=1400;
//
//	for(unsigned int nbytes=0;;nbytes+=packetsize) {
//		int length=packetsize;
//		bool stop=false;
//		if(data_to_send.getLength()<nbytes+packetsize) {
//			length=data_to_send.getLength()-nbytes;
//			stop=true;
//		}
//		Buffer packet = data_to_send(nbytes,length);
//
//
//		RTPHeader head;
//
//		head.m_version = 0x80;
//		head.m_marker_ploadtype=stop ? 0xE0 : 0x60 ;
//		head.m_seqnumb = System::swap( index++ );
//		head.m_timestamp = (Uint32)(Time::now());
//		head.m_SSRC = 0x01234567;
//
//		const unsigned char* pointer = (const unsigned char*)&head;
//		Buffer header(pointer,sizeof(RTPHeader),true);
//
//		socket->putBytes (header+packet );
//		if(stop) break;
//	}

}

// ============================================================================

//class RTPHeader {
//
//public :
//
//	Uint8 version() const		{ return m_version; }
//	bool marker() const			{ return (m_marker_ploadtype&0x80)!=0; }
//	Uint8 payloadtype() const	{ return  m_marker_ploadtype&0x7F; }
//	Uint16 seqnumb() const		{ return System::swap(m_seqnumb); }
//	Uint32 timestamp() const	{ return System::swap(m_timestamp); }
//	Uint32 SSRC() const			{ return System::swap(m_SSRC); }
//
//	Buffer toBuffer() const {
//		Buffer res;
//		res += Buffer("RTP header\n");
//		res += toRawBuffer();
//		res += Buffer("\nVersion: ")+Buffer((Uint32)version(),Buffer::Base16);
//		res += Buffer("\nMarker: ")+Buffer((Bool)marker());
//		res += Buffer("\nPayload Type: ")+Buffer((Uint32)payloadtype(),Buffer::Base16);
//		res += Buffer("\nSequence Number: ")+Buffer((Uint32)seqnumb());
//		res += Buffer("\nTimeStamp: ")+Buffer((Uint32)timestamp());
//		res += Buffer("\nSSRC: ")+Buffer((Uint32)SSRC(),Buffer::Base16);
//		res += Buffer("\n");
//		return res;
//	}
//
//	Buffer toRawBuffer() const {
//		Buffer res;
//		for( Uint32 i = 0 ; i < ( Uint32 )sizeof( RTPHeader ) ; i++ ) {
//			Char val=((const Char*)this)[i];
//			Char nibble1 = ((val & 0xF0) >>4);
//			Char nibble1char = (nibble1>9)?('A'+nibble1-10):('0'+nibble1);
//			Char nibble2 = (val & 0x0F);
//			Char nibble2char = (nibble2>9)?('A'+nibble2-10):('0'+nibble2);
//			res += nibble1char;
//			res += nibble2char;
//			res += " ";
//		}
//		return res;
//	}
//
//	Uint8 m_version; //=0x80
//	Uint8 m_marker_ploadtype;
//	Uint16 m_seqnumb;
//	Uint32 m_timestamp;
//	Uint32 m_SSRC;
//};

// ============================================================================

void RTP::WriterBackend::initUICallback(
		UI::IElementPtr		ui ) {

	Msg::sync( dynRtspBuf, ui->findChildByName( "rtspLineEdit" ) );
	Msg::sync( dynAddrBuf, ui->findChildByName( "addrLineEdit" ) );
	Msg::sync( dynPortBuf, ui->findChildByName( "portLineEdit" ) );

	Msg::sync( dynButtSgn, ui->findChildByName( "connectPushButton" ) );

	Msg::sync( dynPtBuffr, ui->findChildByName( "ptTextLabel" ) );
	Msg::sync( dynSsrcBuf, ui->findChildByName( "ssrcTextLabel" ) );
	Msg::sync( dynCsrcBuf, ui->findChildByName( "csrcTextLabel" ) );

	resetUI();

}

// ============================================================================

void RTP::WriterBackend::buttonClickedCallback(
		Msg::EventCPtr ) {

////	msgDbg( "buttonClicked!" );
//
//	Locker	locker( mutex );
//
//	VideoStreamPtr iVStream;
//
//	if ( rtpHandler ) {
//
//		if ( rtspHandler ) {
//			rtspHandler->close();
//			rtspHandler.kill();
//			rtcpHandler.kill();
//		}
//
//		rtpHandler.kill();
//		session.kill();
//		valid = false;
//
//		rtspLineEdit->setEnabled( true );
//		addrLineEdit->setEnabled( true );
//		portLineEdit->setEnabled( true );
//
//		connectPushButton->setText( "Connect..." );
//
//		return;
//
//	}
//
//	RTPHandlerPtr	tmpRtpHandler;
//	RTCPHandlerPtr	tmpRtcpHandler;
//	RTSPHandlerPtr	tmpRtspHandler;
//
//	if ( rtspLineEdit->getText().hasContent() ) {
//
//		// RTSP server <--- rtsp url!
//
//		tmpRtspHandler = new RTSPHandler;
//
//		BytesSessionPtr	iBSession;
//
//		Net::UDPSocketPtr	rtpSocket;
//		Net::UDPSocketPtr	rtcpSocket;
//
//		try {
//			iBSession = tmpRtspHandler->open(
//				rtspLineEdit->getText(),	// in
//				rtpSocket,			// out
//				rtcpSocket );			// out
//		}
//		catch ( Exception & e ) {
//			msgExc( e, "Can't open RTSP handler!" );
//			return;
//		}
//
//		MuxedStreamPtr iPStream = iBSession->getContent();
//
//// FIXME: handle program with other layout too!
//
//		iVStream = iPStream->getComponent( 0 );
//
//		tmpRtcpHandler = new RTCPHandler;
//
//		tmpRtpHandler = new RTPHandler(
//			ptTextLabel,
//			ssrcTextLabel,
//			csrcTextLabel );
//
//		tmpRtpHandler->useRTCP( tmpRtcpHandler );
//
//		try {
//			tmpRtpHandler->open( rtpSocket );
//		}
//		catch ( Exception& e ) {
//			msgExc( e, "Can't open RTP handler!" );
//			return;
//		}
//
//		try {
//			tmpRtcpHandler->open(
//				rtcpSocket );
//		}
//		catch ( Exception& e ) {
//			msgExc( e, "Can't open RTCP handler!" );
//			return;
//		}
//
//	}
//
//	else if ( addrLineEdit->getText().hasContent() ) {
//
//		// Raw multicast <--- address + port!
//
//		Net::IPv4Address	addr;
//		Uint16			port;
//
//		addr = Net::IPv4Address( addrLineEdit->getText() );
//		port = portLineEdit->getText().toUint16();
//
////		if ( ! addr.isOK() ) {
////			msgWrn( "Invalid IPv4 address!" );
////			return;
////		}
//
//		if ( ! addr.isMulticast() ) {
//			msgWrn( "Not an IPv4 multicast address!" );
//			return;
//		}
//
//		tmpRtpHandler = new RTPHandler(
//			ptTextLabel,
//			ssrcTextLabel,
//			csrcTextLabel );
//
//		try {
//			tmpRtpHandler->open( addr, port );
//		}
//		catch ( Exception& e ) {
//			msgExc( e, "Can't open RTP handler!" );
//			return;
//		}
//
//	}
//
//	else {
//
//		// Raw unicast <--- port!
//
//		Uint16			port;
//
//		port = portLineEdit->getText().toUint16();
//
//		tmpRtpHandler = new RTPHandler(
//			ptTextLabel,
//			ssrcTextLabel,
//			csrcTextLabel );
//
//		try {
//			tmpRtpHandler->open( port );
//		}
//		catch ( Exception& e ) {
//			msgExc( e, "Can't open RTP handler!" );
//			return;
//		}
//
//	}
//
//	rtpHandler = tmpRtpHandler;
//	rtcpHandler = tmpRtcpHandler;
//	rtspHandler = tmpRtspHandler;
//
//	if ( ! iVStream ) {
//		iVStream = new VideoStream;
//		iVStream->setCompression( VideoCompression::MPEG4 );
//	}
//
//	if ( ! iVStream->getFrameRate() ) {
//		Double n = frateLineEdit->getText().toDouble();
//		Double d = 1.0;
//		while ( n != ::floor( n ) ) {
//			n *= 10.0;
//			d *= 10.0;
//		}
//		iVStream->setFrameRate( ( Uint32 )n );
//		iVStream->setFrameScale( ( Uint32 )d );
//	}
//
//	if ( ! iVStream->getWidth() || ! iVStream->getHeight() ) {
//		iVStream->setWidth( widthLineEdit->getText().toUint32() );
//		iVStream->setHeight( heightLineEdit->getText().toUint32() );
//	}
//
//	session = new BytesSession( Session::Live );
//	session->setContent( iVStream );
//
//	valid = false;
//
//	rtspLineEdit->setEnabled( false );
//	addrLineEdit->setEnabled( false );
//	portLineEdit->setEnabled( false );
//
//	connectPushButton->setText( "Disconnect..." );
//
//	cond.signal();

}

// ============================================================================

void RTP::WriterBackend::resetUI() {

//	ptTextLabel->setText( "" );
//	ssrcTextLabel->setText( "" );
//	csrcTextLabel->setText( "" );

}

// ============================================================================

static L3::BackendRegisterer< RTP::WriterBackend > readerRegisterer;

// ============================================================================

