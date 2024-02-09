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
//	VMP.AVC.VUIParameters.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_AVC_VUIParameters_H_
#define _VMP_AVC_VUIParameters_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.VUIParametersPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "VMP.AVC.BitstreamPtr.h"
#include "VMP.AVC.HRDParametersPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL VUIParameters : virtual public BFC::DObject {

public :

	/// \brief Creates a new VUIParameters.

	VUIParameters(
	);

	void decode(
			BitstreamPtr	pStream
	);

	BFC::Buffer toBuffer(
	) const;

	BFC::Uint32		aspect_ratio_info_present_flag;
	BFC::Uint32		aspect_ratio_idc;
	BFC::Uint32		sar_width;
	BFC::Uint32		sar_height;
	BFC::Uint32		overscan_info_present_flag;
	BFC::Uint32		overscan_appropriate_flag;
	BFC::Uint32		video_signal_type_present_flag;
	BFC::Uint32		video_format;
	BFC::Uint32		video_full_range_flag;
	BFC::Uint32		colour_description_present_flag;
	BFC::Uint32		colour_primaries;
	BFC::Uint32		transfer_characteristics;
	BFC::Uint32		matrix_coefficients;
	BFC::Uint32		chroma_location_info_present_flag;
	BFC::Uint32		chroma_sample_loc_type_top_field;
	BFC::Uint32		chroma_sample_loc_type_bottom_field;
	BFC::Uint32		timing_info_present_flag;
	BFC::Uint32		num_units_in_tick;
	BFC::Uint32		time_scale;
	BFC::Uint32		fixed_frame_rate_flag;
	BFC::Uint32		nal_hrd_parameters_present_flag;
	HRDParametersPtr	nal_hrd_parameters;
	BFC::Uint32		vcl_hrd_parameters_present_flag;
	HRDParametersPtr	vcl_hrd_parameters;
	// if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
	BFC::Uint32		low_delay_hrd_flag;
	BFC::Uint32		pic_struct_present_flag;
	BFC::Uint32		bitstream_restriction_flag;
	BFC::Uint32		motion_vectors_over_pic_boundaries_flag;
	BFC::Uint32		max_bytes_per_pic_denom;
	BFC::Uint32		max_bits_per_mb_denom;
	BFC::Uint32		log2_max_mv_length_vertical;
	BFC::Uint32		log2_max_mv_length_horizontal;
	BFC::Uint32		num_reorder_frames;
	BFC::Uint32		max_dec_frame_buffering;

protected :

private :

	/// \brief Forbidden copy constructor.

	VUIParameters(
		const	VUIParameters &
	);

	/// \brief Forbidden copy operator.

	VUIParameters & operator = (
		const	VUIParameters &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_VUIParameters_H_

// ============================================================================

