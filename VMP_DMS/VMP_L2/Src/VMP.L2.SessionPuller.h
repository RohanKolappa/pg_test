// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.SessionPuller.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_SessionPuller_H_
#define _VMP_L2_SessionPuller_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.SessionPullerPtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Debug.DObject.h"

#include "VMP.L2.PullEngine.h"
#include "VMP.L2.PushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Helper class connecting a PullEngine to a PushEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL SessionPuller : virtual public BFC::DObject {

public :

	SessionPuller(
			PullEnginePtr		producer,
			PushEnginePtr		consumer,
		const	BFC::Uint32		maxFrames
	);

	virtual ~SessionPuller(
	);

	void stopNow(
	);

	void run(
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.L2.SessionPuller" );

	CLASS_EXCEPTION( WrongUsage, "Wrong usage" );

protected :

	void doLoop(
	);

private :

	BFC::Bool		softStop;
	PullEnginePtr		producer;
	PushEnginePtr		consumer;
	BFC::Uint32		maxFrames;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	SessionPuller(
		const	SessionPuller&
	);

	SessionPuller& operator = (
		const	SessionPuller&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_SessionPuller_H_

// ============================================================================

