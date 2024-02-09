// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.DevNullEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_DevNullEngine_H_
#define _VMP_L2_DevNullEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.DevNullEnginePtr.h"

// ============================================================================

#include "BFC.Time.Chrono.h"

#include "VMP.L2.FinalPushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief FinalPushEngine sending everything it receives to /dev/null.
///
/// \ingroup VMP_L2

class VMP_L2_DLL DevNullEngine : public FinalPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	DevNullEngine(
		const	BFC::Bool	pDumpData = false
	);

	// Implementation of the PushEngine interface...

	virtual void putSessionCallback(
			SessionPtr		session
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr		frame
	);

protected :

private :

	BFC::Bool		dumpData;
	BFC::Uint32		numberSessions;
	BFC::Time::Chrono	chronoSessions;
	BFC::Uint64		numberFrames;
	BFC::Uint64		lengthFrames;
	BFC::Time::Chrono	chronoFrames;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DevNullEngine(
		const	DevNullEngine&
	);

	DevNullEngine& operator = (
		const	DevNullEngine&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_DevNullEngine_H_

// ============================================================================

