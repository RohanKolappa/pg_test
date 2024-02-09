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
//	VMP.M2S.TSSection.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSSection_h_
#define _VMP_M2S_TSSection_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSSectionPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSSection : virtual public BFC::DObject {

public :

	TSSection(
	);

	void decode(
		const	BFC::Uchar *&		ptr,
			BFC::Uint32 &		len
	);

	BFC::Bool isReady(
	) const;

	BFC::Buffer getPayload(
	) const;

	/// Copy of the whole section.

	BFC::Buffer		buffer;

	// Fields that must be present in every section.

	BFC::Uint32		table_id;			//  8 uimsbf
	BFC::Bool		section_syntax_indicator;	//  1 bslbf
	//			'0';				//  1 bslbf
	//			reserved;			//  2 bslbf
	BFC::Uint32		section_length;			// 12 uimsbf

	// Fields that are valid iff section_syntax_indicator is true!

	BFC::Uint32		table_id_ext;			// 16 uimsbf
	//			reserved;			//  2 uimsbf
	BFC::Uint32		version_number;			//  5 uimsbf
	BFC::Bool		current_next_indicator;		//  1 bslbf
	BFC::Uint32		section_number;			//  8 uimsbf
	BFC::Uint32		last_section_number;		//  8 uimsbf
	BFC::Uint32		CRC_32;				// 32 rpchof

protected :

private :

	/// \brief Forbidden copy constructor.

	TSSection(
		const	TSSection &
	);

	/// \brief Forbidden copy operator.

	TSSection & operator = (
		const	TSSection &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSSection_h_

// ============================================================================

