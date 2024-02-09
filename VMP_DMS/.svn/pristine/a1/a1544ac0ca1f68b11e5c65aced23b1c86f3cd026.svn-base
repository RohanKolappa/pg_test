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
//	BFC.Net.IPAddrUtil.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.Net.IPAddrUtil.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Buffer Net::IPAddrUtil::textToNumericFormatV4(
	const	Buffer &	src ) {

	if ( src.isEmpty() ) {
		throw InvalidArgument();
	}

	Buffer res;

	res.resize( INADDR4SZ );

	BufferArray s = BufferParser::tokenize( src, "." );
	Uint32 val;

	switch ( s.getSize() ) {
	case 1:
		val = s[ 0 ].toUint32();
		res[ 0 ] = ( Uchar )( val >> 24 );
		res[ 1 ] = ( Uchar )( val >> 16 );
		res[ 2 ] = ( Uchar )( val >>  8 );
		res[ 3 ] = ( Uchar )( val       );
		break;
	case 2:
		val = s[ 0 ].toUint32();
		if ( val > 0xff ) {
			throw InvalidArgument();
		}
		res[ 0 ] = ( Uchar )( val );
		val = s[ 1 ].toUint32();
		if ( val > 0xffffff ) {
			throw InvalidArgument();
		}
		res[ 1 ] = ( Uchar )( val >> 16 );
		res[ 2 ] = ( Uchar )( val >>  8 );
		res[ 3 ] = ( Uchar )( val       );
		break;
	case 3:
		for ( Uint32 i = 0 ; i < 2 ; i++ ) {
			val = s[ i ].toUint32();
			if ( val > 0xff ) {
				throw InvalidArgument();
			}
			res[ i ] = ( Uchar )( val );
		}
		val = s[ 2 ].toUint32();
		if ( val > 0xffff ) {
			throw InvalidArgument();
		}
		res[ 2 ] = ( Uchar )( val >> 8 );
		res[ 3 ] = ( Uchar )( val      );
		break;
	case 4:
		for ( Uint32 i = 0 ; i < 4 ; i++ ) {
			val = s[ i ].toUint32();
			if ( val > 0xff ) {
				throw InvalidArgument();
			}
			res[ i ] = ( Uchar )( val );
		}
		break;
	default:
		throw InvalidArgument();
	}

	return res;

}

Buffer Net::IPAddrUtil::textToNumericFormatV6(
	const	Buffer &	src ) {

	// Shortest valid string is "::", hence at least 2 chars
	if (src.getLength() < 2) {
		throw InvalidArgument();
	}

	int colonp;
	char ch;
	Bool saw_xdigit;
	int val;
	Buffer	srcb = src;
	Buffer	dst;

	dst.resize( INADDR16SZ );

	Uint32	srcb_length = srcb.getLength();
	Uint32	pc = src.findFwd ("%");

	if ( pc == srcb_length - 1 ) {
		throw InvalidArgument();
	}

	if ( pc < src.getLength() ) {
		srcb_length = pc;
	}

	colonp = -1;
	Uint32 i = 0, j = 0;
	/* Leading :: requires some special handling. */
	if (srcb[i] == ':')
		if (srcb[++i] != ':')
			throw InvalidArgument();
	int curtok = i;
	saw_xdigit = false;
	val = 0;
	while ( i < srcb_length ) {
		ch = srcb[i++];
		int chval = charToVal( ch );
		if (chval != -1) {
			val <<= 4;
			val |= chval;
			if (val > 0xffff)
				throw InvalidArgument();
			saw_xdigit = true;
			continue;
		}
		if (ch == ':') {
			curtok = i;
			if (!saw_xdigit) {
				if (colonp != -1)
					throw InvalidArgument();
				colonp = j;
				continue;
			} else if (i == srcb_length) {
				throw InvalidArgument();
			}
			if (j + INT16SZ > INADDR16SZ)
				throw InvalidArgument();
			dst[j++] = ( Uchar ) ((val >> 8) & 0xff);
			dst[j++] = ( Uchar ) (val & 0xff);
			saw_xdigit = false;
			val = 0;
			continue;
		}
		if (ch == '.' && ((j + INADDR4SZ) <= INADDR16SZ)) {
			Buffer ia4 = src( curtok, srcb_length - curtok );
			/* check this IPv4 address has 3 dots, ie. A.B.C.D */
			Uint32 dot_count = 0, index=0;
			while ((index = ia4.findFwd ('.', index)) < ia4.getLength()) {
				dot_count ++;
				index ++;
			}
			if (dot_count != 3) {
				throw InvalidArgument();
			}
			Buffer v4addr = textToNumericFormatV4(ia4);
			if (v4addr.isEmpty()) {
				throw InvalidArgument();
			}
			for (Uint32 k = 0; k < INADDR4SZ; k++) {
				dst[j++] = v4addr[k];
			}
			saw_xdigit = false;
			break;  /* '\0' was seen by inet_pton4(). */
		}
		throw InvalidArgument();
	}
	if (saw_xdigit) {
		if (j + INT16SZ > INADDR16SZ)
			throw InvalidArgument();
		dst[j++] = (Uchar) ((val >> 8) & 0xff);
		dst[j++] = (Uchar) (val & 0xff);
	}

	if (colonp != -1) {
		Uint32 n = j - colonp;

		if (j == INADDR16SZ)
			throw InvalidArgument();
		for (i = 1; i <= n; i++) {
			dst[INADDR16SZ - i] = dst[colonp + n - i];
			dst[colonp + n - i] = 0;
		}
		j = INADDR16SZ;
	}
	if (j != INADDR16SZ)
		throw InvalidArgument();

	Buffer newdst = convertFromIPv4MappedAddress( dst );

	return ( newdst.hasContent() ? newdst : dst );

}

// ============================================================================

Bool Net::IPAddrUtil::isIPv4LiteralAddress(
	const	Buffer &	src ) {

	try {
		textToNumericFormatV4( src );
	}
	catch ( Exception & ) {
		return false;
	}

	return true;

}

Bool Net::IPAddrUtil::isIPv6LiteralAddress(
	const	Buffer &	pSrc ) {

	try {
		textToNumericFormatV6( pSrc );
	}
	catch ( Exception & ) {
		return false;
	}

	return true;

}

// ============================================================================

Buffer Net::IPAddrUtil::convertFromIPv4MappedAddress(
	const	Buffer &	addr ) {

	return ( isIPv4MappedAddress( addr )
		? addr( 12, INADDR4SZ )
		: Buffer() );

}

// ============================================================================

Bool Net::IPAddrUtil::isIPv4MappedAddress(
	const	Buffer &	addr ) {

	return ( addr.getLength() >= INADDR16SZ
	      && addr[  0 ] == 0x00
	      && addr[  1 ] == 0x00
	      && addr[  2 ] == 0x00
	      && addr[  3 ] == 0x00
	      && addr[  4 ] == 0x00
	      && addr[  5 ] == 0x00
	      && addr[  6 ] == 0x00
	      && addr[  7 ] == 0x00
	      && addr[  8 ] == 0x00
	      && addr[  9 ] == 0x00
	      && addr[ 10 ] == 0xff
	      && addr[ 11 ] == 0xff );

}

// ============================================================================

