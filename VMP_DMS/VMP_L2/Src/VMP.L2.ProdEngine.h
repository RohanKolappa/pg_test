// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.ProdEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_ProdEngine_H_
#define _VMP_L2_ProdEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.ProdEnginePtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "VMP.L2.Engine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all engines working in Prod mode.
///
/// \ingroup VMP_L2

class VMP_L2_DLL ProdEngine : public Engine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new ProdEngine.

	ProdEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		Engine( Prod, pType ),
		nbrLoops	( 0 ),
		nbrFrmes	( 0 )
	{
	}

public :

	/// \brief Returns the data type the unique input should produce.

	const BFC::DL::TypeCPtr & getInputType(
	) const {
		return Engine::getInputType( 0 );
	}

	void setNbrLoops(
		const	BFC::Uint32	pNbrLoops
	) {
		nbrLoops = pNbrLoops;
	}

	void setNbrFrames(
		const	BFC::Uint32	pNbrFrmes
	) {
		nbrFrmes = pNbrFrmes;
	}

	virtual void fireWork(
	) = 0;

	virtual void haltWork(
	) = 0;

	virtual void waitDone(
	) = 0;

	virtual BFC::Bool isDone(
	) const = 0;

protected :

private :

	BFC::Uint32	nbrLoops;
	BFC::Uint32	nbrFrmes;

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_ProdEngine_H_

// ============================================================================

