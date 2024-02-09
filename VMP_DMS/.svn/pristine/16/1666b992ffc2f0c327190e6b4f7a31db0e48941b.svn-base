// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.Base.FourCC.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FourCC.h"

// ============================================================================

using namespace BFC;

// ========================================================================

FourCC::FourCC(
	const	Buffer &	s ) {

	if ( s.length() < 4 ) {
		v =
		m = 0;
	}
	else {
		v = *(const Uint32 *)s.getBufferAddr();
		m = 0xFFFFFFFF;
	}

}

FourCC::FourCC(
	const	Char *		vp2,
	const	Char *		mp2 ) {

	if ( length( vp2 ) != 4 || length( mp2 ) != 4 ) {
		v =
		m = 0;
	}
	else {
		Char *	vp = (Char *)&v;
		Char *	mp = (Char *)&m;
		Uint32	i = 4;
		while ( i-- ) {
			*vp++ = *vp2++;
			*mp++ = ( Char )( *mp2++ == ' ' ? 0x00 : 0xFF );
		}
	}

}

// ============================================================================

Buffer FourCC::toBuffer(
	const	Bool		b ) const {

	Buffer sub;

//	if ( v ) {
		sub = Buffer( ( const Uchar * )&v , 4, true );
//	}
//	else {
//		sub = "<NULL>";
//	}

	if ( b ) {
		sub += " ("
			+ Buffer( v & m, Buffer::Base16, 8 )
			+ ")";
	}

	return sub;

}

// ============================================================================

Uint32 FourCC::length(
	const	Char *		s ) {

	if ( ! s ) {
		return 0;
	}

	const Char * e = s;

	while ( *e++ ) {
	}

	return (Uint32)( e - s - 1 );

}

// ============================================================================





