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
//	TBFC.Base.Utils.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.Utils.h"

// ============================================================================

using namespace TBFC;

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
	const	Buffer		& s ) {

	Buffer res;

	for (Uint32 i = 0 ; i < s.length() ; i++)
		switch (s[i]) {
		case 0x00 : res += "<NUL>"; break;
		case 0x01 : res += "<SOH>"; break;
		case 0x02 : res += "<STX>"; break;
		case 0x03 : res += "<ETX>"; break;
		case 0x04 : res += "<EOT>"; break;
		case 0x05 : res += "<ENQ>"; break;
		case 0x06 : res += "<ACK>"; break;
		case 0x07 : res += "<BEL>"; break;
		case 0x08 : res += "<BS>"; break;
		case 0x09 : res += "<HT>"; break;
		case 0x0A : res += "<LF>"; break;
		case 0x0B : res += "<VT>"; break;
		case 0x0C : res += "<FF>"; break;
		case 0x0D : res += "<CR>"; break;
		case 0x0E : res += "<SO>"; break;
		case 0x0F : res += "<SI>"; break;
		case 0x10 : res += "<DLE>"; break;
		case 0x11 : res += "<DC1>"; break;
		case 0x12 : res += "<DC2>"; break;
		case 0x13 : res += "<DC3>"; break;
		case 0x14 : res += "<DC4>"; break;
		case 0x15 : res += "<NAK>"; break;
		case 0x16 : res += "<SYN>"; break;
		case 0x17 : res += "<ETB>"; break;
		case 0x18 : res += "<CAN>"; break;
		case 0x19 : res += "<EM>"; break;
		case 0x1A : res += "<SUB>"; break;
		case 0x1B : res += "<ESC>"; break;
		case 0x1C : res += "<FS>"; break;
		case 0x1D : res += "<GS>"; break;
		case 0x1E : res += "<RS>"; break;
		case 0x1F : res += "<US>"; break;
		default : res += s[i];
		}

	return res;

}

const Uchar Utils::hexCharTable[] = "0123456789ABCDEF";

Buffer Utils::toHex(
	const	Buffer		& s ) {

	Buffer res;

	if ( s.isEmpty() ) {
		return res;
	}

	Uint32 l = s.getLength();
	Uint32 n = l * 3 - 1;

	res.resize( n );

	Uint32 i, j, u;

	for ( i = 0 , j = 0 ; i < l ; i++ ) {
		if ( i ) {
			res[ j++ ] = '.';
		}
		u = ( Uint32 )s[ i ];
		res[ j++ ] = hexCharTable[ ( u & 0xF0 ) >> 4 ];
		res[ j++ ] = hexCharTable[ ( u & 0x0F )      ];
	}

	return res;

}

// ============================================================================

