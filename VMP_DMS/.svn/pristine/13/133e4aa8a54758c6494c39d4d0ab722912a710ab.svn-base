// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
//
// This file is part of "BFC::Light".
//
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
// 
// Filename:
//	BFC.Mem.Branch.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_Branch_H_
#define _BFC_Mem_Branch_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCS.h"

#include "BFC.Mem.DataSharer.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Acts as a hub for 0 or 1 Block object, 0, 1, or more Writer
///	object(s), and 0, 1, or more Reader object(s), who are all sharing the
///	same memory block.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Branch : public DataSharer {

public :

	Branch(
	);

	Branch(
			Data *		pData
	);

	Branch(
		const	Buffer &	pBuffer
	);

	Branch(
		const	Branch &	pOther
	);

	Branch(
		const	Branch &	pOther,
		const	Uint32		pBeg,
		const	Uint32		pLen
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

	BASE_CLASS_GEN_EXCEPTION( "BFC.Mem.Branch" );

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
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_Branch_H_

// ============================================================================

