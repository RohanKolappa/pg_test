// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PushEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_PushEngine_H_
#define _VMP_L2_PushEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PushEnginePtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "VMP.Frame.h"
#include "VMP.Session.h"
#include "VMP.L2.Engine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all engines working in Push mode (N out/right,
///	1 in/left).
///
/// \ingroup VMP_L2

class VMP_L2_DLL PushEngine : public Engine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new PushEngine.

	PushEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		Engine( Push, pType ) {
	}

public :

	/// \brief Returns the data type the unique input should produce.

	const BFC::DL::TypeCPtr & getInputType(
	) const {
		return Engine::getInputType( 0 );
	}

	/// \brief Enters a new session.
	///
	/// \param pSession
	///	The new session descriptor.
	///
	/// \throws AlreadyInSession
	///	This object has already successfully entered a new session.
	///
	/// \throws Engine::SessionRefused
	///	This object is not suitable to handle this new session.

	virtual void putSessionCallback(
			SessionPtr		pSession
	) = 0;

	/// \brief Terminates the current session.
	///
	/// \throws NotInSession
	///	This object has not entered a session yet.

	virtual void endSessionCallback(
	) = 0;

	/// \brief Consumes exactly one frame.
	///
	/// \throws NotInSession
	///	This object has not entered a session yet.
	///
	/// \throws Engine::FrameRefused
	///	This bject is not suitable to handle this new frame.
	///
	/// \throws EndOfStream
	///	This exception can be thrown if a (possibly temporary) "end of
	///	stream" condition is encountered. The current session is NOT
	///	ended yet!
	///
	/// \throws BrokenSession
	///	This exception can be thrown if a permanent error condition is
	///	encountered. The current session is automatically ended.

	virtual void putFrameCallback(
			FramePtr		frame
	) = 0;

	virtual void flushSessionCallback(
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_PushEngine_H_

// ============================================================================

