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
//	VMP.AVC.SliceHeader.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_SliceHeader_H_
#define _VMP_AVC_SliceHeader_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.SliceHeaderPtr.h"

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

class VMP_AVC_DLL SliceHeader : virtual public BFC::DObject {

public :

	/// \brief Creates a new SliceHeader.

	SliceHeader(
	);

	void decode(
			NALUCPtr	pNALU,
		const	BFC::Bool	separate_colour_plane_flag,
		const	BFC::Uint32	log2_max_frame_num,
		const	BFC::Bool	frame_mbs_only_flag,
		const	BFC::Uint32	pic_order_cnt_type,
		const	BFC::Uint32	log2_max_pic_order_cnt_lsb
	);

	BFC::Buffer toBuffer(
	) const;

	BFC::Uint32	first_mb_in_slice;
	BFC::Uint32	slice_type;
	BFC::Uint32	pic_parameter_set_id;
	BFC::Uint32	colour_plane_id;
	BFC::Uint32	frame_num;
	BFC::Uint32	field_pic_flag;
	BFC::Uint32	bottom_field_flag;
	BFC::Uint32	idr_pic_id;
	BFC::Uint32	pic_order_cnt_lsb;

protected :

private :

	/// \brief Forbidden copy constructor.

	SliceHeader(
		const	SliceHeader &
	);

	/// \brief Forbidden copy operator.

	SliceHeader & operator = (
		const	SliceHeader &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_SliceHeader_H_

// ============================================================================

