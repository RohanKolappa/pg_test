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
//	BFC.Base.Utils.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

static Bool wsCharTable[256];
static Bool wsCharTableInitialized = false;

static void initwsCharTable() {

	for (Uint32 i = 0 ; i < 256 ; i++)
		wsCharTable[i] = false;
	wsCharTable[(int) ' '] = true;
	wsCharTable[(int)'\t'] = true;
	wsCharTable[(int)'\r'] = true;
	wsCharTable[(int)'\n'] = true;
	wsCharTableInitialized = true;

}

void Utils::skipWhiteSpaces(
	const	Buffer		& src,
		Uint32		& pos ) {

	if ( ! wsCharTableInitialized ) {
		initwsCharTable();
	}

	Uint32		len = src.getLength();
	const Uchar	* ptr = src.getCstPtr();

	while ( pos < len && wsCharTable[ ( Uint32 )ptr[ pos ] ] ) {
		pos++;
	}

}

void Utils::skipNonWhiteSpaces(
	const	Buffer		& src,
		Uint32		& pos ) {

	if ( ! wsCharTableInitialized ) {
		initwsCharTable();
	}

	Uint32		len = src.getLength();
	const Uchar	* ptr = src.getCstPtr();

	while ( pos < len && ! wsCharTable[ ( Uint32 )ptr[ pos ] ] ) {
		pos++;
	}

}

Uint32 Utils::findMatchingChar(
	const	Buffer		& src,
	const	Uint32		start ) {

	uchar		c = src[ start ];
	Uint32		stop = 0;
	Uint32		len = src.getLength();
	const Uchar	* ptr = src.getCstPtr();

	if ( c == '"' ) {
		stop = start + 1;
		while ( stop < len && ptr[ stop ] != '"' ) {
			stop++;
		}
		if ( stop == len ) {
			stop = 0;
		}
	}

	return stop;

}

// ============================================================================

Buffer Utils::toPrintable(
	const	Buffer &	s ) {

	Buffer res;

	for ( Uint32 i = 0 ; i < s.getLength() ; i++ ) {
		Uchar c = s[ i ];
		if ( c >= 0x20 && c <= 0x7F ) {
			res += c;
		}
		else {
			res += '<';
			res += Buffer( ( Uint32 )c, Buffer::Base16, 2 );
			res += '>';
		}
//		switch (s[i]) {
//		case 0x00 : res += "<NUL>"; break;
//		case 0x01 : res += "<SOH>"; break;
//		case 0x02 : res += "<STX>"; break;
//		case 0x03 : res += "<ETX>"; break;
//		case 0x04 : res += "<EOT>"; break;
//		case 0x05 : res += "<ENQ>"; break;
//		case 0x06 : res += "<ACK>"; break;
//		case 0x07 : res += "<BEL>"; break;
//		case 0x08 : res += "<BS>"; break;
//		case 0x09 : res += "<HT>"; break;
//		case 0x0A : res += "<LF>"; break;
//		case 0x0B : res += "<VT>"; break;
//		case 0x0C : res += "<FF>"; break;
//		case 0x0D : res += "<CR>"; break;
//		case 0x0E : res += "<SO>"; break;
//		case 0x0F : res += "<SI>"; break;
//		case 0x10 : res += "<DLE>"; break;
//		case 0x11 : res += "<DC1>"; break;
//		case 0x12 : res += "<DC2>"; break;
//		case 0x13 : res += "<DC3>"; break;
//		case 0x14 : res += "<DC4>"; break;
//		case 0x15 : res += "<NAK>"; break;
//		case 0x16 : res += "<SYN>"; break;
//		case 0x17 : res += "<ETB>"; break;
//		case 0x18 : res += "<CAN>"; break;
//		case 0x19 : res += "<EM>"; break;
//		case 0x1A : res += "<SUB>"; break;
//		case 0x1B : res += "<ESC>"; break;
//		case 0x1C : res += "<FS>"; break;
//		case 0x1D : res += "<GS>"; break;
//		case 0x1E : res += "<RS>"; break;
//		case 0x1F : res += "<US>"; break;
//		default : res += s[i];
//		}
	}

	return res;

}

Buffer Utils::keepPrintable(
	const	Buffer &	s,
	const	Uchar		f,
	const	Bool		k ) {

	Uchar	mask = ( Uchar )( k ? 0x7F : 0xFF );
	Buffer	res;

	res.resize( s.getLength() );

	for ( Uint32 i = 0 ; i < s.getLength() ; i++ ) {
		Uchar c = ( Uchar )( s[ i ] & mask );
		res[ i ] = ( c >= 0x20 && c <= 0x7F ? c : f );
	}

	return res;

}

Buffer Utils::trimNonPrintable(
	const	Buffer &	s ) {

	Buffer	res;

	res.resize( s.getLength() );

	const Uchar *	src = s.getCstPtr();
	Uchar *		dst = res.getVarPtr();

	for ( Uint32 i = s.getLength() ; i ; i-- ) {
		Uchar c = *src++;
		if ( c >= 0x20 ) {
			*dst++ = c;
		}
	}

	res.resize( dst - res.getVarPtr() );

	return res;

}

const Uchar Utils::hexCharTable[] = "0123456789ABCDEF";

Bool Utils::isHexChar(
	const	Uchar		c ) {

	return ( ( c >= '0' && c <= '9' )
	      || ( c >= 'A' && c <= 'F' )
	      || ( c >= 'a' && c <= 'f' ) );

}

Buffer Utils::toHex(
	const	Buffer &	s,
	const	Bool		d ) {

	Buffer res;

	if ( s.isEmpty() ) {
		return res;
	}

	Uint32 l = s.getLength();
	Uint32 n = ( d ? l * 3 - 1 : l * 2 );

	res.resize( n );

	Uint32 i, j, u;

	for ( i = 0 , j = 0 ; i < l ; i++ ) {
		if ( i && d ) {
			res[ j++ ] = '.';
		}
		u = ( Uint32 )s[ i ];
		res[ j++ ] = hexCharTable[ ( u & 0xF0 ) >> 4 ];
		res[ j++ ] = hexCharTable[ ( u & 0x0F )      ];
	}

	return res;

}

Uchar Utils::toHexL(
	const	Uchar		c ) {

	return hexCharTable[ ( c & 0x0F ) ];

}

Uchar Utils::toHexH(
	const	Uchar		c ) {

	return hexCharTable[ ( c & 0xF0 ) >> 4 ];

}

Uint32 Utils::fromHex(
	const	Uchar		c ) {

	if ( c >= '0' && c <= '9' )
		return ( c - '0' );

	if ( c >= 'a' && c <= 'f' )
		return ( c + 10 - 'a' );

	if ( c >= 'A' && c <= 'F' )
		return ( c + 10 - 'A' );

	throw InvalidArgument();

}

Uint32 Utils::fromHex(
	const	Uchar		ch,
	const	Uchar		cl ) {

	return ( ( fromHex( ch ) << 4 )
	       | ( fromHex( cl )      ) );

}

Buffer Utils::fromHex(
	const	Buffer &	s,
	const	Bool		d ) {

	Buffer	res;
	Uint32	len = ( d ? ( s.getLength() + 1 ) / 3 : s.getLength() / 2 );

	res.resize( len );

	Uchar *		dst = res.getVarPtr();
	const Uchar *	src = s.getCstPtr();

	while ( len-- ) {
		*dst++ = ( Uchar )( ( fromHex( src[ 0 ] ) << 4 )
		                  | ( fromHex( src[ 1 ] )      ) );
		src += ( d ? 3 : 2 );
	}

	return res;

}

// ============================================================================

