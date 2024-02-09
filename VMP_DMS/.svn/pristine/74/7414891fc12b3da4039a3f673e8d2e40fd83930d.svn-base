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
//	VMP.M2S.Descriptor.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.System.h"
#include "BFC.Base.Utils.h"

#include "VMP.M2S.Descriptor.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::M2S::Descriptor
// ============================================================================

BFC_PTR_IMPL_NS( M2S, Descriptor )

// ============================================================================

void M2S::UnknownDescriptor::decode(
	const	Buffer &	pData ) {

	data = pData;

}

Buffer M2S::UnknownDescriptor::toBuffer() const {

	Buffer res;

	res = "UnknownDescriptor[ tag:"
		+ Buffer( tag, Buffer::Base16, 2 )
		+ ", data: \""
		+ Utils::toPrintable( data )
		+ "\" ]";

	return res;

}

// ============================================================================
// VMP::M2S::VideoStreamDescriptor ( 0x02 )
// ============================================================================

void M2S::VideoStreamDescriptor::decode(
	const	Buffer &	pData ) {

	const Uchar *	ptr = pData.getCstPtr();
	Uint32		len = pData.getLength();

	if ( len < 1 ) {
		throw InternalError( "Underflow!" );
	}

	Uchar c = ptr[ 0 ];

	multiple_frame_rate_flag	= ( ( c & 0x80 ) != 0 );
	frame_rate_code			= ( ( c & 0x78 ) >> 3 );
	MPEG_1_only_flag		= ( ( c & 0x04 ) != 0 );
	constrained_parameter_flag	= ( ( c & 0x02 ) != 0 );
	still_picture_flag		= ( ( c & 0x01 ) != 0 );

	if ( MPEG_1_only_flag ) {
		if ( len < 3 ) {
			throw InternalError( "Underflow!" );
		}
		profile_and_level_indication	= ptr[ 1 ];
		c = ptr[ 2 ];
		chroma_format			= ( ( c & 0xC0 ) >> 6 );
		frame_rate_extension_flag	= ( ( c & 0x20 ) != 0 );
	}

}

Buffer M2S::VideoStreamDescriptor::toBuffer() const {

	return Buffer( "VideoStreamDescriptor[ ... ]" );

}

// ============================================================================
// VMP::M2S::CADescriptor ( 0x09 )
// ============================================================================

BFC_PTR_IMPL_NS( M2S, CADescriptor )

// ============================================================================

void M2S::CADescriptor::decode(
	const	Buffer &	pData ) {

	const Uchar *	ptr = pData.getCstPtr();
	Uint32		len = pData.getLength();

	if ( len < 4 ) {
		CA_system_ID	= 0xFFFF;
		CA_PID		= 0xFFFF;
	}
	else {
		CA_system_ID	= ( ( ( Uint32 )ptr[ 0 ]        ) << 8 )
				| ( ( ( Uint32 )ptr[ 1 ]        )      );
		CA_PID		= ( ( ( Uint32 )ptr[ 2 ] & 0x1F ) << 8 )
				| ( ( ( Uint32 )ptr[ 3 ]        )      );
		data		= pData( 4 );
	}

}

Buffer M2S::CADescriptor::toBuffer() const {

	return Buffer( "CADescriptor[ ID: "
		+ Buffer( CA_system_ID, Buffer::Base16, 4 )
		+ ", PID: "
		+ Buffer( CA_PID, Buffer::Base16, 4 )
		+ ", data len: "
		+ Buffer( data.getLength() )
		+ " ]" );

}

// ============================================================================
// VMP::M2S::ISO639LanguageDescriptor ( 0x0A )
// ============================================================================

void M2S::ISO639LanguageDescriptor::decode(
	const	Buffer &	pData ) {

	Uint32		len = pData.getLength();

	if ( len % 4 ) {
		throw InternalError( "Invalid payload length!" );
	}

	for ( Uint32 i = 0 ; i < len ; i+= 4 ) {
		codes += pData( i, 3 );
	}

}

Buffer M2S::ISO639LanguageDescriptor::toBuffer() const {

	Buffer res;

	res = "ISO639LanguageDescriptor[";

	for ( Uint32 i = 0 ; i < codes.size() ; i++ ) {
		if ( i ) {
			res += ",";
		}
		res += codes[ i ];
	}

	res += "]";

	return res;

}

// ============================================================================
// VMP::M2S::CarouselIdentifierDescriptor ( 0x13 )
// ============================================================================

void M2S::CarouselIdentifierDescriptor::decode(
	const	Buffer &	pData ) {

	const Uchar *	ptr = pData.getCstPtr();
	Uint32		len = pData.getLength();

	if ( len < 4 ) {
		throw InternalError( "Underflow!" );
	}

	carousel_id = System::BE2Host( *( const Uint32 * )ptr );

}

Buffer M2S::CarouselIdentifierDescriptor::toBuffer() const {

	return "CarouselIdentifierDescriptor[ id: "
		+ Buffer( carousel_id, Buffer::Base16, 4 )
		+ " ]";

}

// ============================================================================
// VMP::M2S::AssociationTagDescriptor ( 0x14 )
// ============================================================================

void M2S::AssociationTagDescriptor::decode(
	const	Buffer &	/*pData*/ ) {

}

Buffer M2S::AssociationTagDescriptor::toBuffer() const {

	return "AssociationTagDescriptor[]";

}

// ============================================================================

