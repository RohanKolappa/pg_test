// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP".
// 
// "VMP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.IdFilter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_IdFilter_H_
#define _VMP_IdFilter_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL( VMP_Libs_Base_DLL, IdFilter )

} // namespace VMP

// ============================================================================

#include "BFC.TL.CompArray.h"

#include "VMP.Filter.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief Filter discriminating based on an Id.
///
/// This filter has 2 lists of ids, one for allowed ids, one for blocked ids.
///
/// An id will be filtered out if any of the following conditions is true:
/// -	the list of blocked ids is not empty, and it contains this id;
/// -	the list of allowed ids is not empty, and it doesn't contain this id.
///
/// Otherwise, the id will be allowed, that is, if both the following are true:
/// -	the list of blocked ids is empty, or it doesn't contain this id;
/// -	the list of allowed ids is empty, or it contains this id.
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL IdFilter : public Filter {

public :

	void addBlocked(
		const	BFC::Uint32	pId
	) {
		blocked += pId;
	}

	void delBlocked(
		const	BFC::Uint32	pId
	) {
		blocked.remove( pId );
	}

	void resetBlocked(
	) {
		blocked.kill();
	}

	BFC::Bool isBlocked(
		const	BFC::Uint32	pId
	) const {
		return ( ( ! blocked.isEmpty() &&   blocked.contains( pId ) )
		      || ( ! allowed.isEmpty() && ! allowed.contains( pId ) ) );
	}

	void addAllowed(
		const	BFC::Uint32	pId
	) {
		allowed += pId;
	}

	void delAllowed(
		const	BFC::Uint32	pId
	) {
		allowed.remove( pId );
	}

	void resetAllowed(
	) {
		allowed.kill();
	}

	BFC::Bool isAllowed(
		const	BFC::Uint32	pId
	) const {
		return ( ( blocked.isEmpty() || ! blocked.contains( pId ) )
		      && ( allowed.isEmpty() ||   allowed.contains( pId ) ) );
	}

protected :

private :

	BFC::CompArray< BFC::Uint32 >	blocked;
	BFC::CompArray< BFC::Uint32 >	allowed;

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_IdFilter_H_

// ============================================================================

