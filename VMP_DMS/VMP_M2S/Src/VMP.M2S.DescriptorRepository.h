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
//	VMP.M2S.DescriptorRepository.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_DescriptorRepository_H_
#define _VMP_M2S_DescriptorRepository_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.DescriptorRepositoryPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "VMP.M2S.DescriptorFactoryPtr.h"
#include "VMP.M2S.DescriptorPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL DescriptorRepository : virtual public BFC::SObject {

public :

	/// \brief Creates a new DescriptorRepository.

	DescriptorRepository(
	);

	DescriptorPtr makeDescriptor(
		const	BFC::Uchar	pDescrTag
	);

	BFC::Buffer getNotFound(
	) const;

protected :

	void addFactory(
		const	BFC::Uchar	pDescrTag,
			DescriptorFactoryPtr
					pDescFact
	);

private :

	DescriptorFactoryPtr	factList[ 0x100 ];
	DescriptorFactoryPtr	unknFact;
	BFC::Bool		usedList[ 0x100 ];

	/// \brief Forbidden copy constructor.

	DescriptorRepository(
		const	DescriptorRepository &
	);

	/// \brief Forbidden copy operator.

	DescriptorRepository & operator = (
		const	DescriptorRepository &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_DescriptorRepository_H_

// ============================================================================

