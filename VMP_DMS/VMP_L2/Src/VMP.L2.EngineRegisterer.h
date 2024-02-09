// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.EngineRegisterer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_EngineRegisterer_H_
#define _VMP_L2_EngineRegisterer_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "BFC.DL.Registerer.h"
#include "BFC.DL.SimpleBuilder.h"

#include "VMP.L2.EngineFactory.h"
#include "VMP.L2.EngineRepository.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Helper template class used to register L2 engines to the unique
///	L2 engines repository.
///
/// \ingroup VMP_L2

template < typename TEngine >
class EngineRegisterer : public BFC::DL::Registerer {

public :

	/// \brief Creates a new EngineRegisterer.
	///
	/// This registerer automatically registers a new EngineFactory
	/// creating Engine objects of type \a TEngine to the unique
	/// EngineRepository.

	EngineRegisterer(
	) :
		BFC::DL::Registerer(
			EngineRepository::instance(),
			new EngineFactory(
				new BFC::DL::SimpleBuilder< TEngine > ) ) {
	}

	/// \brief Returns the EngineFactory created by this object.

	EngineFactoryPtr getEngineFactory(
	) const {
		return getFactory();
	}

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_EngineRegisterer_H_

// ============================================================================

