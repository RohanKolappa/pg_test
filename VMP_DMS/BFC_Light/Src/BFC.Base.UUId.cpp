// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.UUId.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Aliasing.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCS.h"
#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"
#include "BFC.Base.UUId.h"

#include "BFC.Math.Utils.h"

#include "BFC.Time.Clock.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::UUId
// ============================================================================

UUId UUId::makeV1() {

	UUId r;

	r.generateV1();

	return r;

}

// ============================================================================

UUId::UUId() {

	clear();

}

UUId::UUId(
	const	Buffer &	pBuffer ) {

	if ( pBuffer.getLength() == 36 ) {
		fromBuffer( pBuffer );
	}
	else if ( pBuffer.getLength() == 16 ) {
		fromCompactBuffer( pBuffer );
	}
	else {
		throw InvalidArgument();
	}

}

UUId::UUId(
	const	Uint32		pTimeLow,
	const	Uint16		pTimeMid,
	const	Uint16		pTimeHgh,
	const	Uint16		pClck,
	const	Buffer &	pNode ) {

	buildV1(
		  ( ( Uint64 )pTimeLow       )
		| ( ( Uint64 )pTimeMid << 32 )
		| ( ( Uint64 )pTimeHgh << 48 ),
		pClck,
		pNode );

}

UUId::UUId(
	const	Uint64		pTime,
	const	Uint16		pClck,
	const	Buffer &	pNode ) {

	buildV1( pTime, pClck, pNode );

}

UUId::UUId(
	const	Uint32		f1,
	const	Uint16		f2,
	const	Uint16		f3,
	const	Uchar		f40,
	const	Uchar		f41,
	const	Uchar		f42,
	const	Uchar		f43,
	const	Uchar		f44,
	const	Uchar		f45,
	const	Uchar		f46,
	const	Uchar		f47 ) {

	setValue( d    , System::Host2BE( f1 ) );
	setValue( d + 4, System::Host2BE( f2 ) );
	setValue( d + 6, System::Host2BE( f3 ) );

	*( d +  8 ) = f40;
	*( d +  9 ) = f41;
	*( d + 10 ) = f42;
	*( d + 11 ) = f43;
	*( d + 12 ) = f44;
	*( d + 13 ) = f45;
	*( d + 14 ) = f46;
	*( d + 15 ) = f47;

}

UUId::UUId(
	const	UUId &		o ) {

	System::memcpy( d, o.d, 16 );

}

// ============================================================================

UUId & UUId::operator = (
	const	UUId &		o ) {

	if ( this != &o ) {
		System::memcpy( d, o.d, 16 );
	}

	return *this;

}

// ============================================================================

void UUId::clear() {

	System::bzero( d, 16 );

}

// ============================================================================

void UUId::generateV1() {

	buildV1(
		getCurrentTime(),
		( Uchar )Math::rand( ( Uint32 )0xFFFF ),
		Buffer::rand( 6 ) );

}

void UUId::generateV3(
	const	UUId &		pNSId,
	const	Buffer &	pName ) {

	MD5	md5;
	Buffer	hash;

	md5.init();
	md5.process( pNSId.toCompactBuffer() );
	md5.process( pName );
	md5.done( hash );

	buildV3( hash );

}

// ============================================================================

Bool UUId::operator == (
	const	UUId &		o ) const {

	return ( System::memcmp( d, o.d, 16 ) == 0 );

}

Bool UUId::operator != (
	const	UUId &		o ) const {

	return ( System::memcmp( d, o.d, 16 ) != 0 );

}

// ============================================================================

Bool UUId::isNull() const {

	return ( System::memcmp( d, n, 16 ) == 0 );

}

// ============================================================================

void UUId::fromCompactBuffer(
	const	Buffer &	src ) {

	if ( src.getLength() != 16 ) {
		throw InvalidArgument( "UUId: not 16 bytes long!" );
	}

	src.safeDumpToBuffer( d, 16 );

}

Buffer UUId::toCompactBuffer() const {

	return Buffer( d, 16, true );

}

void UUId::fromBuffer(
	const	Buffer &	in ) {

	if ( in.length() != 36 ) {
		throw InvalidArgument( "UUId length != 36" );
	}

	fromBuffer( in.getBufferAddr() );

}

void UUId::fromBuffer(
	const	Uchar *		ip ) {

	// 0..7 | 9..12 | 14..17 | 19..22 | 24..35

	Uchar * op = d;

	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;

	skipDash( ip++ );

	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;

	skipDash( ip++ );

	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;

	skipDash( ip++ );

	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;

	skipDash( ip++ );

	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;
	hexStrToNum( ip, op ); ip += 2; op++;

}

void UUId::fromASFFile(
	const	Buffer &	in ) {

	if ( in.length() != 16 ) {
		throw InvalidArgument("UUId (ASF file format) length != 16");
	}

	const Uchar *	ip = in.getCstPtr();
	Uchar *		op = d;

	op[0] = ip[3];
	op[1] = ip[2];
	op[2] = ip[1];
	op[3] = ip[0];
	op += 4; ip += 4;

	op[0] = ip[1];
	op[1] = ip[0];
	op += 2; ip += 2;

	op[0] = ip[1];
	op[1] = ip[0];
	op += 2; ip += 2;

	*(Uint64 *)op = *(Uint64 *)ip;

}

Buffer UUId::toASFFile() const {

	Buffer res;

	res.resize( 16 );

	const Uchar *	ip = d;
	Uchar *		op = res.getVarPtr();

	op[0] = ip[3];
	op[1] = ip[2];
	op[2] = ip[1];
	op[3] = ip[0];
	op += 4; ip += 4;

	op[0] = ip[1];
	op[1] = ip[0];
	op += 2; ip += 2;

	op[0] = ip[1];
	op[1] = ip[0];
	op += 2; ip += 2;

	*(Uint64 *)op = *(Uint64 *)ip;

	return res;

}

Buffer UUId::toBuffer() const {

	Buffer		res;

	res.resize( 36 );

	const Uchar *	ip = d;
	Uchar *		op = res.getVarPtr();

	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;

	putDash( op++ );

	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;

	putDash( op++ );

	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;

	putDash( op++ );

	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;

	putDash( op++ );

	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;
	hexNumToStr( ip, op ); ip++; op += 2;

	return res;

}

// ============================================================================

void UUId::buildV1(
	const	Uint64		pTime,
	const	Uint16		pClck,
	const	Buffer &	pNode ) {

	if ( pNode.getLength() != 6 ) {
		throw InvalidArgument( "Node length != 6" );
	}

	setValue( d    , System::Host2BE( ( Uint32 )( pTime       ) ) );
	setValue( d + 4, System::Host2BE( ( Uint16 )( pTime >> 32 ) ) );
	setValue( d + 6, System::Host2BE( ( Uint16 )( pTime >> 48 ) ) );
	setValue( d + 8, System::Host2BE( ( Uint16 )( pClck       ) ) );

	d[ 6 ] &= 0x0F;		// Version 1
	d[ 6 ] |= 0x10;

	d[ 8 ] &= 0x3F;		// Variant 1
	d[ 8 ] |= 0x80;

	pNode.safeDumpToBuffer( d + 10, 6 );

}

void UUId::buildV3(
	const	Buffer &	hash ) {

	if ( hash.getLength() != 16 ) {
		throw InvalidArgument();
	}

	hash.safeDumpToBuffer( d, 16 );

	d[ 6 ] &= 0x0F;
	d[ 6 ] |= 0x30;

	d[ 8 ] &= 0x3F;
	d[ 8 ] |= 0x80;

}

// ============================================================================

Uchar UUId::hexVal(
	const	Uchar		c ) {

	if ( c >= '0' && c <= '9' ) return ( Uchar )( c - '0' );
	if ( c >= 'a' && c <= 'f' ) return ( Uchar )( c - 'a' + 10 );
	if ( c >= 'A' && c <= 'F' ) return ( Uchar )( c - 'A' + 10 );

	throw InvalidArgument( "UUId: invalid hex char!" );

}

void UUId::skipDash(
	const	Uchar		* i ) {

	if ( * i != '-' ) {
		throw InvalidArgument( "UUId: Missing dash!" );
	}

}

// ============================================================================

Uint64 UUId::getSystemTime() {

	// Offset between UUID formatted times and Unix formatted times:
	// - UUID UTC base time is October 15, 1582.
	// - Unix base time is January 1, 1970.

	return ( Uint64 )Time::now() * CU64( 10 ) + CU64( 0x01B21DD213814000 );

}

Uint64 UUId::getCurrentTime() {

	static Uint64	old = getSystemTime();
	static Uint16	cnt = 0;
	static FastCS	crt;

	FastCSLocker	lck( crt );
	Uint64		now;

	for (;;) {
		now = getSystemTime();
		if ( old != now ) {
			old = now;
			cnt = 0;
			break;
		}
		if ( cnt < 1024 ) {
			cnt++;
			break;
		}
	}

	return now + cnt;

}

// ============================================================================

const Uchar * UUId::hexTable = (const Uchar *)"0123456789ABCDEF";

const Uchar UUId::n[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// ============================================================================
// BFC::UUId::MD5
// ============================================================================

UUId::MD5::MD5() {

	init();

}

// ============================================================================

void UUId::MD5::init() {

	msgBuf.kill();
	msgLen = 0;

	state[ 0 ] = 0x67452301UL;
	state[ 1 ] = 0xefcdab89UL;
	state[ 2 ] = 0x98badcfeUL;
	state[ 3 ] = 0x10325476UL;

}

void UUId::MD5::process(
	const	Buffer &	pData ) {

	msgBuf += pData;
	msgLen += pData.getLength();

	while ( msgBuf.getLength() >= 64 ) {
		compress( msgBuf.getCstPtr() );
		msgBuf = msgBuf( 64 );
	}

}

void UUId::MD5::done(
		Buffer &	out ) {

	msgLen <<= 3;
	msgBuf += ( Uchar )0x80;

	if ( msgBuf.getLength() > 56 ) {
		msgBuf += Buffer::chars( 128 - msgBuf.getLength(), 0x00 );
	}
	else {
		msgBuf += Buffer::chars( 64 - msgBuf.getLength(), 0x00 );
	}

	STORE64L( msgLen, msgBuf.getVarPtr() + msgBuf.getLength() - 8 );

	while ( msgBuf.getLength() >= 64 ) {
		compress( msgBuf.getCstPtr() );
		msgBuf = msgBuf( 64 );
	}

	if ( out.getLength() != 16 ) {
		out.resize( 16 );
	}

	Uchar * ptr = out.getVarPtr();

	for ( Uint32 i = 0 ; i < 4 ; i++ ) {
		STORE32L( state[ i ], ptr + 4 * i );
	}

}

// ============================================================================

void UUId::MD5::compress(
	const	Uchar *		buf ) {

	Uint32 W[ 16 ];

	for ( Uint32 i = 0 ; i < 16 ; i++ ) {
		W[ i ] = LOAD32L( buf + 4 * i );
	}

	Uint32 a = state[0];
	Uint32 b = state[1];
	Uint32 c = state[2];
	Uint32 d = state[3];

	FF(a,b,c,d,W[0],7,0xd76aa478UL);
	FF(d,a,b,c,W[1],12,0xe8c7b756UL);
	FF(c,d,a,b,W[2],17,0x242070dbUL);
	FF(b,c,d,a,W[3],22,0xc1bdceeeUL);
	FF(a,b,c,d,W[4],7,0xf57c0fafUL);
	FF(d,a,b,c,W[5],12,0x4787c62aUL);
	FF(c,d,a,b,W[6],17,0xa8304613UL);
	FF(b,c,d,a,W[7],22,0xfd469501UL);
	FF(a,b,c,d,W[8],7,0x698098d8UL);
	FF(d,a,b,c,W[9],12,0x8b44f7afUL);
	FF(c,d,a,b,W[10],17,0xffff5bb1UL);
	FF(b,c,d,a,W[11],22,0x895cd7beUL);
	FF(a,b,c,d,W[12],7,0x6b901122UL);
	FF(d,a,b,c,W[13],12,0xfd987193UL);
	FF(c,d,a,b,W[14],17,0xa679438eUL);
	FF(b,c,d,a,W[15],22,0x49b40821UL);
	GG(a,b,c,d,W[1],5,0xf61e2562UL);
	GG(d,a,b,c,W[6],9,0xc040b340UL);
	GG(c,d,a,b,W[11],14,0x265e5a51UL);
	GG(b,c,d,a,W[0],20,0xe9b6c7aaUL);
	GG(a,b,c,d,W[5],5,0xd62f105dUL);
	GG(d,a,b,c,W[10],9,0x02441453UL);
	GG(c,d,a,b,W[15],14,0xd8a1e681UL);
	GG(b,c,d,a,W[4],20,0xe7d3fbc8UL);
	GG(a,b,c,d,W[9],5,0x21e1cde6UL);
	GG(d,a,b,c,W[14],9,0xc33707d6UL);
	GG(c,d,a,b,W[3],14,0xf4d50d87UL);
	GG(b,c,d,a,W[8],20,0x455a14edUL);
	GG(a,b,c,d,W[13],5,0xa9e3e905UL);
	GG(d,a,b,c,W[2],9,0xfcefa3f8UL);
	GG(c,d,a,b,W[7],14,0x676f02d9UL);
	GG(b,c,d,a,W[12],20,0x8d2a4c8aUL);
	HH(a,b,c,d,W[5],4,0xfffa3942UL);
	HH(d,a,b,c,W[8],11,0x8771f681UL);
	HH(c,d,a,b,W[11],16,0x6d9d6122UL);
	HH(b,c,d,a,W[14],23,0xfde5380cUL);
	HH(a,b,c,d,W[1],4,0xa4beea44UL);
	HH(d,a,b,c,W[4],11,0x4bdecfa9UL);
	HH(c,d,a,b,W[7],16,0xf6bb4b60UL);
	HH(b,c,d,a,W[10],23,0xbebfbc70UL);
	HH(a,b,c,d,W[13],4,0x289b7ec6UL);
	HH(d,a,b,c,W[0],11,0xeaa127faUL);
	HH(c,d,a,b,W[3],16,0xd4ef3085UL);
	HH(b,c,d,a,W[6],23,0x04881d05UL);
	HH(a,b,c,d,W[9],4,0xd9d4d039UL);
	HH(d,a,b,c,W[12],11,0xe6db99e5UL);
	HH(c,d,a,b,W[15],16,0x1fa27cf8UL);
	HH(b,c,d,a,W[2],23,0xc4ac5665UL);
	II(a,b,c,d,W[0],6,0xf4292244UL);
	II(d,a,b,c,W[7],10,0x432aff97UL);
	II(c,d,a,b,W[14],15,0xab9423a7UL);
	II(b,c,d,a,W[5],21,0xfc93a039UL);
	II(a,b,c,d,W[12],6,0x655b59c3UL);
	II(d,a,b,c,W[3],10,0x8f0ccc92UL);
	II(c,d,a,b,W[10],15,0xffeff47dUL);
	II(b,c,d,a,W[1],21,0x85845dd1UL);
	II(a,b,c,d,W[8],6,0x6fa87e4fUL);
	II(d,a,b,c,W[15],10,0xfe2ce6e0UL);
	II(c,d,a,b,W[6],15,0xa3014314UL);
	II(b,c,d,a,W[13],21,0x4e0811a1UL);
	II(a,b,c,d,W[4],6,0xf7537e82UL);
	II(d,a,b,c,W[11],10,0xbd3af235UL);
	II(c,d,a,b,W[2],15,0x2ad7d2bbUL);
	II(b,c,d,a,W[9],21,0xeb86d391UL);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

}

// ============================================================================

