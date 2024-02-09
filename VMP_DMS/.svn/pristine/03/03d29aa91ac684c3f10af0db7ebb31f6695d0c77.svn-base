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
//	VMP.M2S.TSSectionFramer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.TSPacket.h"
#include "VMP.M2S.TSSection.h"
#include "VMP.M2S.TSSectionFramer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSSectionFramer )

// ============================================================================

M2S::TSSectionFramer::TSSectionFramer() {

	setObjectName( "VMP.M2S.TSSectionFramer" );

}

// ============================================================================

void M2S::TSSectionFramer::setPeerConsumer(
	const	Uint32		pTableId,
		GPC::ConsumerPtr
				pConsumer ) {

	if ( pTableId >= 0x100 ) {
		throw InvalidArgument();
	}

	consumer[ pTableId ] = pConsumer;

}

void M2S::TSSectionFramer::delPeerConsumer(
	const	Uint32		pTableId ) {

	if ( pTableId >= 0x100 ) {
		throw InvalidArgument();
	}

	consumer[ pTableId ].kill();

}

void M2S::TSSectionFramer::detachAllConsumers() {

	for ( Uint32 i = 0 ; i < 0x100 ; i++ ) {
		consumer[ i ].kill();
	}

}

// ============================================================================

void M2S::TSSectionFramer::putObject(
		SPtr		pTSPacket ) {

	TSPacketPtr	packet = pTSPacket;

//	msgDbg( "putObject( TSPacket )" );

	if ( ! packet->hasPayload() ) {
		msgWrn( "putObject(): No payload to consume!" );
		return;
	}

	Buffer buf = packet->getPayload();

	if ( buf.isEmpty() ) {
		throw InvalidArgument( "Empty payload!" );
	}

	const Uchar *	ptr = buf.getCstPtr();
	Uint32		len = buf.getLength();

	if ( packet->getPayloadUnitStartIndicator() ) {
		Uint32 pointer_field = *ptr++;
		len--;
//		msgDbg( "putObject(): pointer_field == "
//			+ Buffer( pointer_field ) );
		if ( pointer_field >= len ) {
			msgWrn( "putObject(): pointer_field overflow!" );
			return;
		}
		if ( pointer_field ) {
			if ( curr ) {
				// Payload == end of previous + begin of new!
				Uint32 tmp = pointer_field; // end block length
				curr->decode( ptr, tmp );
				if ( tmp ) {
					msgWrn( "putObject(): Junk between 2 sections!" );
					ptr += tmp;
				}
				if ( ! curr->isReady() ) {
					msgWrn( "putObject(): Underflow while finishing section!" );
				}
				else {
					putSection( curr );
				}
				curr.kill();
			}
			else {
				msgWrn( "putObject(): Junk before start of section!" );
				ptr += pointer_field;
				len -= pointer_field;
			}
		}
		while ( len ) {
//			msgDbg( "decoding fresh section" );
			curr = new TSSection;
//			msgDbg( "decoding fresh section: before, len == "
//				+ Buffer( len ) );
			curr->decode( ptr, len );
//			msgDbg( "decoding fresh section: after, len == "
//				+ Buffer( len ) );
			if ( curr->isReady() ) {
				putSection( curr );
				curr.kill();
			}
			else if ( len ) {
				msgWrn( "putObject(): Truncated section!" );
			}
		}
	}
	else {
//		msgDbg( "putObject(): ! pointer_field, with len == "
//			+ Buffer( len ) );
		// Continuation of a previous section...
		if ( ! curr ) {
			msgWrn( "putObject(): Dropping partial section!" );
			return;
		}
		curr->decode( ptr, len );
		if ( curr->isReady() ) {
			putSection( curr );
			curr.kill();
		}
		while ( len-- ) {
			// We didn't consume everything! The rest must be
			// stuffing bytes!
			if ( *ptr++ != 0xFF ) {
				msgWrn( "putObject(): Invalid stuffing after partial section, with no PUSI!" );
				break;
			}
		}
	}

}

// ============================================================================

void M2S::TSSectionFramer::putSection(
		TSSectionPtr	pTSSction ) {

	if ( ! consumer[ pTSSction->table_id ] ) {
		msgWrn( "putSection(): No consumer for table_id["
			+ Buffer( pTSSction->table_id,
				Buffer::Base16, 2 )
			+ "]" );
	}
	else {
		consumer[ pTSSction->table_id ]->putObject( pTSSction );
	}

}

// ============================================================================

