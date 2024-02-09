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
//	VMP.AVC.VUIParameters.cpp
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
#include "VMP.AVC.VUIParameters.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, VUIParameters )

// ============================================================================

AVC::VUIParameters::VUIParameters() {

	setObjectName( "VMP.AVC.VUIParameters" );

	matrix_coefficients = 2;

}

// ============================================================================

void AVC::VUIParameters::decode(
		BitstreamPtr	s ) {

	aspect_ratio_info_present_flag		= s->read_u_1();
	if ( aspect_ratio_info_present_flag ) {
		aspect_ratio_idc			= s->read_u_v( 8 );
		if ( aspect_ratio_idc == 255 /* Extended_SAR */ ) {
			sar_width				= s->read_u_v( 16 );
			sar_height				= s->read_u_v( 16 );
		}
	}

	overscan_info_present_flag		= s->read_u_1();
	if ( overscan_info_present_flag ) {
		overscan_appropriate_flag		= s->read_u_1();
	}

	video_signal_type_present_flag		= s->read_u_1();
	if ( video_signal_type_present_flag ) {
		video_format				= s->read_u_v( 3 );
		video_full_range_flag			= s->read_u_1();
		colour_description_present_flag		= s->read_u_1();
		if ( colour_description_present_flag ) {
			colour_primaries			= s->read_u_v( 8 );
			transfer_characteristics		= s->read_u_v( 8 );
			matrix_coefficients			= s->read_u_v( 8 );
		}
	}

	chroma_location_info_present_flag	= s->read_u_1();
	if ( chroma_location_info_present_flag ) {
		chroma_sample_loc_type_top_field	= s->read_ue_v();
		chroma_sample_loc_type_bottom_field	= s->read_ue_v();
	}

	timing_info_present_flag		= s->read_u_1();
	if ( timing_info_present_flag ) {
		num_units_in_tick			= s->read_u_v( 32 );
		time_scale				= s->read_u_v( 32 );
		fixed_frame_rate_flag			= s->read_u_1();
	}
	else {
		num_units_in_tick			= 0;
		time_scale				= 0;
		fixed_frame_rate_flag			= 0;
	}

	nal_hrd_parameters_present_flag		= s->read_u_1();
	if ( nal_hrd_parameters_present_flag ) {
		nal_hrd_parameters = new HRDParameters;
		nal_hrd_parameters->decode( s );
	}

	vcl_hrd_parameters_present_flag		= s->read_u_1();
	if ( vcl_hrd_parameters_present_flag ) {
		vcl_hrd_parameters = new HRDParameters;
		vcl_hrd_parameters->decode( s );
	}

	if ( nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag ) {
		low_delay_hrd_flag			= s->read_u_1();
	}

	pic_struct_present_flag			= s->read_u_1();
	bitstream_restriction_flag		= s->read_u_1();
	if ( bitstream_restriction_flag ) {
		motion_vectors_over_pic_boundaries_flag	= s->read_u_1();
		max_bytes_per_pic_denom			= s->read_ue_v();
		max_bits_per_mb_denom			= s->read_ue_v();
		log2_max_mv_length_horizontal		= s->read_ue_v();
		log2_max_mv_length_vertical		= s->read_ue_v();
		num_reorder_frames			= s->read_ue_v();
		max_dec_frame_buffering			= s->read_ue_v();
	}

}

// ============================================================================

Buffer AVC::VUIParameters::toBuffer() const {

	Buffer res;

	res += "VUI: aspect_ratio_info_present_flag          : " + Buffer( aspect_ratio_info_present_flag );
	if ( aspect_ratio_info_present_flag ) {
		res += "\nVUI: aspect_ratio_idc                        : " + Buffer( aspect_ratio_idc );
		if ( aspect_ratio_idc == 255 ) {
			res += "\nVUI: sar_width                               : " + Buffer( sar_width );
			res += "\nVUI: sar_height                              : " + Buffer( sar_height );
		}
	}

	res += "\nVUI: overscan_info_present_flag              : " + Buffer( overscan_info_present_flag );
	if ( overscan_info_present_flag ) {
		res += "\nVUI: overscan_appropriate_flag               : " + Buffer( overscan_appropriate_flag );
	}

	res += "\nVUI: video_signal_type_present_flag          : " + Buffer( video_signal_type_present_flag );
	if ( video_signal_type_present_flag ) {
		res += "\nVUI: video_format                            : " + Buffer( video_format );
		res += "\nVUI: video_full_range_flag                   : " + Buffer( video_full_range_flag );
		res += "\nVUI: colour_description_present_flag         : " + Buffer( colour_description_present_flag );
		if ( colour_description_present_flag ) {
			res += "\nVUI: colour_primaries                        : " + Buffer( colour_primaries );
			res += "\nVUI: transfer_characteristics                : " + Buffer( transfer_characteristics );
			res += "\nVUI: matrix_coefficients                     : " + Buffer( matrix_coefficients );
		}
	}

	res += "\nVUI: chroma_location_info_present_flag       : " + Buffer( chroma_location_info_present_flag );
	if ( chroma_location_info_present_flag ) {
		res += "\nVUI: chroma_sample_loc_type_top_field        : " + Buffer( chroma_sample_loc_type_top_field );
		res += "\nVUI: chroma_sample_loc_type_bottom_field     : " + Buffer( chroma_sample_loc_type_bottom_field );
	}

	res += "\nVUI: timing_info_present_flag                : " + Buffer( timing_info_present_flag );
	if ( timing_info_present_flag ) {
		res += "\nVUI: num_units_in_tick                       : " + Buffer( num_units_in_tick );
		res += "\nVUI: time_scale                              : " + Buffer( time_scale );
		res += "\nVUI: fixed_frame_rate_flag                   : " + Buffer( fixed_frame_rate_flag );
	}

	res += "\nVUI: nal_hrd_parameters_present_flag         : " + Buffer( nal_hrd_parameters_present_flag );
	if ( nal_hrd_parameters_present_flag ) {
		res += "\n" + nal_hrd_parameters->toBuffer();
	}

	res += "\nVUI: vcl_hrd_parameters_present_flag         : " + Buffer( vcl_hrd_parameters_present_flag );
	if ( vcl_hrd_parameters_present_flag ) {
		res += "\n" + vcl_hrd_parameters->toBuffer();
	}

	if ( nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag ) {
		res += "\nVUI: low_delay_hrd_flag                      : " + Buffer( low_delay_hrd_flag );
	}

	res += "\nVUI: pic_struct_present_flag                 : " + Buffer( pic_struct_present_flag );
	res += "\nVUI: bitstream_restriction_flag              : " + Buffer( bitstream_restriction_flag );
	if ( bitstream_restriction_flag ) {
		res += "\nVUI: motion_vectors_over_pic_boundaries_flag : " + Buffer( motion_vectors_over_pic_boundaries_flag );
		res += "\nVUI: max_bytes_per_pic_denom                 : " + Buffer( max_bytes_per_pic_denom );
		res += "\nVUI: max_bits_per_mb_denom                   : " + Buffer( max_bits_per_mb_denom );
		res += "\nVUI: log2_max_mv_length_horizontal           : " + Buffer( log2_max_mv_length_horizontal );
		res += "\nVUI: log2_max_mv_length_vertical             : " + Buffer( log2_max_mv_length_vertical );
		res += "\nVUI: num_reorder_frames                      : " + Buffer( num_reorder_frames );
		res += "\nVUI: max_dec_frame_buffering                 : " + Buffer( max_dec_frame_buffering );
	}

	return res;

}

// ============================================================================

