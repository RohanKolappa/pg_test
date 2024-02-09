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
//	VMP.M2S.TSPATFramer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.TSPAT.h"
#include "VMP.M2S.TSPATFramer.h"
#include "VMP.M2S.TSSection.h"
#include "VMP.M2S.TSTable.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSPATFramer )

// ============================================================================

M2S::TSPATFramer::TSPATFramer() {

	setObjectName( "VMP.M2S.TSPATFramer" );

}

// ============================================================================

void M2S::TSPATFramer::putObject(
		SPtr		pTSTable ) {

	TSTablePtr	table = pTSTable;

//	msgDbg( "Got table!" );

	// Sanity checks...

	if ( table->getTableId() != 0x00 ) {
		msgWrn( "Invalid table_id["
			+ Buffer( table->getTableId() ) + "]" );
		return;
	}

	if ( ! table->getSyntaxIndicator() ) {
		msgWrn( "PAT doesn't use section syntax ?! Huh..." );
		return;
	}

//	msgDbg( "Got PAT! (presumably)" );
//	msgDbg( "... version: " + Buffer( table->getVersion() ) );

	Uint32		nbrSections = table->getNbrSections();
	TSSectionPtr	section;
	Buffer		payload;
	Uint32		i;
	Uint32		length;
	Uint32		entries;
	const Uchar	*address;
	Uint32		tsId;
	Uint32		progNbr;
	Uint32		progPID;

	TSPATPtr pat = new TSPAT;

	pat->setVersion( table->getVersion() );

	for ( i = 0 ; i < nbrSections ; i++ ) {

//		msgDbg( "Scanning section["
//			+ Buffer( i + 1 ) + "/"
//			+ Buffer( nbrSections ) + "]..." );

		section = table->getSection( i );
		payload = section->buffer;

//		msgDbg( "payload: " + Utils::toHex( payload ) );

		tsId = ( (Uint32)( payload[ 3 ] ) << 8 )
		     | ( (Uint32)( payload[ 4 ] )      );

		if ( ! i ) {
//			msgDbg( "Found transport_stream_id ["
//				+ Buffer( tsId, Buffer::Base16, 4 )
//				+ "]!" );
			pat->setTSId( tsId );
		}
		else if ( pat->getTSId() != tsId ) {
			msgWrn( "Mutating transport_stream_id!" );
		}

		length = payload.length();

		if ( length % 4 ) {
			msgWrn( "Invalid section["
				+ Buffer( i ) + "] payload length!" );
			return;
		}

		entries = ( length - 12 ) / 4;
		address = payload.getBufferAddr() + 8;

//		msgDbg( "Found [" + Buffer( entries )
//			+ "] entries in section [" + Buffer( i + 1 )
//			+ "]..." );

		while ( entries-- ) {

			progNbr = ( ( (Uint32)( address[ 0 ]        ) ) << 8 )
			        | ( ( (Uint32)( address[ 1 ]        ) )      );
			progPID = ( ( (Uint32)( address[ 2 ] & 0x1F ) ) << 8 )
			        | ( ( (Uint32)( address[ 3 ]        ) )      );

			address += 4;

			if ( progNbr ) {
				if ( pat->map.contains( progNbr ) ) {
					msgWrn( "Duplicate program number ["
						+ Buffer( progNbr, Buffer::Base16, 4 )
						+ "]!" );
					msgWrn( "Old PID: " + Buffer( pat->map[ progNbr ], Buffer::Base16, 4 ) );
					msgWrn( "New PID: " + Buffer( progPID, Buffer::Base16, 4 ) );
				}
				else {
//					msgDbg( "Found PMT PID ["
//						+ Buffer( progPID, Buffer::Base16, 4 )
//						+ "] for program ["
//						+ Buffer( progNbr, Buffer::Base16, 4 )
//						+ "]" );
					pat->map.add( progNbr, progPID );
				}
			}
			else {
				if ( pat->getNetworkPID() != TSPAT::InvalidId ) {
					if ( pat->getNetworkPID() == progPID ) {
						msgWrn( "Duplicate network_PID!" );
					}
					else {
						msgWrn( "Mutating network_PID:" );
						msgWrn( "Old PID: " + Buffer( pat->getNetworkPID(), Buffer::Base16, 4 ) );
						msgWrn( "New PID: " + Buffer( progPID, Buffer::Base16, 4 ) );
					}
				}
				else {
//					msgDbg( "Found network_id ["
//						+ Buffer( progPID, Buffer::Base16, 4 )
//						+ "]" );
					pat->setNetworkPID( progPID );
				}
			}

		}

	}

	// Dump PAT...

	getPeerConsumer()->putObject( pat );

}

// ============================================================================

