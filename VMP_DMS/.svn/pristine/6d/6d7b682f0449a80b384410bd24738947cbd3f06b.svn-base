// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.RTP.ReaderEngine.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include <math.h>

#include "BFC.MT.Locker.h"

#include "BFC.Net.IPv4Address.h"

#include "VMP.MuxedStream.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.ReaderEngine.h"
#include "VMP.RTP.ReaderUserInterface.vui.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

const DL::TypeCPtr & RTP::ReaderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "227f2e65-e9cd-4b8e-90c1-03fc566bb400" ),
		"VMP.RTP.ReaderEngine",
		L2::FinalPullEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Reader",
			"Devices",
			"Simple RTP reader",
			RTP::readerUserInterface
		)
	);

	return i;

}

// ============================================================================

RTP::ReaderEngine::ReaderEngine() :

	L2::FinalPullEngine( getClassType() ),
	cond( mutex ) {

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	addCallback( this, &ReaderEngine::buttonClickedCallback );

}

RTP::ReaderEngine::~ReaderEngine() {

	delCallbacks();

}

// ============================================================================

void RTP::ReaderEngine::initUICallback(
		UI::IElementPtr		ui ) {

	rtspLineEdit = ui->findChildByName( "rtspLineEdit" );
	addrLineEdit = ui->findChildByName( "addrLineEdit" );
	portLineEdit = ui->findChildByName( "portLineEdit" );
	widthLineEdit = ui->findChildByName( "widthLineEdit" );
	heightLineEdit = ui->findChildByName( "heightLineEdit" );
	frateLineEdit = ui->findChildByName( "frateLineEdit" );
	connectPushButton = ui->findChildByName( "connectPushButton" );

	ptTextLabel = ui->findChildByName( "ptTextLabel" );
	ssrcTextLabel = ui->findChildByName( "ssrcTextLabel" );
	csrcTextLabel = ui->findChildByName( "csrcTextLabel" );

	resetUI();

	connectPushButton->addObserver( this );

}

// ============================================================================

SessionPtr RTP::ReaderEngine::getSessionCallback() {

	MT::Locker	locker( mutex );

	while ( ! rtpHandler ) {
		cond.wait();
	}

	valid = true;

	rtpHandler->reset();

	return session;

}

void RTP::ReaderEngine::endSessionCallback() {

	resetUI();

}

FramePtr RTP::ReaderEngine::getFrameCallback(
	const	BFC::Uint64		/* index */ ) {

	MT::Locker	locker( mutex );

	if ( ! valid ) {
		throw L2::BrokenSession();
	}

	FramePtr res;

	try {
		res = rtpHandler->getFrame();
	}
	catch ( ThreadStopped & ) {
		throw;
	}
	catch ( Exception & e ) {
		resetUI();
		throw L2::BrokenSession( e );
	}

	return res;

}

// ============================================================================

void RTP::ReaderEngine::buttonClickedCallback(
		Msg::EventCPtr ) {

//	msgDbg( "buttonClicked!" );

	MT::Locker	locker( mutex );

	VideoStreamPtr iVStream;

	if ( rtpHandler ) {

		if ( rtspHandler ) {
			rtspHandler->close();
			rtspHandler.kill();
			rtcpHandler.kill();
		}

		rtpHandler.kill();
		session.kill();
		valid = false;

		rtspLineEdit->setEnabled( true );
		addrLineEdit->setEnabled( true );
		portLineEdit->setEnabled( true );

		connectPushButton->setText( "Connect..." );

		return;

	}

	RTPHandlerPtr	tmpRtpHandler;
	RTCPHandlerPtr	tmpRtcpHandler;
	RTSPHandlerPtr	tmpRtspHandler;

	if ( rtspLineEdit->getText().hasContent() ) {

		// RTSP server <--- rtsp url!

		tmpRtspHandler = new RTSPHandler;

		BytesSessionPtr	iBSession;

		Net::UDPSocketPtr	rtpSocket;
		Net::UDPSocketPtr	rtcpSocket;

		try {
			iBSession = tmpRtspHandler->open(
				rtspLineEdit->getText(),	// in
				rtpSocket,			// out
				rtcpSocket );			// out
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't open RTSP handler!" );
			return;
		}

		MuxedStreamPtr iPStream = iBSession->getContent();

// FIXME: handle program with other layout too!

		iVStream = iPStream->getComponent( 0 );

		tmpRtcpHandler = new RTCPHandler;

		tmpRtpHandler = new RTPHandler;

		tmpRtpHandler->useRTCP( tmpRtcpHandler );

		try {
			tmpRtpHandler->open( rtpSocket );
		}
		catch ( Exception& e ) {
			msgExc( e, "Can't open RTP handler!" );
			return;
		}

		try {
			tmpRtcpHandler->open(
				rtcpSocket );
		}
		catch ( Exception& e ) {
			msgExc( e, "Can't open RTCP handler!" );
			return;
		}

	}

	else if ( addrLineEdit->getText().hasContent() ) {

		// Raw multicast <--- address + port!

		Net::IPv4Address	addr;
		Uint16			port;

		addr = Net::IPv4Address( addrLineEdit->getText() );
		port = portLineEdit->getText().toUint16();

//		if ( ! addr.isOK() ) {
//			msgWrn( "Invalid IPv4 address!" );
//			return;
//		}

		if ( ! addr.isMulticast() ) {
			msgWrn( "Not an IPv4 multicast address!" );
			return;
		}

		tmpRtpHandler = new RTPHandler;

		try {
			tmpRtpHandler->open( addr, port );
		}
		catch ( Exception& e ) {
			msgExc( e, "Can't open RTP handler!" );
			return;
		}

	}

	else {

		// Raw unicast <--- port!

		Uint16			port;

		port = portLineEdit->getText().toUint16();

		tmpRtpHandler = new RTPHandler;

		try {
			tmpRtpHandler->open( port );
		}
		catch ( Exception& e ) {
			msgExc( e, "Can't open RTP handler!" );
			return;
		}

	}

	rtpHandler = tmpRtpHandler;
	rtcpHandler = tmpRtcpHandler;
	rtspHandler = tmpRtspHandler;

	if ( ! iVStream ) {
		iVStream = new VideoStream;
		iVStream->setCompression( VideoCompression::MPEG4 );
	}

	if ( ! iVStream->getFrameRate() ) {
		Double n = frateLineEdit->getText().toDouble();
		Double d = 1.0;
		while ( n != ::floor( n ) ) {
			n *= 10.0;
			d *= 10.0;
		}
		iVStream->setFrameRate( ( Uint32 )n );
		iVStream->setFrameScale( ( Uint32 )d );
	}

	if ( ! iVStream->getWidth() || ! iVStream->getHeight() ) {
		iVStream->setWidth( widthLineEdit->getText().toUint32() );
		iVStream->setHeight( heightLineEdit->getText().toUint32() );
	}

	session = new BytesSession( Session::Live );
	session->setContent( iVStream );

	valid = false;

	rtspLineEdit->setEnabled( false );
	addrLineEdit->setEnabled( false );
	portLineEdit->setEnabled( false );

	connectPushButton->setText( "Disconnect..." );

	cond.signal();

}

// ============================================================================

void RTP::ReaderEngine::resetUI() {

	ptTextLabel->setText( "" );
	ssrcTextLabel->setText( "" );
	csrcTextLabel->setText( "" );

}

// ============================================================================

DynVar RTP::ReaderEngine::getUserSettings() {

	VMP::DynVar res;

	res.add( "rtsp", rtspLineEdit->getText() );
	res.add( "addr", addrLineEdit->getText() );
	res.add( "port", portLineEdit->getText() );
	res.add( "width", widthLineEdit->getText() );
	res.add( "height", heightLineEdit->getText() );
	res.add( "frate", frateLineEdit->getText() );

	return res;

}

void RTP::ReaderEngine::setUserSettings(
	const	DynVar&		settings ) {

	rtspLineEdit->setText( settings.getBuffer( "rtsp" ) );
	addrLineEdit->setText( settings.getBuffer( "addr" ) );
	portLineEdit->setText( settings.getBuffer( "port" ) );
	widthLineEdit->setText( settings.getBuffer( "width" ) );
	heightLineEdit->setText( settings.getBuffer( "height" ) );
	frateLineEdit->setText( settings.getBuffer( "frate" ) );

}

// ============================================================================

static L2::EngineRegisterer< RTP::ReaderEngine > readerRegisterer;

// ============================================================================

