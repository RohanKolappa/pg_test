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
//	VMP.AVC.PictureParameterSet.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_PictureParameterSet_H_
#define _VMP_AVC_PictureParameterSet_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.PictureParameterSetPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "VMP.AVC.NALUPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL PictureParameterSet : virtual public BFC::DObject {

public :

	/// \brief Creates a new PictureParameterSet.

	PictureParameterSet(
	);

	void decode(
			NALUCPtr	pNALU
	);

	BFC::Buffer toBuffer(
	) const;

	static const BFC::Uint32 MAXnum_slice_groups_minus1	= 8;

	BFC::Uint32	pic_parameter_set_id;					// ue(v)
	BFC::Uint32	seq_parameter_set_id;					// ue(v)
	BFC::Bool	entropy_coding_mode_flag;				// u(1)
	BFC::Bool	bottom_field_pic_order_in_frame_present_flag;		// u(1)
	BFC::Uint32	num_slice_groups_minus1;				// ue(v)
//	if ( num_slice_groups_minus1 > 0 ) {
	BFC::Uint32	slice_group_map_type;					// ue(v)
//	if ( slice_group_map_type = = 0 ) {
//		for ( i = 0 ; i <= num_slice_groups_minus1 ; i++ )
	BFC::Uint32	run_length_minus1[ MAXnum_slice_groups_minus1 ];	// ue(v)
//	}
//	else if ( slice_group_map_type == 2 ) {
//		for ( i = 0 ; i < num_slice_groups_minus1 ; i++ ) {
	BFC::Uint32	top_left[ MAXnum_slice_groups_minus1 ];			// ue(v)
	BFC::Uint32	bottom_right[ MAXnum_slice_groups_minus1 ];		// ue(v)
//		}
//	}
//	else if ( slice_group_map_type == 3
//	       || slice_group_map_type == 4
//	       || slice_group_map_type == 5 ) {
	BFC::Bool	slice_group_change_direction_flag;			// u(1)
	BFC::Uint32	slice_group_change_rate_minus1;				// ue(v)
//	}
//	else if ( slice_group_map_type == 6 ) {
	BFC::Uint32	pic_size_in_map_units_minus1;				// ue(v)
//		for ( i = 0 ; i <= pic_size_in_map_units_minus1 ; i++ )
	BFC::Uchar *	slice_group_id;						// complete MBAmap u(v)
//	}
//	}

	BFC::Uint32	num_ref_idx_l0_default_active_minus1;			// ue(v)
	BFC::Uint32	num_ref_idx_l1_default_active_minus1;			// ue(v)
	BFC::Bool	weighted_pred_flag;					// u(1)
	BFC::Uint32	weighted_bipred_idc;					// u(2)
	BFC::Int32	pic_init_qp_minus26;					// se(v)
	BFC::Int32	pic_init_qs_minus26;					// se(v)
	BFC::Int32	chroma_qp_index_offset;					// se(v)

	BFC::Bool	deblocking_filter_control_present_flag;			// u(1)
	BFC::Bool	constrained_intra_pred_flag;				// u(1)
	BFC::Bool	redundant_pic_cnt_present_flag;				// u(1)

//	if ( more_rbsp_data() ) {
	BFC::Bool	transform_8x8_mode_flag;				// u(1)
	BFC::Bool	pic_scaling_matrix_present_flag;			// u(1)
//	}

	BFC::Bool	pic_scaling_list_present_flag[ 12 ];			// u(1)
	BFC::Int32	ScalingList4x4[ 6 ][ 16 ];				// se(v)
	BFC::Int32	ScalingList8x8[ 6 ][ 64 ];				// se(v)
	BFC::Bool	UseDefaultScalingMatrix4x4Flag[ 6 ];
	BFC::Bool	UseDefaultScalingMatrix8x8Flag[ 6 ];

	BFC::Int32	cb_qp_index_offset;					// se(v)
	BFC::Int32	cr_qp_index_offset;					// se(v)
	BFC::Int32	second_chroma_qp_index_offset;				// se(v)
	BFC::Bool	vui_pic_parameters_flag;				// u(1)

protected :

private :

	/// \brief Forbidden copy constructor.

	PictureParameterSet(
		const	PictureParameterSet &
	);

	/// \brief Forbidden copy operator.

	PictureParameterSet & operator = (
		const	PictureParameterSet &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_PictureParameterSet_H_

// ============================================================================

