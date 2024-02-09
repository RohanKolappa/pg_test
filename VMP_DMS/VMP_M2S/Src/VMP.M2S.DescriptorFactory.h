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
//	VMP.M2S.DescriptorFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_DescriptorFactory_H_
#define _VMP_M2S_DescriptorFactory_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.DescriptorFactoryPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "VMP.M2S.DescriptorPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL DescriptorFactory : virtual public BFC::SObject {

public :

	virtual DescriptorPtr makeDescriptor(
	) = 0;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

template < typename TDesc >
class DescriptorFactoryTmpl : public DescriptorFactory {

public :

	virtual DescriptorPtr makeDescriptor(
	);

};

// ============================================================================

template < typename TDesc >
DescriptorPtr DescriptorFactoryTmpl< TDesc >::makeDescriptor() {

	return new TDesc;

}

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_DescriptorFactory_H_

// ============================================================================

