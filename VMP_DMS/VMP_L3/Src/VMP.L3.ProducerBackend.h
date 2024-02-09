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
//	VMP.L3.ProducerBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_ProducerBackend_H_
#define _VMP_L3_ProducerBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, ProducerBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.Frame.h"
#include "VMP.Session.h"

#include "VMP.L3.Backend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all producer backends.
///
/// \ingroup VMP_L3

class VMP_L3_DLL ProducerBackend : public Backend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new ProducerBackend.

	ProducerBackend(
		const	BFC::DL::TypeCPtr &	pClassType,
		const	BFC::DL::TypeCPtr &	pSessionType
	);

public :

	BFC::DL::TypeCPtr getOutputType(
	) const {
		return sessionType;
	}

	virtual SessionPtr initSession(
	) = 0;

	virtual void exitSession(
	) = 0;

	virtual FramePtr getFrame(
		const	BFC::Uint64	pIndex
	) = 0;

private :

	BFC::DL::TypeCPtr sessionType;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_ProducerBackend_H_

// ============================================================================

