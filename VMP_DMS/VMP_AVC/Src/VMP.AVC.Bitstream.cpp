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
//	VMP.AVC.Bitstream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.AVC.Bitstream.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, Bitstream )

// ============================================================================

AVC::Bitstream::Bitstream(
	const	Buffer &	pData ) :

	strBuf	( pData ),
	strOff	( 0 ),
	strLen	( strBuf.getLength() ),
	strBit	( strBuf.getLength() * 8 ),
	strPtr	( strBuf.getCstPtr() ) {

	setObjectName( "VMP.AVC.Bitstream" );

//	msgDbg( "data: " + Utils::toHex( pData ) );

}

// ============================================================================

Uint32 AVC::Bitstream::read_u_v(
	const	Uint32		pCount ) {

	return readFLC( pCount );

}

Uint32 AVC::Bitstream::read_u_1() {

	return readFLC( 1 );

}

Uint32 AVC::Bitstream::read_ue_v() {

	return readVLC( false );

}

Int32 AVC::Bitstream::read_se_v() {

	return readVLC( true );

}

// ============================================================================

Uint32 AVC::Bitstream::readFLC(
	const	Uint32		pToRead ) {

	if ( strOff + pToRead > strBit ) {
		throw InternalError( "Underflow! off: "
			+ Buffer( strOff ) + " + req: " + Buffer( pToRead )
			+ " > len: " + Buffer( strBit ) );
	}

	Uint32		bytOff	= ( strOff >> 3 );
	Int32		bitOff	= ( 7 - ( strOff & 0x07 ) );
	const Uchar *	tmpPtr  = strPtr + bytOff;

	Uint32		res = 0;

	for ( Uint32 counter = pToRead ; counter ; counter-- ) {
		res <<= 1;
		res |= ( ( *tmpPtr >> bitOff-- ) & 0x01 );
		if ( bitOff == -1 ) {
			tmpPtr++;
			bitOff = 7;
		}
	}

	strOff += pToRead;

	return res;

}

Uint32 AVC::Bitstream::readVLC(
	const	Bool		pSigned ) {

//	msgDbg( "readVLC(): strOff = " + Buffer( strOff ) );

	Uint32		bytOff	= ( strOff >> 3 );
	Int32		bitOff	= ( 7 - ( strOff & 0x07 ) );
	const Uchar*	tmpPtr	= strPtr + bytOff;
	Uint32		symLen	= 0;

	while ( ( ( *tmpPtr >> bitOff ) & 0x01 ) == 0 ) {
		symLen++;
		strOff++;
		bitOff--;
		if ( bitOff == -1 ) {
			tmpPtr++;
			bytOff++;
			bitOff = 7;
		}
	}

	strOff++;

//	msgDbg( "readVLC(): symLen = " + Buffer( symLen ) + ", strOff = " + Buffer( strOff ) );

	if ( bytOff + ( ( symLen + 7 ) >> 3 ) > strLen ) {
		throw InternalError();
	}

	Uint32	symVal = 0;

	for ( Uint32 i = symLen ; i ; i-- ) {
		strOff++;
		bitOff--;
		if ( bitOff == -1 ) {
			tmpPtr++;
			bitOff = 7;
		}
		symVal <<= 1;
		symVal |= ( ( *tmpPtr >> bitOff ) & 0x01 );
	}

	Uint32 n = ( 1 << symLen ) + symVal - 1;

//	msgDbg( "readVLC(): codeNum = " + Buffer( n ) );

	Uint32 r = ( ! pSigned
		? n
		: ( ( n & 0x01 ) == 0 )
		? -( Int32 )( ( n + 1 ) >> 1 )
		:( ( n + 1 ) >> 1 ) );

//	msgDbg( "readVLC(): res = " + Buffer( r ) );

	return r;

}

// ============================================================================

