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
//	VMP.M2S.TSPacket.cpp
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

#include "BFC.Debug.DObject.h"

#include "VMP.M2S.TSPacket.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSPacket )

// ============================================================================

void M2S::TSPacket::decode(
	const	Uint64		pOffs,
	const	Buffer &	pData ) {

//	msgDbg( "decode(): " + Utils::toHex( pData ) );

	const Uchar *	ptr = pData.getCstPtr();
	Uint32		len = pData.getLength();

	if ( len != 188 ) {
		throw InvalidArgument( "len != 188" );
	}

	if ( ptr[ 0 ] != 0x47 ) {
		throw InvalidArgument( "Not aligned!" );
	}

	packOffs = pOffs;
	packCopy = pData;

	// Read header...

	packHder = System::Host2BE( *( const Uint32 * )ptr );

	ptr += 4;
	len -= 4; // remaining for af + payload

	// If transport_error_indicator is set, skip this packet!

	if ( getTransportErrorIndicator() ) {
//		msgWrn( "decode(): transport_error_indicator is set!" );
		return;
	}

	// If present, read adaptation_field...

	if ( hasAdaptationField() ) {
		adaptFld.decode( ptr, len );
	}

	// If present, read payload...

	if ( ! len ) {
		if ( hasPayload() ) {
			throw InternalError( "Need payload, but no bytes left after AF!" );
		}
		packData.kill();
		return;
	}

	if ( ! hasPayload() ) {
		throw InternalError( "No payload, but bytes left after AF!" );
	}

	packData = packCopy( 188 - len );

}

// ============================================================================

Buffer M2S::TSPacket::toBuffer() const {

	Buffer res;

	res += "[" + Buffer( packOffs, Buffer::Base16, 8 ) + "] ";
	res += ( getTransportErrorIndicator() ? 'E' : '.' );
	res += ( getPayloadUnitStartIndicator() ? 'S' : '.' );
	res += ( getTransportPriority() ? 'P' : '.' );
	res += " | " + Buffer( getPID(), Buffer::Base16, 4 );
	res += " | ";
	res += ( getTransportScramblingControl() & 0x02 ? 'S' : '.' );
	res += ( getTransportScramblingControl() & 0x01 ? 's' : '.' );
	res += ( hasAdaptationField() ? 'A' : '.' );
	res += ( hasPayload() ? 'P' : '.' );
	res += " | " + Buffer( getContinuityCounter(), Buffer::Base16, 1 );

	if ( hasAdaptationField() ) {
		res += " | " + getAdaptationField().toBuffer();
	}

	if ( hasPayload() ) {
		res += " | (" + Buffer( getPayload().getLength() )
			+ "): " + Utils::toHex( getPayload() );
	}

	return res;

}

// ============================================================================

void M2S::TSPacket::AdaptationField::decode(
	const	Uchar *&	ptr,
		Uint32 &	len ) {

	if ( ! len ) {
		throw InternalError( "Underflow while reading AF length!" );
	}

	adaptation_field_length = *ptr++;
	len--;

	if ( ! adaptation_field_length ) {
		discontinuity_indicator		= false;
		random_access_indicator		= false;
		elementary_stream_priority_indicator = false;
		PCR_flag			= false;
		OPCR_flag			= false;
		splicing_point_flag		= false;
		transport_private_data_flag	= false;
		adaptation_field_extension_flag	= false;
		return;
	}

	if ( adaptation_field_length > 183 ) {
		throw InternalError( "adaptation_field_length > 183" );
	}

	if ( adaptation_field_length > len ) {
		throw InternalError( "Underflow while reading AF!" );
	}

	Uint32 a = adaptation_field_length; // available for the fields...
	len -= a; // final update

	Uchar c = *ptr++;
	a--;

	discontinuity_indicator			= ( ( c & 0x80 ) != 0 );
	random_access_indicator			= ( ( c & 0x40 ) != 0 );
	elementary_stream_priority_indicator	= ( ( c & 0x20 ) != 0 );
	PCR_flag				= ( ( c & 0x10 ) != 0 );
	OPCR_flag				= ( ( c & 0x08 ) != 0 );
	splicing_point_flag			= ( ( c & 0x04 ) != 0 );
	transport_private_data_flag		= ( ( c & 0x02 ) != 0 );
	adaptation_field_extension_flag		= ( ( c & 0x01 ) != 0 );

	if ( PCR_flag ) {
		if ( a < 6 ) {
			throw InternalError( "Underflow for PCR" );
		}
		PCR = new TimeStamp;
		PCR->decode_33_6_9( ptr );
		ptr += 6;
		a -= 6;
	}

	if ( OPCR_flag ) {
		if ( a < 6 ) {
			throw InternalError( "Underflow for OPCR" );
		}
		OPCR = new TimeStamp;
		OPCR->decode_33_6_9( ptr );
		ptr += 6;
		a -= 6;
	}

	if ( splicing_point_flag ) {
		if ( a < 1 ) {
			throw InternalError( "Underflow for splicing_point" );
		}
		splice_countdown = (Int32)(Char)*ptr++;
		a--;
	}

	if ( transport_private_data_flag ) {
		if ( a < 1 ) {
			throw InternalError( "Underflow for transport_private_data_length" );
		}
		transport_private_data_length = (Uint32)*ptr++;
		a--;
		if ( a < transport_private_data_length ) {
			throw InternalError( "Underflow for transport_private_data" );
		}
		private_data_byte = Buffer( ptr, transport_private_data_length );
		ptr += transport_private_data_length;
		a -= transport_private_data_length;
	}

	if ( adaptation_field_extension_flag ) {
		if ( a < 1 ) {
			throw InternalError( "Underflow for adaptation_field_extension_length" );
		}
		adaptation_field_extension_length = (Uint32)*ptr++; a--;
		if ( a < adaptation_field_extension_length ) {
			throw InternalError( "Underflow for adaptation_field_extension" );
		}
		ptr += adaptation_field_extension_length;
		a -= adaptation_field_extension_length;
	}

	if ( a ) {
		// remove stuffing bytes...
		ptr += a;
	}

}

// ============================================================================

Buffer M2S::TSPacket::AdaptationField::toBuffer() const {

	Buffer res;

	if ( discontinuity_indicator ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
		res += "disc";
	}

	if ( random_access_indicator ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
		res += "rand";
	}

	if ( elementary_stream_priority_indicator ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
		res += "prio";
	}

	if ( PCR_flag ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
//		res += "PCR: " + PCR.toClock().dateTimeToBuffer( false, true );
		res += "PCR: " + PCR->toBuffer();
	}

	if ( OPCR_flag ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
//		res += "OPCR: " + OPCR.toClock().dateTimeToBuffer( false, true );
		res += "OPCR: " + OPCR->toBuffer();
	}

	if ( splicing_point_flag ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
		res += "splc: " + Buffer( splice_countdown );
	}

	if ( transport_private_data_flag ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
		res += "data";
	}

	if ( adaptation_field_extension_flag ) {
		if ( res.hasContent() ) {
			res += ", ";
		}
		res += "ext";
	}

	return res;

}

// ============================================================================

