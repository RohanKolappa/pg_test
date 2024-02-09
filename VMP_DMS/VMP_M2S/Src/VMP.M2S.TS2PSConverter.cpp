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
//	VMP.M2S.TS2PSConverter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.IO.Device.h"

#include "VMP.M2S.PESPacket.h"
#include "VMP.M2S.TS2PSConverter.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TS2PSConverter )

// ============================================================================

M2S::TS2PSConverter::TS2PSConverter() {

	setObjectName( "VMP.M2S.TS2PSConverter" );

}

// ============================================================================

void M2S::TS2PSConverter::attachDevice(
		IO::DevicePtr	pDevice ) {

	msgDbg( "attachDevice()" );

	device = pDevice;

	const char *static_packs =
		"\x00\x00\x01\xBA"		// pack_start_code
		"\x44\x00\x04\x00\x04\x01"	// SCR
		"\x01\x87\x03"			// program_mux_rate
		"\xF8"
		"\x00\x00\x01\xBB"		// header_start_code
		"\x00\x0C"			// header_length
		"\x80\xC3\x81"			// rate_bound
		"\x02\xE0\x7F"			// flags
		"\xC0\xC0\x18"
		"\xE0\xE0\xA0";

	device->putBytes( Buffer( (const Uchar *)static_packs, 32 ) );

	syncPoint.kill();

}

void M2S::TS2PSConverter::detachDevice() {

	device.kill();

}

// ============================================================================

void M2S::TS2PSConverter::putObject(
		SPtr		pSObject ) {

	if ( pSObject.isA< TimeStamp >() ) {
		handlePCR( pSObject );
	}
	else if ( pSObject.isA< PESPacket >() ) {
		handlePES( pSObject );
	}
	else {
		throw InternalError();
	}

}

// ============================================================================

void M2S::TS2PSConverter::handlePES(
		PESPacketPtr	packet ) {

	Uint32 id = packet->streamType;

	if ( ( id & 0xE0 ) == 0xC0 ) {
		// Audio stream...
		id &= 0xE0;
	}
	else if ( ( id & 0xF0 ) == 0xE0 ) {
		// Video stream...
		id &= 0xF0;
	}
	else {
		throw InvalidArgument( "Invalid stream_id: "
			+ Buffer( id, Buffer::Base16, 2 ) );
	}

//	if ( packet->PTS_flag ) {
//		msgDbg( Buffer( id, Buffer::Base16, 2 )
//			+ " PTS: "
//			+ packet->PTS.toTime::Clock().toBuffer() );
//	}

	packet->streamType = id;

	Buffer dump = packet->packetDump;

	/* if ( packet->packetLength ) {
		msgDbg( "consume(): directly pushing PES packet..."
			+ Buffer( id, Buffer::Base16, 2 ) );
		device->putBytes( dump );
	}
	else if ( dump.length() - 6 < 0xFF00 ) {
		msgDbg( "consume(): pushing fixed PES packet..."
			+ Buffer( id, Buffer::Base16, 2 ) );
		Uint32 l = dump.length() - 6;
		dump[ 4 ] = ( l >> 8 );
		dump[ 5 ] = ( l & 0xFF );
		device->putBytes( dump );
	}
	else */ {
//		msgDbg( "consume(): splitting then pushing PES packets..."
//			+ Buffer( id, Buffer::Base16, 2 ) );
		Buffer data = packet->data_byte;
		Uint32 towrite = data.length();
		Uint32 start = 0;
		for ( Uint32 i = 0 ; towrite ; i++ ) {
			PESPacketPtr tmp = new PESPacket( true );
			tmp->streamType = id;
			if ( ! i ) {
				tmp->data_align_indicator = packet->data_align_indicator;
				if ( packet->PTS_flag ) {
					tmp->PTS_flag = true;
					tmp->PTS = packet->PTS - *syncPoint;
					if ( packet->DTS_flag ) {
						tmp->DTS_flag = true;
						tmp->DTS = packet->DTS - *syncPoint;
					}
				}
			}
			Uint32 len = 0xF000;
			if ( len > towrite ) {
				len = towrite;
			}
			tmp->data_byte = data( start, len );
			start += len;
			towrite -= len;
			dump = tmp->encode();
			device->putBytes( dump );
		}
	}


}

// ============================================================================

void M2S::TS2PSConverter::handlePCR(
		TimeStampCPtr	pcr ) {

	if ( ! syncPoint ) {
		msgDbg( "PCR: " + pcr->toClock().toBuffer() );
		syncPoint = pcr;
	}

}

// ============================================================================

