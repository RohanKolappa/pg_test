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
//	VMP.M4S.SLConfigDescriptor.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M4S_SLConfigDescriptor_H_
#define _VMP_M4S_SLConfigDescriptor_H_

// ============================================================================

#include "VMP.M4S.DLL.h"

// ============================================================================

#include "VMP.M4S.SLConfigDescriptorPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

// ============================================================================

namespace VMP {
namespace M4S {

// ============================================================================

/// \brief ISO/IEC 14496-1, Section ???.
///
///
/// \ingroup VMP_M4S

class VMP_M4S_DLL SLConfigDescriptor : virtual public BFC::DObject {

public :

	/// \brief Creates a new SLConfigDescriptor.

	SLConfigDescriptor(
	);

	void decode(
		const	BFC::Buffer &	pData
	);

	BFC::Buffer encode(
	) const;

protected :

private :

	BFC::Buffer	data;

	/// \brief Forbidden copy constructor.

	SLConfigDescriptor(
		const	SLConfigDescriptor &
	);

	/// \brief Forbidden copy operator.

	SLConfigDescriptor & operator = (
		const	SLConfigDescriptor &
	);

};

// ============================================================================

} // namespace M4S
} // namespace VMP

// ============================================================================

#endif // _VMP_M4S_SLConfigDescriptor_H_

// ============================================================================

