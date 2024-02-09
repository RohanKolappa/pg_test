// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP".
// 
// "VMP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.EncoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Stijn LAST
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "VMP.RTP.EncoderEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

class RTPHeader {

public :

	Uint8 version() const		{ return m_version; }
	bool marker() const			{ return (m_marker_ploadtype&0x80)!=0; }
	Uint8 payloadtype() const	{ return  m_marker_ploadtype&0x7F; }
	Uint16 seqnumb() const		{ return System::swap(m_seqnumb); }
	Uint32 timestamp() const	{ return System::swap(m_timestamp); }
	Uint32 SSRC() const			{ return System::swap(m_SSRC); }

	Buffer toBuffer() const {
		Buffer res;
		res += Buffer("RTP header\n");
		res += toRawBuffer();
		res += Buffer("\nVersion: ")+Buffer((Uint32)version(),Buffer::Base16);
		res += Buffer("\nMarker: ")+Buffer((Bool)marker());
		res += Buffer("\nPayload Type: ")+Buffer((Uint32)payloadtype(),Buffer::Base16);
		res += Buffer("\nSequence Number: ")+Buffer((Uint32)seqnumb());
		res += Buffer("\nTimeStamp: ")+Buffer((Uint32)timestamp());
		res += Buffer("\nSSRC: ")+Buffer((Uint32)SSRC(),Buffer::Base16);
		res += Buffer("\n");
		return res;
	}

	Buffer toRawBuffer() const {
		Buffer res;
		for( Uint32 i = 0 ; i < ( Uint32 )sizeof( RTPHeader ) ; i++ ) {
			Char val=((const Char*)this)[i];
			Char nibble1 = ((val & 0xF0) >>4);
			Char nibble1char = (nibble1>9)?('A'+nibble1-10):('0'+nibble1);
			Char nibble2 = (val & 0x0F);
			Char nibble2char = (nibble2>9)?('A'+nibble2-10):('0'+nibble2);
			res += nibble1char;
			res += nibble2char;
			res += " ";
		}
		return res;
	}

	Uint8 m_version; //=0x80
	Uint8 m_marker_ploadtype;
	Uint16 m_seqnumb;
	Uint32 m_timestamp;
	Uint32 m_SSRC;
};

// ============================================================================

RTP::EncoderEngine::EncoderEngine(
	const	Net::IPv4Address &	a,
	const	Uint16			p ) :

	L2::FinalPushEngine( getClassType() ),

	addr( a ),
	port( p ) {

	socket = new Net::UDPSocket;

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setObjectName( "VMP.L2.BytesSessionConsumer" );

}

RTP::EncoderEngine::~EncoderEngine() {

}

// ============================================================================

void RTP::EncoderEngine::putSessionCallback(
		SessionPtr		/* session */ ) {

	socket->openWrite(addr,port);

}

void RTP::EncoderEngine::endSessionCallback() {

	socket->close();

}

void RTP::EncoderEngine::putFrameCallback(
		FramePtr		frame ) {

	BytesFramePtr bf = frame;

	Buffer data_to_send=bf->getData();
	int packetsize=1400;

	for(unsigned int nbytes=0;;nbytes+=packetsize) {
		int length=packetsize;
		bool stop=false;
		if(data_to_send.getLength()<nbytes+packetsize) {
			length=data_to_send.getLength()-nbytes;
			stop=true;
		}
		Buffer packet = data_to_send(nbytes,length);


		RTPHeader head;

		head.m_version = 0x80;
		head.m_marker_ploadtype=stop ? 0xE0 : 0x60 ;
		head.m_seqnumb = System::swap( index++ );
		head.m_timestamp = (Uint32)(Time::now());
		head.m_SSRC = 0x01234567;

		const unsigned char* pointer = (const unsigned char*)&head;
		Buffer header(pointer,sizeof(RTPHeader),true);

		socket->putBytes (header+packet );
		if(stop) break;
	}

}

// ============================================================================

