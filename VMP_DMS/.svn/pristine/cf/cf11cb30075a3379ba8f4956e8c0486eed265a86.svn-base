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
//	VMP.AVC.SequenceParameterSet.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_SequenceParameterSet_H_
#define _VMP_AVC_SequenceParameterSet_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.SequenceParameterSetPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "VMP.AVC.BitstreamPtr.h"
#include "VMP.AVC.NALUPtr.h"
#include "VMP.AVC.VUIParametersPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL SequenceParameterSet : virtual public BFC::DObject {

public :

	/// \brief Creates a new SequenceParameterSet.

	SequenceParameterSet(
	);

	void decode(
			NALUCPtr	pNALU
	);

	BFC::Buffer toBuffer(
	) const;

	static const BFC::Uint32 MAXnum_ref_frames_in_pic_order_cnt_cycle	= 256;

	BFC::Uint32		profile_idc;
	BFC::Uint32		constrained_set0_flag;
	BFC::Uint32		constrained_set1_flag;
	BFC::Uint32		constrained_set2_flag;
	BFC::Uint32		constrained_set3_flag;
	BFC::Uint32		constrained_set4_flag;
	BFC::Uint32		constrained_set5_flag;
	BFC::Uint32		level_idc;
	BFC::Uint32		seq_parameter_set_id;

//	if ( profile_idc = = 100 | | profile_idc = = 110 | |
//	  profile_idc = = 122 | | profile_idc = = 244 | | profile_idc = = 44 | |
//	  profile_idc = = 83 | | profile_idc = = 86 | | profile_idc = = 118 | |
//	  profile_idc = = 128 ) {

	BFC::Uint32		chroma_format_idc;
	BFC::Uint32		separate_colour_plane_flag;
	BFC::Uint32		bit_depth_luma_minus8;
	BFC::Uint32		bit_depth_chroma_minus8;
	BFC::Uint32		lossless_qpprime_flag;
	BFC::Uint32		seq_scaling_matrix_present_flag;
	BFC::Uint32		seq_scaling_list_present_flag[ 12 ];
	BFC::Int32		ScalingList4x4[ 6 ][ 16 ];
	BFC::Int32		ScalingList8x8[ 6 ][ 64 ];
	BFC::Uint32		UseDefaultScalingMatrix4x4Flag[ 6 ];
	BFC::Uint32		UseDefaultScalingMatrix8x8Flag[ 6 ];

//	}

	BFC::Uint32		log2_max_frame_num_minus4;
	BFC::Uint32		pic_order_cnt_type;
//	if ( pic_order_cnt_type == 0 ) {
	BFC::Uint32		log2_max_pic_order_cnt_lsb_minus4;
//	}
//	else if ( pic_order_cnt_type == 1 ) {
	BFC::Uint32		delta_pic_order_always_zero_flag;
	BFC::Int32		offset_for_non_ref_pic;
	BFC::Int32		offset_for_top_to_bottom_field;
	BFC::Uint32		num_ref_frames_in_pic_order_cnt_cycle;
//	for ( i = 0 ; i < num_ref_frames_in_pic_order_cnt_cycle ; i++ ) {
	BFC::Int32		offset_for_ref_frame[ MAXnum_ref_frames_in_pic_order_cnt_cycle ];
//	}
//	}
	BFC::Uint32		num_ref_frames;
	BFC::Uint32		gaps_in_frame_num_value_allowed_flag;
	BFC::Uint32		pic_width_in_mbs_minus1;
	BFC::Uint32		pic_height_in_map_units_minus1;
	BFC::Uint32		frame_mbs_only_flag;
//	if ( ! frame_mbs_only_flag ) {
	BFC::Uint32		mb_adaptive_frame_field_flag;
//	}
	BFC::Uint32		direct_8x8_inference_flag;
	BFC::Uint32		frame_cropping_flag;
//	if ( frame_cropping_flag ) {
	BFC::Uint32		frame_crop_left_offset;
	BFC::Uint32		frame_crop_right_offset;
	BFC::Uint32		frame_crop_top_offset;
	BFC::Uint32		frame_crop_bottom_offset;
//	}
	BFC::Uint32		vui_parameters_present_flag;
	VUIParametersPtr	vui_seq_parameters;

	BFC::Uint32 getPictWidth(
	) const {
		return pictWdth;
	}

	BFC::Uint32 getPictHeight(
	) const {
		return pictHght;
	}

protected :

	void Scaling_List(
			BFC::Int32 *	scalingList,
			BFC::Uint32	sizeOfScalingList,
			BFC::Uint32 &	UseDefaultScalingMatrix,
			BitstreamPtr	s
	);

private :

	static const BFC::Uchar ZZ_SCAN[ 16 ];
	static const BFC::Uchar ZZ_SCAN8[ 64 ];

	BFC::Uint32	pictWdth;
	BFC::Uint32	pictHght;

	/// \brief Forbidden copy constructor.

	SequenceParameterSet(
		const	SequenceParameterSet &
	);

	/// \brief Forbidden copy operator.

	SequenceParameterSet & operator = (
		const	SequenceParameterSet &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_SequenceParameterSet_H_

// ============================================================================

