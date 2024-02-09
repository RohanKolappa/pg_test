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
//	BFC.Net.HttpURLConnection.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#include "BFC.Net.HttpURLConnection.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, HttpURLConnection )

// ============================================================================

Net::HttpURLConnection::HttpURLConnection(
		URLPtr		u ) :

	URLConnection	( u ),

	method		( "GET" ),
	chunkLength	( -1 ),
	fixedLength	( -1 ),
	responseCode	( -1 ),
	currRedirect	( dfltRedirect ) {

}

// ============================================================================

Buffer Net::HttpURLConnection::getHeaderFieldKey(
	const	Uint32		/*n*/ ) const {

	return Buffer();

}

// ============================================================================

void Net::HttpURLConnection::setFixedLengthStreamingMode(
	const	Uint32		pFixedLength ) {

	if ( isConnected() ) {
		throw InvalidArgument("Already connected");
	}

	if (chunkLength != -1) {
		throw InvalidArgument("Chunked encoding streaming mode set");
	}

	fixedLength = pFixedLength;

}

void Net::HttpURLConnection::setChunkedStreamingMode(
	const	Uint32		pChunkLength ) {

	if ( isConnected() ) {
		throw InvalidArgument("Can't set streaming mode: already connected");
	}

	if ( fixedLength != -1 ) {
		throw InvalidArgument("Fixed length streaming mode set");
	}

	chunkLength = ( pChunkLength ? pChunkLength : DEFAULT_CHUNK_SIZE );

}

Buffer Net::HttpURLConnection::getHeaderField(
	const	Uint32		/*n*/ ) const {

	return Buffer();

}

void Net::HttpURLConnection::setFollowRedirects(
	const	Bool		set ) {

	dfltRedirect = set;

}

Bool Net::HttpURLConnection::getFollowRedirects() {

	return dfltRedirect;

}

void Net::HttpURLConnection::setInstanceFollowRedirects(
	const	Bool		pCurrRedirect ) {

	currRedirect = pCurrRedirect;

}

Bool Net::HttpURLConnection::getInstanceFollowRedirects() const {

	return currRedirect;

}

void Net::HttpURLConnection::setRequestMethod(
	const	Buffer &	pMethod ) {

	if ( isConnected() ) {
		throw InvalidArgument( "Can't reset method: already connected" );
	}

	// This restriction will prevent people from using this class to
	// experiment w/ new HTTP methods using java.  But it should
	// be placed for security - the request Buffer could be
	// arbitrarily long.

	if ( ! methods.contains( pMethod ) ) {
		throw InvalidArgument( "Invalid HTTP method: " + pMethod );
	}

	method = pMethod;

}

Buffer Net::HttpURLConnection::getRequestMethod() const {

	return method;

}

Int32 Net::HttpURLConnection::getResponseCode() {

	/*
	 * We're got the response code already
	 */

	if ( responseCode != -1 ) {
		return responseCode;
	}

	getInputStream();

	Buffer statusLine = getHeaderField( 0 );

	if ( statusLine.isEmpty() ) {
		return -1;
	}

	/*
	 * Examine the status-line - should be formatted as per
	 * section 6.1 of RFC 2616 :-
	 *
	 * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase
	 *
	 * If status line can't be parsed return -1.
	 */

	if ( ! statusLine.contains( "HTTP/1." ) ) {
		return -1;
	}

	Uint32 codePos = statusLine.findFwd( ' ' );

	if ( codePos == 0 || codePos == statusLine.getLength() ) {
		return -1;
	}

	Uint32 phrasePos = statusLine.findFwd( ' ', codePos + 1 );

	if ( phrasePos < statusLine.getLength() ) {
		responseMessage = statusLine( phrasePos + 1 );
	}

	responseCode = statusLine( codePos + 1, phrasePos - codePos - 1 ).toUint32();

	return responseCode;

}

Buffer Net::HttpURLConnection::getResponseMessage() {

	getResponseCode();

	return responseMessage;

}

Time::Clock Net::HttpURLConnection::getHeaderFieldDate(
	const	Buffer &	/*name*/,
	const	Time::Clock	/*dflt*/ ) {

	throw NotImplemented();

//	Buffer dateString = getHeaderField(name);
//
//	try {
//		if (dateString.findFwd("GMT") == -1) {
//			dateString = dateString+" GMT";
//		}
//		return Date.parse(dateString);
//	} catch (Exception e) {
//	}
//
//	return Default;

}

IO::ByteInputStreamPtr Net::HttpURLConnection::getErrorStream() {

	return 0;

}

// ============================================================================

Bool Net::HttpURLConnection::dfltRedirect = true;

static const Char * tbl[] = {
	"GET", "POST", "HEAD", "OPTIONS", "PUT", "DELETE", "TRACE", 0 };

const BufferArray Net::HttpURLConnection::methods( tbl );

//const Uint32 Net::HttpURLConnection::DEFAULT_CHUNK_SIZE = 4096;

#endif // 0

// ============================================================================

