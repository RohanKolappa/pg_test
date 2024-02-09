// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.L2.EngineRepository.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_L2_EngineRepository_H_
#define _VMP_L2_EngineRepository_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.EngineRepositoryPtr.h"

// ============================================================================

#include "BFC.Msg.Observer.h"

#include "BFC.DL.Repository.h"

#include "BFC.TL.Pair.h"

#include "VMP.L2.EngineFactoryPtr.h"

#include "VMP.L3.BackendRepository.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Repository of L2 engines.
///
/// \ingroup VMP_L2

class VMP_L2_DLL EngineRepository :

	public BFC::DL::Repository,
	public BFC::Msg::Observer {

public :

	/// \brief Returns the unique instance of this class.

	static EngineRepositoryPtr instance(
	);

protected :

	/// \brief Creates a new EngineRepository.

	EngineRepository(
	);

public :

	/// \brief Destroys this object.

	virtual ~EngineRepository(
	);

	EngineFactoryPtr getEngineFactory(
		const	BFC::UUId &	pUUId
	) const;

protected :

	void observerCallback(
			BFC::Msg::EventCPtr		pEvent
	);

	void addWrapper(
			BFC::DL::FactoryPtr	factory
	);

	void delWrapper(
			BFC::DL::FactoryPtr	factory
	);

private :

	L3::BackendRepositoryPtr	l3rep;

	typedef BFC::Pair<
		BFC::DL::FactoryPtr,
		BFC::DL::FactoryPtr >	WrappedPair;

	typedef BFC::Array< WrappedPair >
					WrappedArray;

	WrappedArray			wrapped;

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_EngineRepository_H_

// ============================================================================

