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
//	VMP.AVC.Defines.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_Defines_H_
#define _VMP_AVC_Defines_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

enum ColorFormat {

	CF_UNKNOWN = -1,     //!< Unknown color format
	YUV400     =  0,     //!< Monochrome
	YUV420     =  1,     //!< 4:2:0
	YUV422     =  2,     //!< 4:2:2
	YUV444     =  3      //!< 4:4:4

};

// ============================================================================

enum ColorModel {

	CM_UNKNOWN = -1,
	CM_YUV     =  0,
	CM_RGB     =  1,
	CM_XYZ     =  2

};

// ============================================================================

enum PixelFormat {

	PF_UNKNOWN = -1,     //!< Unknown color ordering
	UYVY       =  0,     //!< UYVY
	YUY2       =  1,     //!< YUY2
	YUYV       =  1,     //!< YUYV
	YVYU       =  2,     //!< YVYU
	BGR        =  3,     //!< BGR
	V210       =  4      //!< Video Clarity 422 format (10 bits)

};

// ============================================================================

///< AVC Profile IDC definitions

enum ProfileIDC {

	FREXT_CAVLC444		=  44,		//!< YUV 4:4:4/14 "CAVLC 4:4:4"
	BASELINE		=  66,		//!< YUV 4:2:0/8  "Baseline"
	MAIN			=  77,		//!< YUV 4:2:0/8  "Main"
	SCALABLE_BASELINE	=  83,
	SCALABLE_HIGH		=  86,
	EXTENDED		=  88,		//!< YUV 4:2:0/8  "Extended"
	FREXT_HP		= 100,		//!< YUV 4:2:0/8  "High"
	FREXT_Hi10P		= 110,		//!< YUV 4:2:0/10 "High 10"
	MVC_HIGH		= 118,		//!< YUV 4:2:0/8  "Multiview High"
	FREXT_Hi422		= 122,		//!< YUV 4:2:2/10 "High 4:2:2"
	STEREO_HIGH		= 128,		//!< YUV 4:2:0/8  "Stereo High"
	FREXT_Hi444		= 244		//!< YUV 4:4:4/14 "High 4:4:4"

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_Defines_H_

// ============================================================================

