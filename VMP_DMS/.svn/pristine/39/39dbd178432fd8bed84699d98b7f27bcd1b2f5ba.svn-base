// ============================================================================
// 
// Copyright (c) 2003-2009 Barco N.V.
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
//	BFC.HTTP.Client.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdlib.h>

#include "BFC.Base.Utils.h"

#include "BFC.Base64.Decoder.h"
#include "BFC.Base64.Encoder.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.HTTP.Client.h"

#include "BFC.Net.TCPSocket.h"

#include "BFC.NTLM.Challenge.h"
#include "BFC.NTLM.Request.h"
#include "BFC.NTLM.Response.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( HTTP, Client )

// ============================================================================

HTTP::Client::Client() {

	setObjectName( "BFC.HTTP.Client" );

}

// ============================================================================

HTTP::AnswerPtr HTTP::Client::get(
	const	Net::URL &		url ) {

	Buffer proxy = ::getenv( "http_proxy" );

	return ( proxy.hasContent()
		? getProxy( proxy, url )
		: getDirect( url.getHost(), url.getPort(), url.getFile() ) );

}

// ============================================================================

HTTP::AnswerPtr HTTP::Client::getDirect(
	const	Net::IPv4Address &	addr,
	const	Uint16			port,
	const	Buffer &		url ) {

	Net::TCPSocketPtr	socket = new Net::TCPSocket( addr, port );

//	msgDbg( "get(): connecting..." );

	socket->open();

//	msgDbg( "get(): requesting..." );

	socket->putBytes(
		"GET /" + url + " HTTP/1.0\r\n"
		"Host: " + addr.getFQN() + "\r\n"
		"Connection: Close\r\n"
		"\r\n" );

	AnswerPtr		result = new Answer;

//	msgDbg( "get(): fetching answer..." );

	for (;;) {
		Buffer pld;
		try {
			pld = socket->getBytes();
		}
		catch ( Exception & ) {
			break;
		}
		if ( result->parse( pld ) ) {
			break;
		}
	}

	socket->close();

//	msgDbg( "get(): done!" );

	return result;

}

HTTP::AnswerPtr HTTP::Client::getProxy(
	const	Net::URL &		proxy,
	const	Net::URL &		url ) {

//	msgDbg( "getProxy(): parsing..." );

	Net::IPv4Address	proxyAddr = proxy.getHost();
	Uint16			proxyPort = proxy.getPort();
	Buffer			proxyUser = proxy.getUsername();
	Buffer			proxyPass = proxy.getPassword();

	Net::TCPSocketPtr	socket = new Net::TCPSocket( proxyAddr, proxyPort );

//	msgDbg( "get(): connecting..." );

	socket->open();
	socket->setKeepAlive( true );

	IO::ByteInputStreamPtr	is = new IO::ByteInputStream( socket );
	IO::ByteOutputStreamPtr	os = new IO::ByteOutputStream( socket );

	is->open();
	os->open();

	Buffer	ntlmHostClt = "KNDCLT12345";
	Buffer	ntlmHostDom = "BARCO";
	Buffer	ntlmUserDom = "BARCO";

//	msgDbg( "getProxy(): starting negotiation..." );

	Base64::EncoderPtr	b64Encoder = new Base64::Encoder;

	NTLM::RequestPtr ntlmRequest = new NTLM::Request(
		ntlmHostClt,
		ntlmHostDom );

	Buffer	fstMsg	= b64Encoder->encode( ntlmRequest->toBuffer() );

	Buffer	toSend =
		"GET " + url.toBuffer() + " HTTP/1.1\n"
		"Host: " + url.getHost() + "\n"
		"Proxy-Connection: Keep-Alive\n"
		"Proxy-Authorization: NTLM " + fstMsg + "\n"
		"\n";

//	msgDbg( "getProxy(): sending request:\n" + toSend );

	os->putBytes( toSend );
	os->flush();

//	msgDbg( "getProxy(): request sent!" );

	Buffer	resp;
	Uint32	contentLength = 0;

	for (;;) {
		Buffer line = is->getLine();
//		msgDbg( ">>> " + line );
		if ( line.isEmpty() ) {
			break;
		}
		if ( line.beginsWith( "Content-Length" ) ) {
			contentLength = line( line.findFwd( ':' ) + 1 ).trim().toUint32();
//			msgDbg( "=== content-length: " + Buffer( contentLength ) );
		}
		else if ( line.beginsWith( "Proxy-Authenticate" ) ) {
			resp = line( line.findFwd( ':' ) + 1 ).trim();
//			msgDbg( "=== proxy-authenticate: " + resp );
		}
	}

//	msgDbg( "Skipping content-length bytes..." );

	is->skip( contentLength );

	Buffer algo = resp( 0, resp.findFwd( ' ' ) );

	if ( algo != "NTLM" ) {
		throw InternalError( "Unsupported mode!" );
	}

	resp = resp( resp.findFwd( ' ' ) + 1 ).trim();

//	msgDbg( "getProxy(): NTLM Chellange Packet: " + resp );

	Base64::DecoderPtr b64Decoder = new Base64::Decoder;

	resp = b64Decoder->decode( resp );

//	msgDbg( "--> " + Utils::toHex( resp ) );

	NTLM::ChallengePtr ntlmChallenge = new NTLM::Challenge(
		resp );

	NTLM::ResponsePtr ntlmResp = new NTLM::Response(
		ntlmHostClt,
		proxyUser,
		ntlmUserDom,
		NTLM::Response::computeLMPassword( proxyPass ),
		NTLM::Response::computeNTPassword( proxyPass ),
		ntlmChallenge->getNonce() );

	fstMsg = b64Encoder->encode( ntlmResp->toBuffer() );

	toSend =
		"GET " + url.toBuffer() + " HTTP/1.1\n"
		"Host: " + url.getHost() + "\n"
		"Proxy-Connection: Keep-Alive\n"
		"Proxy-Authorization: NTLM " + fstMsg + "\n"
		"\n";

//	msgDbg( "getProxy(): sending 2nd request:\n" + toSend );

	os->putBytes( toSend );
	os->flush();

//	msgDbg( "getProxy(): Response sent!" );

//	resp.kill();
//	contentLength = 0;
//
//	for (;;) {
//		Buffer line = is->getLine();
//		msgDbg( ">>> " + line );
//		if ( line.isEmpty() ) {
//			break;
//		}
//		if ( line.beginsWith( "Content-Length" ) ) {
//			contentLength = line( line.findFwd( ':' ) + 1 ).trim().toUint32();
//			msgDbg( "=== content-length: " + Buffer( contentLength ) );
//		}
//		else if ( line.beginsWith( "Proxy-Authenticate" ) ) {
//			resp = line( line.findFwd( ':' ) + 1 ).trim();
//			msgDbg( "=== proxy-authenticate: " + resp );
//		}
//	}

//	msgDbg( "getProxy(): parsing response..." );

	AnswerPtr result = new Answer;

	for (;;) {
		Buffer b;
		try {
			b = is->getBytes();
		}
		catch ( IO::EndOfStream & ) {
			msgWrn( "getProxy(): end-of-stream ???" );
			break;
		}
//		msgDbg( ">>> " + b );
		if ( result->parse( b ) ) {
			break;
		}
	}

//	msgDbg( "getProxy(): payload: " + Buffer( result->getPayload().getLength() ) );

//	const BufferArray & hdr = result->getHeader();
//	for ( Uint32 i = 0 ; i < hdr.getSize() ; i++ ) {
//		msgDbg( "header: " + hdr[ i ] );
//	}
//	if ( result->getHeader( "Content-Type" ).contains( "text/" ) ) {
//		msgDbg( "payload: " + result->getPayload() );
//	}

	socket->close();

//	msgDbg( "getProxy(): done!" );

	return result;

}

// ============================================================================

