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
//	VMP.M2S.TSPMTFramer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.DescriptorDecoder.h"
#include "VMP.M2S.TSPMT.h"
#include "VMP.M2S.TSPMTFramer.h"
#include "VMP.M2S.TSSection.h"
#include "VMP.M2S.TSTable.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSPMTFramer )

// ============================================================================

M2S::TSPMTFramer::TSPMTFramer(
		DescriptorDecoderPtr
				pDescrDecoder,
	const	Uint32		pPmtPID ) :

	pmtPID( pPmtPID ) {

	setObjectName( "VMP.M2S.TSPMTFramer( PID["
		+ Buffer( pmtPID, Buffer::Base16, 4 ) + "] )" );

	descrDecoder = pDescrDecoder;

}

// ============================================================================

void M2S::TSPMTFramer::putObject(
		SPtr		pTSTable ) {

	TSTablePtr	table = pTSTable;

//	msgDbg( "Got table!" );

	// Sanity checks...

	if ( table->getTableId() != 0x02 ) {
		msgWrn( "Invalid table_id["
			+ Buffer( table->getTableId() ) + "]" );
		return;
	}

	if ( ! table->getSyntaxIndicator() ) {
		msgWrn( "PMT doesn't use section syntax ?! Huh..." );
		return;
	}

//	msgDbg( "Got PMT! (presumably)" );
//	msgDbg( "... version: " + Buffer( table->getVersion() ) );

	if ( table->getNbrSections() != 1 ) {
		msgWrn( "PMT contains more than 1 section!" );
		return;
	}

	TSSectionPtr	section = table->getSection( 0 );
	Buffer		payload = section->buffer;
	Uint32		prog_nbr = ( (Uint32)( payload[ 3 ] ) << 8 )
			         | ( (Uint32)( payload[ 4 ] )      );

	if ( ! progId.contains( prog_nbr ) ) {
		msgWrn( "Unexpecting PMT for unknown program!" );
		return;
	}

//	if ( progId != prog_nbr ) {
//		msgWrn( "Mutating program_id!" );
//		msgWrn( "In PAT: " + Buffer( progId, Buffer::Base16, 4 ) );
//		msgWrn( "In PMT: " + Buffer( prog_nbr, Buffer::Base16, 4 ) );
//		return;
//	}

	TSPMTPtr	 pmt = new TSPMT;

	pmt->prog_nbr = prog_nbr;

	Uint32		length = payload.length();

	if ( length < 16 ) {
		msgWrn( "Invalid payload length!" );
		return;
	}

	pmt->pcr_pid = ( (Uint32)( payload[ 8 ] & 0x1F ) << 8 )
	             | ( (Uint32)( payload[ 9 ]        )      );

	Uint32		pil = ( (Uint32)( payload[ 10 ] & 0x0F ) << 8 )
			    | ( (Uint32)( payload[ 11 ]        )      );

	if ( pil ) {
		if ( length < 16 + pil ) {
			msgWrn( "Invalid payload length to read prog descriptors!" );
			return;
		}
		Buffer program_info = payload( 12, pil );
		pmt->descriptors = descrDecoder->decode( program_info );
	}

	Uint32		esl = length - 16 - pil;
	const Uchar *	address = payload.getBufferAddr() + 12 + pil;

	while ( esl ) {

		if ( esl < 5 ) {
			msgWrn( "EStream entry underflow!" );
			return;
		}

		Uint32 stream_type = address[ 0 ];
		Uint32 elem_PID = ( ( (Uint32)( address[ 1 ] & 0x1F ) ) << 8 )
		                | ( ( (Uint32)( address[ 2 ]        ) )      );
		Uint32 esinfo_len = ( ( (Uint32)( address[ 3 ] & 0x0F ) ) << 8 )
		                  | ( ( (Uint32)( address[ 4 ]        ) )      );

		EStreamPtr estream = new EStream;

		estream->stream_type = stream_type;
		estream->pid = elem_PID;

		if ( esinfo_len ) {
			if ( esl < 5 + esinfo_len ) {
				msgWrn( "EStream entry underflow (for extra info)!" );
				return;
			}
			Buffer esinfo( address + 5, esinfo_len );
			estream->descriptors = descrDecoder->decode( esinfo );
		}

		pmt->estreams += estream;

		esl -= 5 + esinfo_len;
		address += 5 + esinfo_len;

	}

	getPeerConsumer()->putObject( pmt );

}

// ============================================================================

