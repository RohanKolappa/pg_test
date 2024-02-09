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
//	VMP.M2S.TSTable.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.M2S.TSSection.h"
#include "VMP.M2S.TSTable.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSTable )

// ============================================================================

M2S::TSTable::TSTable(
		TSSectionPtr	pSection ) {

	setObjectName( "VMP.M2S.TSTable" );

	table_id = pSection->table_id;
	syntax_indicator = pSection->section_syntax_indicator;
	nbr_sections = 1;
	if ( syntax_indicator ) {
		version_number = pSection->version_number;
		last_section_number = pSection->last_section_number;
		sections.resize( last_section_number + 1 );
		sections[ pSection->section_number ] = pSection;
	}
	else {
		version_number = (Uint32)-1;
		last_section_number = (Uint32)-1;
		sections.resize( 1 );
		sections[ 0 ] = pSection;
	}

}

// ============================================================================

Bool M2S::TSTable::consume(
		TSSectionPtr	pSection ) {

	if ( syntax_indicator != pSection->section_syntax_indicator ) {
		msgWrn( "Mutating section_syntax_indicator!" );
		return false;
	}

	if ( ! syntax_indicator ) {
		sections[ 0 ] = pSection;
		return true;
	}

	if ( table_id != pSection->table_id ) {
		msgWrn( "Received invalid table_id!!!" );
		msgWrn( "Table  : " + Buffer( table_id ) );
		msgWrn( "Section: " + Buffer( pSection->table_id ) );
		return false;
	}

	if ( last_section_number != pSection->last_section_number ) {
		msgWrn( "Incoherent last_section_number!" );
		msgWrn( "table_id: " + Buffer( table_id, Buffer::Base16, 2 ) );
		msgWrn( "Table  : " + Buffer( last_section_number ) );
		msgWrn( "Section: " + Buffer( pSection->last_section_number ) );
		return false;
	}

	if ( sections[ pSection->section_number ] ) {
		// Already got this section!
		return false;
	}

	sections[ pSection->section_number ] = pSection;
	nbr_sections++;

	return true;

}

// ============================================================================

