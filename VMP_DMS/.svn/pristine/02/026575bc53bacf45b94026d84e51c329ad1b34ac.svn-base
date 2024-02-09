// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.TSSection.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.TSSection.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSSection )

// ============================================================================

M2S::TSSection::TSSection() {

	setObjectName( "VMP.M2S.TSSection" );

}

// ============================================================================

void M2S::TSSection::decode(
	const	Uchar *&	ptr,
		Uint32 &	len ) {

	Uint32 toRead;

	if ( buffer.length() < 3 ) {
		// First try to read the section_length!
		toRead = 3 - buffer.length();
		if ( len < toRead ) {
			// Really a partial section here!
			buffer += Buffer( ptr, len, true );
			ptr += len;
			len = 0;
			return;
		}
		buffer += Buffer( ptr, toRead, true );
		ptr += toRead;
		len -= toRead;
		// Now decode the first 3 bytes in 'buffer'...
		table_id = buffer[0];
		section_syntax_indicator = ( ( buffer[1] & 0x80 ) != 0 );
		section_length = ( (Uint32)( buffer[1] & 0x0F ) << 8 )
		               | ( (Uint32)( buffer[2]        )      );
	}

	toRead = section_length + 3 - buffer.length();

	if ( len < toRead ) {
		buffer += Buffer( ptr, len, true );
		ptr += len;
		len = 0;
		return;
	}

	buffer += Buffer( ptr, toRead, true );

	ptr += toRead;
	len -= toRead;

	// Now, we've got a complete section!

	if ( ! section_syntax_indicator ) {
		return;
	}

	if ( section_length < 8 + 4 - 3 ) {
		msgWrn( "underflow with section_syntax_indicator!" );
		buffer.kill();
		return;
	}

	table_id_ext = ( (Uint32)( buffer[ 3 ] ) << 8 )
	             | ( (Uint32)( buffer[ 4 ] )      );
	version_number = ( ( buffer[ 5 ] >> 1 ) & 0x1F );
	current_next_indicator = ( ( buffer[ 5 ] & 0x01 ) != 0 );
	section_number = buffer[ 6 ];
	last_section_number = buffer[ 7 ];

	if ( section_number > last_section_number ) {
		msgWrn( "section number > last!" );
		buffer.kill();
		return;
	}

	CRC_32 = *(const Uint32 *)( buffer.getBufferAddr() + buffer.length() - 4 );

// FIXME: check CRC!

}

// ============================================================================

Bool M2S::TSSection::isReady() const {

	return ( buffer.length() >= 3
	      && buffer.length() == section_length + 3 );

}

// ============================================================================

Buffer M2S::TSSection::getPayload() const {

	if ( ! isReady() ) {
		return Buffer();
	}

	return ( section_syntax_indicator
		? buffer( 8, buffer.getLength() - 12 )
		: buffer( 3 ) );

}

// ============================================================================

