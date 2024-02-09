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
//	VMP.AVC.HRDParameters.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_AVC_HRDParameters_H_
#define _VMP_AVC_HRDParameters_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.HRDParametersPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "VMP.AVC.BitstreamPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL HRDParameters : virtual public BFC::DObject {

public :

	/// \brief Creates a new HRDParameters.

	HRDParameters(
	);

	void decode(
			BitstreamPtr	pStream
	);

	BFC::Buffer toBuffer(
	) const;

	static const BFC::Uint32 MAXIMUMVALUEOFcpb_cnt = 32;

	BFC::Uint32	cpb_cnt_minus1;
	BFC::Uint32	bit_rate_scale;
	BFC::Uint32	cpb_size_scale;
	BFC::Uint32	bit_rate_value_minus1 [MAXIMUMVALUEOFcpb_cnt];
	BFC::Uint32	cpb_size_value_minus1 [MAXIMUMVALUEOFcpb_cnt];
	BFC::Uint32	cbr_flag              [MAXIMUMVALUEOFcpb_cnt];
	BFC::Uint32	initial_cpb_removal_delay_length_minus1;
	BFC::Uint32	cpb_removal_delay_length_minus1;
	BFC::Uint32	dpb_output_delay_length_minus1;
	BFC::Uint32	time_offset_length;

protected :

private :

	/// \brief Forbidden copy constructor.

	HRDParameters(
		const	HRDParameters &
	);

	/// \brief Forbidden copy operator.

	HRDParameters & operator = (
		const	HRDParameters &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_HRDParameters_H_

// ============================================================================

