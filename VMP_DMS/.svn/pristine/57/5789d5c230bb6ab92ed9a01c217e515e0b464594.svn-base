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
//	VMP.M2S.TSTable.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSTable_h_
#define _VMP_M2S_TSTable_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSTablePtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.TL.Array.h"

#include "VMP.M2S.TSSectionPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSTable : virtual public BFC::DObject {

public :

	TSTable(
			TSSectionPtr	firstSection
	);

	BFC::Bool consume(
			TSSectionPtr	section
	);

	BFC::Uint32 getTableId() const { return table_id; }
	BFC::Bool getSyntaxIndicator() const { return syntax_indicator; }
	BFC::Uint32 getVersion() const { return ( version_number ); }
	BFC::Uint32 getNbrSections() const { return sections.size(); }
	TSSectionPtr getSection( const BFC::Uint32 i ) const { return sections[ i ]; }

	BFC::Bool isReady(
	) const {
		return ( ! syntax_indicator
		      || nbr_sections >= last_section_number + 1 );
	}

protected :

private :

	BFC::Uint32			table_id;
	BFC::Bool			syntax_indicator;
	BFC::Uint32			version_number;
	BFC::Uint32			last_section_number;
	BFC::Uint32			nbr_sections; // # !0 in sections[]
	BFC::Array< TSSectionPtr >	sections;

	/// \brief Forbidden default constructor.

	TSTable(
	);

	/// \brief Forbidden copy constructor.

	TSTable(
		const	TSTable &
	);

	/// \brief Forbidden copy operator.

	TSTable & operator = (
		const	TSTable &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSTable_h_

// ============================================================================

