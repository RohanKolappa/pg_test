// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.BackendRegisterer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_BackendRegisterer_H_
#define _VMP_L3_BackendRegisterer_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.DL.Registerer.h"
#include "BFC.DL.SimpleBuilder.h"

#include "VMP.L3.BackendFactory.h"
#include "VMP.L3.BackendRepository.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Helper template class used to register L3 backends to the unique
///	L3 backends repository.
///
/// \ingroup VMP_L3

template < typename TBackend >
class BackendRegisterer : public BFC::DL::Registerer {

public :

	/// \brief Creates a new BackendRegisterer.
	///
	/// This registerer automatically registers a new BackendFactory
	/// creating Backend objects of type \a TBackend, and described by
	/// \a descr, to the unique BackendRepository.

	BackendRegisterer(
	) :
		BFC::DL::Registerer(
			BackendRepository::instance(),
			new BackendFactory(
				new BFC::DL::SimpleBuilder< TBackend > ) ) {
	}

	/// \brief Returns the BackendFactory created by this object.

	BackendFactoryPtr getBackendFactory(
	) const {
		return getFactory();
	}

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_BackendRegisterer_H_

// ============================================================================

