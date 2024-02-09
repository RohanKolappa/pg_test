// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PullEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_PullEngine_H_
#define _VMP_L2_PullEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PullEnginePtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "VMP.Frame.h"
#include "VMP.Session.h"
#include "VMP.L2.Engine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all engines working in Pull mode (1 out/right,
///	N in/left).
///
/// \ingroup VMP_L2

class VMP_L2_DLL PullEngine : public Engine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new PullEngine.

	PullEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		Engine( Pull, pType ) {
	}

public :

	/// \brief Returns the data type the unique output should consume.

	const BFC::DL::TypeCPtr & getOutputType(
	) const {
		return Engine::getOutputType( 0 );
	}

	/// \brief Enters a new Session.
	///
	/// \returns A new session description.
	///
	/// \throws AlreadyInSession
	///	This object has already successfully entered a new session.
	///
	/// \throws NotSuitable
	///	This object is not capable of generating a new Session. This
	///	should be used to signal a temporary failure, for example.

	virtual SessionPtr getSessionCallback(
	) = 0;

	/// \brief Terminates the current Session.
	///
	/// \throws NotInSession
	///	This object has not entered a session yet.

	virtual void endSessionCallback(
	) = 0;

	/// \brief Produces a new Frame.
	///
	/// \throws NotInSession
	///	This object has not entered a session yet.
	///
	/// \throws EndOfStream
	///	This exception can be thrown if a (possibly temporary) "end of
	///	stream" condition is encountered. The current session is NOT
	///	ended yet!
	///
	/// \throws BrokenSession
	///	This exception can be thrown if a permanent error condition is
	///	encountered. The current session is automatically ended.

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		index
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	PullEngine(
		const	PullEngine&
	);

	PullEngine& operator = (
		const	PullEngine&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_PullEngine_H_

// ============================================================================

