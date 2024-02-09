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
//	VMP.M2S.TSCATFramer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.M2S.DescriptorDecoder.h"
#include "VMP.M2S.TSCAT.h"
#include "VMP.M2S.TSCATFramer.h"
#include "VMP.M2S.TSSection.h"
#include "VMP.M2S.TSTable.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSCATFramer )

// ============================================================================

M2S::TSCATFramer::TSCATFramer(
		DescriptorDecoderPtr
				pDescrDecoder ) {

	setObjectName( "VMP.M2S.TSCATFramer" );

	descrDecoder = pDescrDecoder;

}

// ============================================================================

void M2S::TSCATFramer::putObject(
		SPtr		pTSTable ) {

	TSTablePtr	table = pTSTable;

//	msgDbg( "Got table!" );

	// Sanity checks...

	if ( table->getTableId() != 0x01 ) {
		msgWrn( "Invalid table_id["
			+ Buffer( table->getTableId() ) + "]" );
		return;
	}

	if ( ! table->getSyntaxIndicator() ) {
		msgWrn( "CAT doesn't use section syntax ?! Huh..." );
		return;
	}

//	msgDbg( "Got CAT! (presumably)" );
//	msgDbg( "... version: " + Buffer( table->getVersion() ) );

	Uint32		nbrSections = table->getNbrSections();

	TSCATPtr cat = new TSCAT;

	cat->version = table->getVersion();

	for ( Uint32 i = 0 ; i < nbrSections ; i++ ) {

//		msgDbg( "Scanning section["
//			+ Buffer( i + 1 ) + "/"
//			+ Buffer( nbrSections ) + "]..." );

		Buffer	payload = table->getSection( i )->getPayload();

//		msgDbg( "payload: " + Utils::toHex( payload ) );

		cat->descriptors += descrDecoder->decode( payload );

	}

	// Dump CAT...

	getPeerConsumer()->putObject( cat );

}

// ============================================================================

