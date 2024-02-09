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
//	VMP.AVC.SequenceParameterSet.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.AVC.Bitstream.h"
#include "VMP.AVC.Defines.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.SequenceParameterSet.h"
#include "VMP.AVC.VUIParameters.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, SequenceParameterSet )

// ============================================================================

AVC::SequenceParameterSet::SequenceParameterSet() {

	setObjectName( "VMP.AVC.SequenceParameterSet" );

}

// ============================================================================

void AVC::SequenceParameterSet::decode(
		NALUCPtr	pNALU ) {

	Buffer		d = pNALU->getRBSP();

//	msgDbg( "decode(): " + Utils::toHex( d ) );

	BitstreamPtr	s = new Bitstream( d );

	profile_idc			= s->read_u_v( 8 );

	if ( profile_idc != BASELINE
	  && profile_idc != MAIN
	  && profile_idc != EXTENDED
	  && profile_idc != FREXT_HP
	  && profile_idc != FREXT_Hi10P
	  && profile_idc != FREXT_Hi422
	  && profile_idc != FREXT_Hi444
	  && profile_idc != FREXT_CAVLC444
	  && profile_idc != MVC_HIGH
	  && profile_idc != STEREO_HIGH ) {
		throw InternalError( "Invalid Profile IDC (" + Buffer( profile_idc ) + ") encountered." );
	}

	constrained_set0_flag		= s->read_u_1();
	constrained_set1_flag		= s->read_u_1();
	constrained_set2_flag		= s->read_u_1();
	constrained_set3_flag		= s->read_u_1();
	constrained_set4_flag		= s->read_u_1();
	constrained_set5_flag		= s->read_u_1();

	Uint32	reserved_zero = s->read_u_v( 2 );

	if ( reserved_zero != 0 ) {
		msgWrn( "'reserved_zero' flag not equal to 0. Possibly new constrained_setX flag introduced.");
	}

	level_idc			= s->read_u_v( 8 );
	seq_parameter_set_id		= s->read_ue_v();

	chroma_format_idc		= 1;
	separate_colour_plane_flag	= 0;
	bit_depth_luma_minus8		= 0;
	bit_depth_chroma_minus8		= 0;
	lossless_qpprime_flag		= 0;

	if ( profile_idc == FREXT_HP		// 100
	  || profile_idc == FREXT_Hi10P		// 110
	  || profile_idc == FREXT_Hi422		// 122
	  || profile_idc == FREXT_Hi444		// 244
	  || profile_idc == FREXT_CAVLC444	// 44
	  || profile_idc == SCALABLE_BASELINE	// 83
	  || profile_idc == SCALABLE_HIGH	// 86
	  || profile_idc == MVC_HIGH		// 118
	  || profile_idc == STEREO_HIGH ) {	// 128

		chroma_format_idc		= s->read_ue_v();
		if ( chroma_format_idc == YUV444 ) {	// 3
			separate_colour_plane_flag	= s->read_u_1();
		}
		bit_depth_luma_minus8		= s->read_ue_v();
		bit_depth_chroma_minus8		= s->read_ue_v();
		lossless_qpprime_flag		= s->read_u_1();
		seq_scaling_matrix_present_flag	= s->read_u_1();

		if ( seq_scaling_matrix_present_flag ) {
			Uint32 n_ScalingList = ( chroma_format_idc != YUV444 ? 8 : 12 );
			for ( Uint32 i = 0 ; i < n_ScalingList ; i++ ) {
				seq_scaling_list_present_flag[ i ]	= s->read_u_1();
				if ( seq_scaling_list_present_flag[ i ] ) {
					if ( i < 6 ) {
						Scaling_List(
							ScalingList4x4[ i ],
							16,
							UseDefaultScalingMatrix4x4Flag[ i ],
							s );
					}
					else {
						Scaling_List(
							ScalingList8x8[ i - 6 ],
							64,
							UseDefaultScalingMatrix8x8Flag[ i - 6 ],
							s );
					}
				}
			}
		}
	}

	log2_max_frame_num_minus4	= s->read_ue_v();
	pic_order_cnt_type		= s->read_ue_v();

	if ( pic_order_cnt_type == 0 ) {
		log2_max_pic_order_cnt_lsb_minus4	= s->read_ue_v();
	}
	else if ( pic_order_cnt_type == 1 ) {
		delta_pic_order_always_zero_flag	= s->read_u_1();
		offset_for_non_ref_pic			= s->read_se_v();
		offset_for_top_to_bottom_field		= s->read_se_v();
		num_ref_frames_in_pic_order_cnt_cycle	= s->read_ue_v();
		for ( Uint32 i = 0 ; i < num_ref_frames_in_pic_order_cnt_cycle ; i++ ) {
			offset_for_ref_frame[ i ]		= s->read_se_v();
		}
	}

	num_ref_frames			= s->read_ue_v();
	gaps_in_frame_num_value_allowed_flag
					= s->read_u_1();
	pic_width_in_mbs_minus1		= s->read_ue_v();
	pic_height_in_map_units_minus1	= s->read_ue_v();
	frame_mbs_only_flag		= s->read_u_1();
	if ( ! frame_mbs_only_flag ) {
		mb_adaptive_frame_field_flag	= s->read_u_1();
	}
	direct_8x8_inference_flag	= s->read_u_1();
	frame_cropping_flag		= s->read_u_1();
	if ( frame_cropping_flag ) {
		frame_crop_left_offset		= s->read_ue_v();
		frame_crop_right_offset		= s->read_ue_v();
		frame_crop_top_offset		= s->read_ue_v();
		frame_crop_bottom_offset	= s->read_ue_v();
	}
	vui_parameters_present_flag	= s->read_u_1();
	if ( vui_parameters_present_flag ) {
		vui_seq_parameters = new VUIParameters;
		vui_seq_parameters->decode( s );
	}

	if ( ( s->tell() + 7 ) / 8 != d.getLength() ) {
		msgWrn( "decode(): junk at end! (bitOff: " + Buffer( s->tell() )
			+ ", bitLen: " + Buffer( d.getLength() * 8 ) + ")" );
//		Uint32 l = d.getLength() * 8 - s->tell();
//		for ( Uint32 i = 0 ; i < l ; i++ ) {
//			msgWrn( "decode(): " + Buffer( s->read_u_1() ) );
//		}
	}

	// Compute pictWdth & pictHght...

	pictWdth = ( pic_width_in_mbs_minus1 + 1 ) * 16;
	pictHght = ( pic_height_in_map_units_minus1 + 1 ) * 16;

	if ( frame_cropping_flag ) {
		Uint32	SubWidthC	= 0;	// See Table 6-1.
		Uint32	SubHeightC	= 0;	// See Table 6-1.
		if ( chroma_format_idc == 1 ) {
			SubWidthC	= 2;
			SubHeightC	= 2;
		}
		else if ( chroma_format_idc == 2 ) {
			SubWidthC	= 2;
			SubHeightC	= 1;
		}
		else if ( chroma_format_idc != 3 ) {
			throw InternalError( "Unsupported chroma format!" );
		}
		else if ( ! separate_colour_plane_flag ) {
			SubWidthC	= 1;
			SubHeightC	= 1;
		}
		Uint32	ChromaArrayType	= 0;	// See "separate_colour_plane_flag".
		if ( ! separate_colour_plane_flag ) {
			ChromaArrayType	= chroma_format_idc;
		}
		else {
			ChromaArrayType	= 0;
		}
		Uint32	CropUnitX	= 0;	// See Formule 7-18..7-21.
		Uint32	CropUnitY	= 0;	// See Formule 7-18..7-21.
		if ( ChromaArrayType == 0 ) {
			CropUnitX	= 1;
			CropUnitY	= 2 - frame_mbs_only_flag;
		}
		else {
			CropUnitX	= SubWidthC;
			CropUnitY	= SubHeightC * ( 2 - frame_mbs_only_flag );
		}
		pictWdth -= CropUnitX * ( frame_crop_left_offset + frame_crop_right_offset );
		pictHght -= CropUnitY * ( frame_crop_top_offset + frame_crop_bottom_offset );
	}

}

// ============================================================================

Buffer AVC::SequenceParameterSet::toBuffer() const {

	Buffer res;

	res += "SPS: profile_idc                             : " + Buffer( profile_idc );
	res += "\nSPS: constrained_set0_flag                   : " + Buffer( constrained_set0_flag );
	res += "\nSPS: constrained_set1_flag                   : " + Buffer( constrained_set1_flag );
	res += "\nSPS: constrained_set2_flag                   : " + Buffer( constrained_set2_flag );
	res += "\nSPS: constrained_set3_flag                   : " + Buffer( constrained_set3_flag );
	res += "\nSPS: constrained_set4_flag                   : " + Buffer( constrained_set4_flag );
	res += "\nSPS: constrained_set5_flag                   : " + Buffer( constrained_set5_flag );
	res += "\nSPS: level_idc                               : " + Buffer( level_idc             );
	res += "\nSPS: seq_parameter_set_id                    : " + Buffer( seq_parameter_set_id  );

	if ( profile_idc == FREXT_HP		// 100
	  || profile_idc == FREXT_Hi10P		// 110
	  || profile_idc == FREXT_Hi422		// 122
	  || profile_idc == FREXT_Hi444		// 244
	  || profile_idc == FREXT_CAVLC444	// 44
	  || profile_idc == SCALABLE_BASELINE	// 83
	  || profile_idc == SCALABLE_HIGH	// 86
	  || profile_idc == MVC_HIGH		// 118
	  || profile_idc == STEREO_HIGH ) {	// 128

		res += "\nSPS: chroma_format_idc                       : " + Buffer( chroma_format_idc );
		if ( chroma_format_idc == YUV444 ) {
			res += "\nSPS: separate_colour_plane_flag              : " + Buffer( separate_colour_plane_flag );
		}
		res += "\nSPS: bit_depth_luma_minus8                   : " + Buffer( bit_depth_luma_minus8 );
		res += "\nSPS: bit_depth_chroma_minus8                 : " + Buffer( bit_depth_chroma_minus8 );
		res += "\nSPS: lossless_qpprime_flag                   : " + Buffer( lossless_qpprime_flag );
		res += "\nSPS: seq_scaling_matrix_present_flag         : " + Buffer( seq_scaling_matrix_present_flag );

	}

	res += "\nSPS: log2_max_frame_num_minus4               : " + Buffer( log2_max_frame_num_minus4 );
	res += "\nSPS: pic_order_cnt_type                      : " + Buffer( pic_order_cnt_type );

	if ( pic_order_cnt_type == 0 ) {
		res += "\nSPS: log2_max_pic_order_cnt_lsb_minus4       : " + Buffer( log2_max_pic_order_cnt_lsb_minus4 );
	}
	else if ( pic_order_cnt_type == 1 ) {
		res += "\nSPS: delta_pic_order_always_zero_flag        : " + Buffer( delta_pic_order_always_zero_flag );
		res += "\nSPS: offset_for_non_ref_pic                  : " + Buffer( offset_for_non_ref_pic );
		res += "\nSPS: offset_for_top_to_bottom_field          : " + Buffer( offset_for_top_to_bottom_field );
		res += "\nSPS: num_ref_frames_in_pic_order_cnt_cycle   : " + Buffer( num_ref_frames_in_pic_order_cnt_cycle );
		for ( Uint32 i = 0 ; i < num_ref_frames_in_pic_order_cnt_cycle ; i++ ) {
			res += "\nSPS: offset_for_ref_frame[ i ]               : " + Buffer( offset_for_ref_frame[ i ] );
		}
	}

	res += "\nSPS: num_ref_frames                          : " + Buffer( num_ref_frames );
	res += "\nSPS: gaps_in_frame_num_value_allowed_flag    : " + Buffer( gaps_in_frame_num_value_allowed_flag );
	res += "\nSPS: pic_width_in_mbs_minus1                 : " + Buffer( pic_width_in_mbs_minus1 );
	res += "\nSPS: pic_height_in_map_units_minus1          : " + Buffer( pic_height_in_map_units_minus1 );
	res += "\nSPS: frame_mbs_only_flag                     : " + Buffer( frame_mbs_only_flag );
	if ( ! frame_mbs_only_flag ) {
		res += "\nSPS: mb_adaptive_frame_field_flag            : " + Buffer( mb_adaptive_frame_field_flag );
	}
	res += "\nSPS: direct_8x8_inference_flag               : " + Buffer( direct_8x8_inference_flag );
	res += "\nSPS: frame_cropping_flag                     : " + Buffer( frame_cropping_flag );
	if ( frame_cropping_flag ) {
		res += "\nSPS: frame_crop_left_offset                  : " + Buffer( frame_crop_left_offset );
		res += "\nSPS: frame_crop_right_offset                 : " + Buffer( frame_crop_right_offset );
		res += "\nSPS: frame_crop_top_offset                   : " + Buffer( frame_crop_top_offset );
		res += "\nSPS: frame_crop_bottom_offset                : " + Buffer( frame_crop_bottom_offset );
	}
	res += "\nSPS: vui_parameters_present_flag             : " + Buffer( vui_parameters_present_flag );

	if ( vui_parameters_present_flag ) {
		res += "\n" + vui_seq_parameters->toBuffer();
	}

	return res;

}

// ============================================================================

void AVC::SequenceParameterSet::Scaling_List(
		Int32 *		scalingList,
		Uint32		sizeOfScalingList,
		Uint32 &	UseDefaultScalingMatrix,
		BitstreamPtr	s ) {

	Int32	lastScale = 8;
	Int32	nextScale = 8;

	for ( Uint32 j = 0 ; j < sizeOfScalingList ; j++ ) {
		Uchar scanj = ( sizeOfScalingList == 16 ? ZZ_SCAN[ j ] : ZZ_SCAN8[ j ] );
		if ( nextScale != 0 ) {
			Int32 delta_scale = s->read_se_v();
			nextScale = ( lastScale + delta_scale + 256 ) % 256;
			UseDefaultScalingMatrix = ( scanj == 0 && nextScale == 0 ? 1 : 0 );
		}
		scalingList[ scanj ] = ( nextScale == 0 ? lastScale : nextScale );
		lastScale = scalingList[ scanj ];
	}

}

// ============================================================================

const Uchar AVC::SequenceParameterSet::ZZ_SCAN[ 16 ] = {
	0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

const Uchar AVC::SequenceParameterSet::ZZ_SCAN8[ 64 ] = {
	0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

// ============================================================================

