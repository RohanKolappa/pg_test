// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
//
// This file is part of "TBFC::Light".
//
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
// 
// Filename:
//	TBFC.Mem.Branch.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Mem_Branch_H_
#define _TBFC_Mem_Branch_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.Base.FastCS.h"

#include "TBFC.Mem.DataSharer.h"

// ============================================================================

namespace TBFC {
namespace Mem {

// ============================================================================

/// \brief Acts as a hub for 0 or 1 Block object, 0, 1, or more Writer
///	object(s), and 0, 1, or more Reader object(s), who are all sharing the
///	same memory block.
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Branch : public DataSharer {

public :

	Branch(
	);

	Branch(
			Data		* __data
	);

	Branch(
		const	Buffer		& __buffer
	);

	Branch(
		const	Branch		& __other
	);

	Branch(
		const	Branch		& __other,
		const	Uint32		__beg,
		const	Uint32		__len
	);

	/* virtual */ ~Branch(
	);

	Uint32 delCreator(
	);

	void addWriter(
	);

	Uint32 delWriter(
	);

	void addReader(
	);

	Uint32 delReader(
	);

	Buffer toBuffer(
	) const;

	BASE_CLASS_GEN_EXCEPTION( "TBFC.Mem.Branch" );

	CLASS_EXCEPTION( Busy, "Busy" );

protected :

private :

	mutable FastCS	criticalS;	///< Counters protection.
	Uint32		currUsers;	///< Number of users (observers + readers + writers).
	Uint32		currRders;	///< Number of readers.
	Uint32		currWrtrs;	///< Number of writers.

	Branch & operator = (
		const	Branch		&
	);

};

// ============================================================================

} // namespace Mem
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Mem_Branch_H_

// ============================================================================

