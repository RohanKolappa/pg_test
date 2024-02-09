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
//	VMP.AVC.PictureParameterSet.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.AVC.Bitstream.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.PictureParameterSet.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, PictureParameterSet )

// ============================================================================

AVC::PictureParameterSet::PictureParameterSet() {

	setObjectName( "VMP.AVC.PictureParameterSet" );

}

// ============================================================================

void AVC::PictureParameterSet::decode(
		NALUCPtr	pNALU ) {

	Buffer		d = pNALU->getRBSP();

//	msgDbg( "decode(): " + Utils::toHex( d ) );

	BitstreamPtr	s = new Bitstream( d );

	pic_parameter_set_id				= s->read_ue_v();
	seq_parameter_set_id				= s->read_ue_v();
	entropy_coding_mode_flag			= s->read_u_1();
	bottom_field_pic_order_in_frame_present_flag	= s->read_u_1();
	num_slice_groups_minus1				= s->read_ue_v();

	if ( num_slice_groups_minus1 > 0 ) {
		slice_group_map_type				= s->read_ue_v();
		if ( slice_group_map_type == 0 ) {
			for ( Uint32 i = 0 ; i <= num_slice_groups_minus1 ; i++ ) {
				run_length_minus1[ i ]				= s->read_ue_v();
			}
		}
		else if ( slice_group_map_type == 2 ) {
			for ( Uint32 i = 0 ; i < num_slice_groups_minus1 ; i++ ) {
				top_left[ i ]					= s->read_ue_v();
				bottom_right[ i ]				= s->read_ue_v();
			}
		}
		else if ( slice_group_map_type == 3
		       || slice_group_map_type == 4
		       || slice_group_map_type == 5 ) {
			slice_group_change_direction_flag		= s->read_u_1();
			slice_group_change_rate_minus1			= s->read_ue_v();
		}
		else if ( slice_group_map_type == 6 ) {
			pic_size_in_map_units_minus1			= s->read_ue_v();
			return;
			for ( Uint32 i = 0 ; i <= pic_size_in_map_units_minus1 ; i++ ) {
// FIXME: decoding stops here!
//				slice_group_id;						// complete MBAmap u(v)
			}
		}
	}

	num_ref_idx_l0_default_active_minus1		= s->read_ue_v();
	num_ref_idx_l1_default_active_minus1		= s->read_ue_v();
	weighted_pred_flag				= s->read_u_1();
	weighted_bipred_idc				= s->read_u_v( 2 );
	pic_init_qp_minus26				= s->read_se_v();
	pic_init_qs_minus26				= s->read_se_v();
	chroma_qp_index_offset				= s->read_se_v();

	deblocking_filter_control_present_flag		= s->read_u_1();
	constrained_intra_pred_flag			= s->read_u_1();
	redundant_pic_cnt_present_flag			= s->read_u_1();

//	msgDbg( "decode(): read " + Buffer( s->tell() ) + " / "
//		+ Buffer( d.getLength() * 8 ) + " bits!" );
//	if ( ( s->tell() + 7 ) / 8 != d.getLength() ) {
//		msgWrn( "decode(): junk at end! (bitOff: " + Buffer( s->tell() )
//			+ ", bitLen: " + Buffer( d.getLength() * 8 ) + ")" );
//	}

}

// ============================================================================

Buffer AVC::PictureParameterSet::toBuffer() const {

	Buffer res;

	res += "PPS: pic_parameter_set_id                         : " + Buffer( pic_parameter_set_id );
	res += "\nPPS: seq_parameter_set_id                         : " + Buffer( seq_parameter_set_id );
	res += "\nPPS: entropy_coding_mode_flag                     : " + Buffer( entropy_coding_mode_flag );
	res += "\nPPS: bottom_field_pic_order_in_frame_present_flag : " + Buffer( bottom_field_pic_order_in_frame_present_flag );
	res += "\nPPS: num_slice_groups_minus1                      : " + Buffer( num_slice_groups_minus1 );
	res += "\nPPS: num_ref_idx_l0_default_active_minus1         : " + Buffer( num_ref_idx_l0_default_active_minus1 );
	res += "\nPPS: num_ref_idx_l1_default_active_minus1         : " + Buffer( num_ref_idx_l1_default_active_minus1 );
	res += "\nPPS: weighted_pred_flag                           : " + Buffer( weighted_pred_flag );
	res += "\nPPS: weighted_bipred_idc                          : " + Buffer( weighted_bipred_idc );
	res += "\nPPS: pic_init_qp_minus26                          : " + Buffer( pic_init_qp_minus26 );
	res += "\nPPS: pic_init_qs_minus26                          : " + Buffer( pic_init_qs_minus26 );
	res += "\nPPS: chroma_qp_index_offset                       : " + Buffer( chroma_qp_index_offset );
	res += "\nPPS: deblocking_filter_control_present_flag       : " + Buffer( deblocking_filter_control_present_flag );
	res += "\nPPS: constrained_intra_pred_flag                  : " + Buffer( constrained_intra_pred_flag );
	res += "\nPPS: redundant_pic_cnt_present_flag               : " + Buffer( redundant_pic_cnt_present_flag );

	return res;

}

// ============================================================================

