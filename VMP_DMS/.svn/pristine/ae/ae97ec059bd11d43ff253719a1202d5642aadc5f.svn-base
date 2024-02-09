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
//	BFC.NTLM.Response.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteOutputStream.h"
#include "BFC.IO.MemoryOutputStream.h"

#include "BFC.Math.Utils.h"

#include "BFC.NTLM.Response.h"

#include "BFC.Crypto.DES.h"
#include "BFC.Crypto.MD4.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( NTLM, Response )

// ============================================================================

NTLM::Response::Response(
	const	Buffer &	host,
	const	Buffer &	user,
	const	Buffer &	userDomain,
	const	Buffer &	lmPassword,
	const	Buffer &	ntPassword,
	const	Buffer &	nonce ) {

	if ( host.isEmpty() ) {
		throw InvalidArgument("host : null value not allowed");
	}

	if ( user.isEmpty() ) {
		throw InvalidArgument("user : null value not allowed");
	}

	if ( userDomain.isEmpty() ) {
		throw InvalidArgument("userDomain : null value not allowed");
	}

	if ( lmPassword.isEmpty() ) {
		throw InvalidArgument("lmPassword : null value not allowed");
	}

	if ( ntPassword.isEmpty() ) {
		throw InvalidArgument("ntPassword : null value not allowed");
	}

	if ( nonce.isEmpty() ) {
		throw InvalidArgument("nonce : null value not allowed");
	}

	if ( lmPassword.getLength() != 16 ) {
		throw InvalidArgument("lmPassword : illegal size");
	}

	if ( ntPassword.getLength() != 16 ) {
		throw InvalidArgument("ntPassword : illegal size");
	}

	if ( nonce.getLength() != 8 ) {
		throw InvalidArgument("nonce : illegal size");
	}

	Buffer	lmResponse;
	Buffer	ntResponse;

	computeNTLMResponse(
		lmPassword, ntPassword, nonce,
		lmResponse, ntResponse );

	Uint32	lmRespLen = 0x18;
	Uint32	ntRespLen = 0x18;
	Uint32	domainLen = 2 * userDomain.getLength();
	Uint32	hostLen = 2 * host.getLength();
	Uint32	userLen = 2 * user.getLength();
	Uint32	domainOff = 0x40;
	Uint32	userOff = domainOff + domainLen;
	Uint32	hostOff = userOff + userLen;
	Uint32	lmRespOff = hostOff + hostLen;
	Uint32	ntRespOff = lmRespOff + lmRespLen;
	Uint32	msgLen = ntRespOff + ntRespLen;

	IO::MemoryOutputStreamPtr	mStream = new IO::MemoryOutputStream;
	IO::ByteOutputStreamPtr		bStream = new IO::ByteOutputStream( mStream );

	bStream->open();

	static const Uchar hdr[ 12 ] = {
		'N', 'T', 'L', 'M',
		'S', 'S', 'P', 0x00,
		0x03, 0x00, 0x00, 0x00
	};

	bStream->putBytes( Buffer( hdr, 12 ) );

	bStream->putLEUint32( lmRespLen | ( lmRespLen << 16 ) );
	bStream->putLEUint32( lmRespOff );
	bStream->putLEUint32( ntRespLen | ( ntRespLen << 16 ) );
	bStream->putLEUint32( ntRespOff );
	bStream->putLEUint32( domainLen | ( domainLen << 16 ) );
	bStream->putLEUint32( domainOff );
	bStream->putLEUint32( userLen | ( userLen << 16 ) );
	bStream->putLEUint32( userOff );
	bStream->putLEUint32( hostLen | ( hostLen << 16 ) );
	bStream->putLEUint32( hostOff );
	bStream->putLEUint32( 0x00000000 );
	bStream->putLEUint32( msgLen );
	bStream->putLEUint32( 0x0000 );	 // bStream->putLEUint32( 0x8201 );

	Buffer tmp;

	tmp = userDomain.toUpper();

	for ( Uint32 i = 0 ; i < tmp.getLength() ; i++ ) {
		bStream->putLEUint16( tmp[ i ] );
	}

	for ( Uint32 i = 0 ; i < user.getLength() ; i++ ) {
		bStream->putLEUint16( user[ i ] );
	}

	tmp = host.toUpper();

	for ( Uint32 i = 0 ; i < tmp.getLength() ; i++ ) {
		bStream->putLEUint16( tmp[ i ] );
	}

	bStream->putBytes( lmResponse );
	bStream->putBytes( ntResponse );

	bStream->flush();

	data = mStream->getContent();

}

// ============================================================================

Buffer NTLM::Response::computeLMPassword(
	const	Buffer &	password ) {

	Buffer	lm_pw = password.toUpper();

	if ( lm_pw.getLength() > 14 ) {
		lm_pw = lm_pw( 0, 14 );
	}
	else if ( lm_pw.getLength() < 14 ) {
		while ( lm_pw.getLength() < 14 ) {
			lm_pw += ( Uchar )0x00;
		}
	}

	Crypto::CipherPtr des = new Crypto::DES;

	Buffer	lm_hpw;

	des->setKey( computeDESKey( lm_pw( 0, 7 ) ) );
	lm_hpw = des->encrypt( Buffer( MAGIC, 8 ) );

	des->setKey( computeDESKey( lm_pw( 7, 7 ) ) );
	lm_hpw += des->encrypt( Buffer( MAGIC, 8 ) );

	return lm_hpw;

}

Buffer NTLM::Response::computeNTPassword(
	const	Buffer &	password ) {

	Uint32	len = Math::getMin( password.getLength(), 14U );
	Buffer	nt_pw;

	nt_pw.resize( 2 * len );

	for ( Uint32 i = 0 ; i < len ; i++ ) {
		nt_pw[ 2 * i     ] = password[ i ];
		nt_pw[ 2 * i + 1 ] = 0x00;
	}

	Crypto::HashPtr md4 = new Crypto::MD4;

	md4->init();
	md4->process( nt_pw );

	Buffer res;

	md4->done( res );

	return res;

}

// ============================================================================

void NTLM::Response::computeNTLMResponse(
	const	Buffer &	lmPassword,
	const	Buffer &	ntPassword,
	const	Buffer &	nonce,
		Buffer &	lmResponse,
		Buffer &	ntResponse ) {

	if ( lmPassword.getLength() != 16 ) {
		throw InvalidArgument("lmPassword : illegal size");
	}

	if ( ntPassword.getLength() != 16 ) {
		throw InvalidArgument("ntPassword : illegal size");
	}

	if ( nonce.getLength() != 8 ) {
		throw InvalidArgument("nonce : illegal size");
	}

	static const Buffer pad = Buffer::chars( 5, 0x00 );

	lmResponse = encrypt( lmPassword + pad, nonce );
	ntResponse = encrypt( ntPassword + pad, nonce );

}

Buffer NTLM::Response::computeDESKey(
	const	Buffer &	pKey56 ) {

	if ( pKey56.getLength() != 7 ) {
		throw InternalError();
	}

	const Uchar *	k56 = pKey56.getCstPtr();

	Buffer	res;

	res.resize( 8 );

	Uchar *	k64 = res.getVarPtr();

	k64[ 0 ] = (                     ( k56[ 0 ]      ) );
	k64[ 1 ] = ( ( k56[ 0 ] << 7 ) | ( k56[ 1 ] >> 1 ) );
	k64[ 2 ] = ( ( k56[ 1 ] << 6 ) | ( k56[ 2 ] >> 2 ) );
	k64[ 3 ] = ( ( k56[ 2 ] << 5 ) | ( k56[ 3 ] >> 3 ) );
	k64[ 4 ] = ( ( k56[ 3 ] << 4 ) | ( k56[ 4 ] >> 4 ) );
	k64[ 5 ] = ( ( k56[ 4 ] << 3 ) | ( k56[ 5 ] >> 5 ) );
	k64[ 6 ] = ( ( k56[ 5 ] << 2 ) | ( k56[ 6 ] >> 6 ) );
	k64[ 7 ] = ( ( k56[ 6 ] << 1 )                     );

	return res;

}

Buffer NTLM::Response::encrypt(
	const	Buffer &	keys,
	const	Buffer &	plaintext ) {

	Crypto::CipherPtr des = new Crypto::DES;

	Buffer	res;

	des->setKey( computeDESKey( keys( 0, 7 ) ) );
	res += des->encrypt( plaintext );

	des->setKey( computeDESKey( keys( 7, 7 ) ) );
	res += des->encrypt( plaintext );

	des->setKey( computeDESKey( keys( 14, 7 ) ) );
	res += des->encrypt( plaintext );

	return res;

}

// ============================================================================

const Uchar NTLM::Response::MAGIC[ 8 ] = {
	0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25
};

// ============================================================================

