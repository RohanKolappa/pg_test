// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.EStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.EStream.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, EStream )

// ============================================================================

static const char *typeTbl[] = {
	"<reserved>",					// 0x00
	"MPEG-1 Video",					// 0x01
	"MPEG-2 Video",					// 0x02
	"MPEG-1 Audio",					// 0x03
	"MPEG-2 Audio",					// 0x04
	"MPEG-2 private sections",			// 0x05
	"MPEG-2 private data",				// 0x06
	"MHEG",						// 0x07
	"MPEG-2 Annex A DSM CC",			// 0x08
	"H.222.1",					// 0x09
	"ISO/IEC 13818-6 type A",			// 0x0A
	"ISO/IEC 13818-6 type B",			// 0x0B
	"ISO/IEC 13818-6 type C",			// 0x0C
	"ISO/IEC 13818-6 type D",			// 0x0D
	"ISO/IEC 13818-1 auxiliary",			// 0x0E
	"ISO/IEC 14496-3 AAC",				// 0x0F
	"ISO/IEC 14496-2 Visual",			// 0x10
	"ISO/IEC 14496-3 Audio",			// 0x11
	"ISO/IEC 14496-1 Stream",			// 0x12
	"ISO/IEC 14496-1 Stream",			// 0x13
	"ISO/IEC 13818-6 SDP",				// 0x14
	"Metadata in PES",				// 0x15
	"Metadata in metadata_sections",		// 0x16
	"Metadata in ISO/IEC 13818-6 Data Carousel",	// 0x17
	"Metadata in ISO/IEC 13818-6 Object Carousel",	// 0x18
	"Metadata in ISO/IEC 13818-6 SDP",		// 0x19
	"MPEG-2 IPMP",					// 0x1A
	"ISO/IEC 14496-10 AVC",				// 0x1B
	"ISO/IEC 14496-3 Audio",			// 0x1C
	"ISO/IEC 14496-17 Text",			// 0x1D
	"ISO/IEC 23002-3 Auxiliary",			// 0x1E
	"ISO/IEC 14496-10 SVC",				// 0x1F
	"ISO/IEC 14496-10 MVC",				// 0x20
	"ISO/IEC 15444-1 Video",			// 0x21
	"ISO/IEC 13818-2 Additional view",		// 0x22
	"ISO/IEC 14496-10 Additional view"		// 0x23
};

Buffer M2S::EStream::getStreamTypeName(
	const	Uint32		i ) {

	if ( i <= 0x23 ) {
		return Buffer( typeTbl[ i ] );
	}

	return "??? (" + Buffer( i, Buffer::Base16, 2 ) + ")";

}

// ============================================================================

