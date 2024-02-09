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
//	VMP.M4S.Defines.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_M4S_Defines_H_
#define _VMP_M4S_Defines_H_

// ============================================================================

#include "VMP.M4S.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace VMP {
namespace M4S {

// ============================================================================

/// \brief 14496-1, Table 1 - List of Class Tags for Descriptors
///
/// \ingroup VMP_M4S

enum ClassTag {

//	Forbidden					= 0x00,
	ObjectDescrTag					= 0x01,
	InitialObjectDescrTag				= 0x02,
	ES_DescrTag					= 0x03,
	DecoderConfigDescrTag				= 0x04,
	DecSpecificInfoTag				= 0x05,
	SLConfigDescrTag				= 0x06,
	ContentIdentDescrTag				= 0x07,
	SupplContentIdentDescrTag			= 0x08,
	IPI_DescrPointerTag				= 0x09,
	IPMP_DescrPointerTag				= 0x0A,
	IPMP_DescrTag					= 0x0B,
	QoS_DescrTag					= 0x0C,
	RegistrationDescrTag				= 0x0D,
	ES_ID_IncTag					= 0x0E,
	ES_ID_RefTag					= 0x0F,
	MP4_IOD_Tag					= 0x10,
	MP4_OD_Tag					= 0x11,
	IPL_DescrPointerRefTag				= 0x12,
	ExtendedProfileLevelDescrTag			= 0x13,
	profileLevelIndicationIndexDescrTag		= 0x14,
//	Reserved for ISO use				= 0x15-0x3F,
	ContentClassificationDescrTag			= 0x40,
	KeyWordDescrTag					= 0x41,
	RatingDescrTag					= 0x42,
	LanguageDescrTag				= 0x43,
	ShortTextualDescrTag				= 0x44,
	ExpandedTextualDescrTag				= 0x45,
	ContentCreatorNameDescrTag			= 0x46,
	ContentCreationDateDescrTag			= 0x47,
	OCICreatorNameDescrTag				= 0x48,
	OCICreationDateDescrTag				= 0x49,
	SmpteCameraPositionDescrTag			= 0x4A,
//	Reserved for ISO use (OCI extensions)		= 0x4B-0x5F,
//	Reserved for ISO use				= 0x60-0xBF,
//	User private					= 0xC0-0xFE,
//	Forbidden					= 0xFF

};

// ============================================================================

/// \brief 14496-1, Table 8 - objectTypeIndication Values
///
/// \ingroup VMP_M4S

enum ObjectType {

//	0x00 Forbidden
//	0x01 Systems ISO/IEC 14496-1 (a)
//	0x02 Systems ISO/IEC 14496-1 (b)
//	0x03-0x1F reserved for ISO use
//	0x20 Visual ISO/IEC 14496-2 (c)
//	0x21-0x3F reserved for ISO use
	M4AObjectType		= 0x40,	// Audio ISO/IEC 14496-3
//	0x41-0x5F reserved for ISO use
//	0x60 Visual ISO/IEC 13818-2 Simple Profile
//	0x61 Visual ISO/IEC 13818-2 Main Profile
//	0x62 Visual ISO/IEC 13818-2 SNR Profile
//	0x63 Visual ISO/IEC 13818-2 Spatial Profile
//	0x64 Visual ISO/IEC 13818-2 High Profile
//	0x65 Visual ISO/IEC 13818-2 422 Profile
//	0x66 Audio ISO/IEC 13818-7 Main Profile
//	0x67 Audio ISO/IEC 13818-7 LowComplexity Profile
//	0x68 Audio ISO/IEC 13818-7 Scaleable Sampling Rate Profile
//	0x69 Audio ISO/IEC 13818-3
//	0x6A Visual ISO/IEC 11172-2
//	0x6B Audio ISO/IEC 11172-3
//	0x6C Visual ISO/IEC 10918-1
//	0x6D - 0xBF reserved for ISO use
//	0xC0 - 0xFE user private
//	0xFF no object type specified

};

// ============================================================================

/// \brief 14496-1, Table 9 - streamType Values
///
/// \ingroup VMP_M4S

enum StreamType {

	Forbidden			= 0x00,
	ObjectDescriptorStream		= 0x01,
	ClockReferenceStream		= 0x02,
	SceneDescriptionStream		= 0x03,
	VisualStream			= 0x04,
	AudioStream			= 0x05,
	MPEG7Stream			= 0x06,
	IPMPStream			= 0x07,
	ObjectContentInfoStream		= 0x08,
	MPEGJStream			= 0x09

};

// ============================================================================

} // namespace M4S
} // namespace VMP

// ============================================================================

#endif // _VMP_M4S_Defines_H_

// ============================================================================

