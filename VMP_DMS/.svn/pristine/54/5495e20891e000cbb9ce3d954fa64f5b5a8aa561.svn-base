// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Base.UUId.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.System.h"
#include "TBFC.Base.UUId.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

UUId::UUId() {

	is[0] = is[1] = is[2] = is[3] = 0;

}

UUId::UUId(
	const	Char *		s ) {

	if ( System::strlen( ( const Uchar * )s ) != 36 ) {
		return;
	}

	fromBuffer( ( const Uchar * )s );

}

UUId::UUId(
	const	Uint32		clock_low,
	const	Uint16		time_mid,
	const	Uint16		time_hi_and_version,
	const	Uint16		clock_seq,
	const	Buffer&		node ) {

	const Uchar *ptr = node.getBufferAddr();

	is[ 0 ] = clock_low;
	is[ 1 ] = ( (Uint32)time_mid << 16 )
	        | ( (Uint32)time_hi_and_version );
	is[ 2 ] = ( (Uint32)clock_seq << 16 )
	        | ( (Uint32)*(const Uint16 *)ptr );
	is[ 3 ] = *(const Uint32 *)( ptr + 2 );

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

	is[ 0 ] = f1;
	is[ 1 ] = ( ( Uint32 )f2        )
	        | ( ( Uint32 )f3  << 16 );
	is[ 2 ] = ( ( Uint32 )f40       )
	        | ( ( Uint32 )f41 <<  8 )
	        | ( ( Uint32 )f42 << 16 )
	        | ( ( Uint32 )f43 << 24 );
	is[ 3 ] = ( ( Uint32 )f44       )
	        | ( ( Uint32 )f45 <<  8 )
	        | ( ( Uint32 )f46 << 16 )
	        | ( ( Uint32 )f47 << 24 );

}

UUId::UUId(
	const	UUId&		o) {

	is[0] = o.is[0];
	is[1] = o.is[1];
	is[2] = o.is[2];
	is[3] = o.is[3];

}

UUId::~UUId() {



}

// ============================================================================

void UUId::fromCompactBuffer(
	const	Buffer		& src ) {

	if ( src.getLength() != 16 ) {
		throw OutOfRange( "UUId: not 16 bytes long!" );
	}

	src.safeDumpToBuffer(
		( Uchar * )is,
		16 );

}

Buffer UUId::toCompactBuffer() const {

	return Buffer( ( const Uchar * )is, 16, true );

}

void UUId::fromBuffer(
	const	Buffer&		in ) {

	if ( in.length() != 36 ) {
		throw OutOfRange( "UUId length != 36" );
	}

	fromBuffer( in.getBufferAddr() );

}

void UUId::fromBuffer(
	const	Uchar		*ip ) {

	// 0..7 | 9..12 | 14..17 | 19..22 | 24..35

	Uchar *op = ( Uchar * )is;

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
	const	Buffer&		in) {

	if ( in.length() != 16 ) {
		throw OutOfRange("UUId (ASF file format) length != 16");
	}

	const Uchar	*ip = in.getBufferAddr();
	Uchar		*op = ( Uchar * )is;

	op[0] = ip[3]; op[1] = ip[2]; op[2] = ip[1]; op[3] = ip[0];
	op += 4; ip += 4;

	op[0] = ip[1]; op[1] = ip[0];
	op += 2; ip += 2;

	op[0] = ip[1]; op[1] = ip[0];
	op += 2; ip += 2;

	*(Uint64 *)op = *(Uint64 *)ip;

}

Buffer UUId::toBuffer() const {

	Uchar tmp[36];

	const Uchar	*ip = ( const Uchar * )is;
	Uchar		*op = tmp;

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

	return Buffer( tmp, 36, true );

}

// ============================================================================

Uchar UUId::hexVal(
	const	Uchar		c ) {

	if ( c >= '0' && c <= '9' ) return ( c - '0' );
	if ( c >= 'a' && c <= 'f' ) return ( c - 'a' + 10 );
	if ( c >= 'A' && c <= 'F' ) return ( c - 'A' + 10 );

	throw OutOfRange( "UUId: invalid hex char!" );

}

void UUId::skipDash(
	const	Uchar		* i ) {

	if ( * i != '-' ) {
		throw OutOfRange( "UUId: Missing dash!" );
	}

}

// ============================================================================

const Uchar *UUId::hexTable = (const Uchar *)"0123456789ABCDEF";

// ============================================================================

