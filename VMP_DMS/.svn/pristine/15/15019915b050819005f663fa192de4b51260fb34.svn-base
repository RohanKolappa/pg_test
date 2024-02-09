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
//	VMP.AVC.SliceHeader.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.AVC.Bitstream.h"
#include "VMP.AVC.Defines.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.SliceHeader.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, SliceHeader )

// ============================================================================

AVC::SliceHeader::SliceHeader() {

	setObjectName( "VMP.AVC.SliceHeader" );

}

// ============================================================================

void AVC::SliceHeader::decode(
		NALUCPtr	pNALU,
	const	Bool		separate_colour_plane_flag,
	const	Uint32		log2_max_frame_num,
	const	Bool		frame_mbs_only_flag,
	const	Uint32		pic_order_cnt_type,
	const	Uint32		log2_max_pic_order_cnt_lsb ) {

	colour_plane_id		= 0;
	field_pic_flag		= 0;
	bottom_field_flag	= 0;
	idr_pic_id		= 0;
	pic_order_cnt_lsb	= 0;

	Buffer		d = pNALU->getRBSP();

//	msgDbg( "decode(): " + Utils::toHex( d ) );

	BitstreamPtr	s = new Bitstream( d );

//	first_mb_in_slice								ue(v)

	first_mb_in_slice		= s->read_ue_v();

//	slice_type									ue(v)

	slice_type			= s->read_ue_v();

//	pic_parameter_set_id								ue(v)

	pic_parameter_set_id		= s->read_ue_v();

//	if ( separate_colour_plane_flag == 1 )
//		colour_plane_id								u(2)

	if ( separate_colour_plane_flag ) {
		colour_plane_id			= s->read_u_v( 2 );
	}

//	frame_num									u(v)

	frame_num			= s->read_u_v( log2_max_frame_num );

//	if ( ! frame_mbs_only_flag ) {
//		field_pic_flag								u(1)
//		if ( field_pic_flag )
//			bottom_field_flag						u(1)
//	}

	if ( ! frame_mbs_only_flag ) {
		field_pic_flag			= s->read_u_1();
		if ( field_pic_flag ) {
			bottom_field_flag		= s->read_u_1();
		}
	}

//	if ( IdrPicFlag )
//		idr_pic_id								ue(v)

	if ( pNALU->getType() == NALU::TypeIDR ) {
		idr_pic_id			= s->read_ue_v();
	}

//	if ( pic_order_cnt_type = = 0 ) {
//		pic_order_cnt_lsb							u(v)
//		if( bottom_field_pic_order_in_frame_present_flag && !field_pic_flag )
//			delta_pic_order_cnt_bottom					se(v)
//	}

	if ( pic_order_cnt_type == 0 ) {
		pic_order_cnt_lsb		= s->read_u_v( log2_max_pic_order_cnt_lsb );
	}

//	if( pic_order_cnt_type = = 1 && !delta_pic_order_always_zero_flag ) {
//		delta_pic_order_cnt[ 0 ]						se(v)
//		if( bottom_field_pic_order_in_frame_present_flag && !field_pic_flag )
//			delta_pic_order_cnt[ 1 ]					se(v)
//	}
//	if( redundant_pic_cnt_present_flag )
//		redundant_pic_cnt							ue(v)
//	if( slice_type = = B )
//		direct_spatial_mv_pred_flag						u(1)
//	if( slice_type = = P | | slice_type = = SP | | slice_type = = B ) {
//		num_ref_idx_active_override_flag					u(1)
//		if( num_ref_idx_active_override_flag ) {
//			num_ref_idx_l0_active_minus1					ue(v)
//			if( slice_type = = B )
//				num_ref_idx_l1_active_minus1				ue(v)
//		}
//	}
//	if( nal_unit_type = = 20 | | nal_unit_type = = 21 )
//		ref_pic_list_mvc_modification( ) /* specified in Annex H */
//	else
//		ref_pic_list_modification( )
//	if ( ( weighted_pred_flag && ( slice_type = = P | | slice_type = = SP ) )
//	  || ( weighted_bipred_idc = = 1 && slice_type = = B ) )
//		pred_weight_table( )
//	if( nal_ref_idc != 0 )
//		dec_ref_pic_marking( )
//	if( entropy_coding_mode_flag && slice_type != I && slice_type != SI )
//		cabac_init_idc								ue(v)
//	slice_qp_delta									se(v)
//	if( slice_type = = SP | | slice_type = = SI ) {
//		if( slice_type = = SP )
//			sp_for_switch_flag						u(1)
//		slice_qs_delta								se(v)
//	}
//	if( deblocking_filter_control_present_flag ) {
//		disable_deblocking_filter_idc						ue(v)
//		if( disable_deblocking_filter_idc != 1 ) {
//			slice_alpha_c0_offset_div2					se(v)
//			slice_beta_offset_div2						se(v)
//		}
//	}
//	if ( num_slice_groups_minus1 > 0
//	  && slice_group_map_type >= 3 && slice_group_map_type <= 5)
//		slice_group_change_cycle						u(v)

}

// ============================================================================

Buffer AVC::SliceHeader::toBuffer() const {

	return "first_mb_in_slice: " + Buffer( first_mb_in_slice )
		+ ", slice_type: " + Buffer( slice_type )
		+ ", pic_parameter_set_id: " + Buffer( pic_parameter_set_id )
		+ ", colour_plane_id: " + Buffer( colour_plane_id )
		+ ", frame_num: " + Buffer( frame_num )
		+ ", field_pic_flag: " + Buffer( field_pic_flag )
		+ ", bottom_field_flag: " + Buffer( bottom_field_flag )
		+ ", idr_pic_id: " + Buffer( idr_pic_id )
		+ ", pic_order_cnt_lsb: " + Buffer( pic_order_cnt_lsb );

}

// ============================================================================

