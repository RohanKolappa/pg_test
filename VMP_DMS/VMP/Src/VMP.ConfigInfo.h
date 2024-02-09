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
//	VMP.ConfigInfo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_ConfigInfo_H_
#define _VMP_ConfigInfo_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.ConfigInfoPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP

class VMP_Libs_Base_DLL ConfigInfo : public BFC::DL::Object {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	ConfigInfo(
		const	BFC::DL::TypeCPtr &
					pType
	) :
		BFC::DL::Object( pType ) {

	}

public :

	virtual void decode(
		const	BFC::Buffer &	pEncoded
	) = 0;

	virtual BFC::Buffer encode(
	) const = 0;

	virtual BFC::Buffer toBuffer(
	) const = 0;

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_ConfigInfo_H_

// ============================================================================

