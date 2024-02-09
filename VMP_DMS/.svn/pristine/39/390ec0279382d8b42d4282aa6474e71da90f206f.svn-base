// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC".
//
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	VMP.AVC.HRDParameters.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.AVC.Bitstream.h"
#include "VMP.AVC.HRDParameters.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, HRDParameters )

// ============================================================================

AVC::HRDParameters::HRDParameters() {

	setObjectName( "VMP.AVC.HRDParameters" );

}

// ============================================================================

void AVC::HRDParameters::decode(
		BitstreamPtr	s ) {

	cpb_cnt_minus1				= s->read_ue_v();
	bit_rate_scale				= s->read_u_v( 4 );
	cpb_size_scale				= s->read_u_v( 4 );

	for ( Uint32 i = 0 ; i <= cpb_cnt_minus1 ; i++ ) {
		bit_rate_value_minus1[ i ]	= s->read_ue_v();
		cpb_size_value_minus1[ i ]	= s->read_ue_v();
		cbr_flag[ i ]			= s->read_u_1();
	}

	initial_cpb_removal_delay_length_minus1	= s->read_u_v( 5 );
	cpb_removal_delay_length_minus1		= s->read_u_v( 5 );
	dpb_output_delay_length_minus1		= s->read_u_v( 5 );
	time_offset_length			= s->read_u_v( 5 );

}

// ============================================================================

Buffer AVC::HRDParameters::toBuffer() const {

	Buffer res;

	res += "HRD: cpb_cnt_minus1                          : " + Buffer( cpb_cnt_minus1 );
	res += "\nHRD: bit_rate_scale                          : " + Buffer( bit_rate_scale );
	res += "\nHRD: cpb_size_scale                          : " + Buffer( cpb_size_scale );

	for ( Uint32 i = 0 ; i <= cpb_cnt_minus1 ; i++ ) {
		res += "\nHRD: bit_rate_value_minus1[ i ]              : " + Buffer( bit_rate_value_minus1[ i ] );
		res += "\nHRD: cpb_size_value_minus1[ i ]              : " + Buffer( cpb_size_value_minus1[ i ] );
		res += "\nHRD: cbr_flag[ i ]                           : " + Buffer( cbr_flag[ i ] );
	}

	res += "\nHRD: initial_cpb_removal_delay_length_minus1 : " + Buffer( initial_cpb_removal_delay_length_minus1 );
	res += "\nHRD: cpb_removal_delay_length_minus1         : " + Buffer( cpb_removal_delay_length_minus1 );
	res += "\nHRD: dpb_output_delay_length_minus1          : " + Buffer( dpb_output_delay_length_minus1 );
	res += "\nHRD: time_offset_length                      : " + Buffer( time_offset_length );

	return res;

}

// ============================================================================

