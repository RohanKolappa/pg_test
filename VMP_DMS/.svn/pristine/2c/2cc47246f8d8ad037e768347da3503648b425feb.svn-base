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
//	VMP.M2S.TSTableFramer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.TSSection.h"
#include "VMP.M2S.TSTable.h"
#include "VMP.M2S.TSTableFramer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSTableFramer )

// ============================================================================

M2S::TSTableFramer::TSTableFramer(
	const	Bool	useTableIdExt ) :

	useExt( useTableIdExt ) {

	setObjectName( "VMP.M2S.TSTableFramer" );

}

// ============================================================================

void M2S::TSTableFramer::putObject(
		SPtr		pTSSection ) {

	TSSectionPtr	pSection = pTSSection;

	if ( useExt && ! pSection->section_syntax_indicator ) {
		throw InternalError( "No section_syntax_indicator!" );
	}

//	msgDbg( "Got section!" );

	if ( ! pSection->current_next_indicator ) {
//		msgWrn( "Found ! current section... Skipping..." );
		return;
	}

	if ( useExt ) {

		Uint32 ext = pSection->table_id_ext;
		Uint32 idx;

		for ( idx = 0 ; idx < entries.getSize() ; idx++ ) {
			if ( entries[ idx ].id_ext == ext ) {
				break;
			}
		}

		if ( idx >= entries.getSize() ) {
			entries.resize( idx + 1 );
			entries[ idx ].id_ext = ext;
		}

		// tbl = &entries[ idx ].table;

		if ( entries[ idx ].table
		  && entries[ idx ].table->getVersion() != pSection->version_number ) {
			entries[ idx ].table.kill();
		}

		if ( ! entries[ idx ].table ) {
			entries[ idx ].table = new TSTable( pSection );
			if ( ! entries[ idx ].table->isReady() ) {
				return;
			}
		}
		else if ( ! entries[ idx ].table->consume( pSection )
		       || ! entries[ idx ].table->isReady() ) {
			return;
		}

		getPeerConsumer()->putObject( entries[ idx ].table );

	}
	else {

		if ( table
		  && table->getVersion() != pSection->version_number ) {
			table.kill();
		}

		if ( ! table ) {
			table = new TSTable( pSection );
			if ( ! table->isReady() ) {
				return;
			}
		}
		else if ( ! table->consume( pSection )
		       || ! table->isReady() ) {
			return;
		}

		getPeerConsumer()->putObject( table );

	}

}

// ============================================================================

