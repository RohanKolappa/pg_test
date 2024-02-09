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
//	VMP.L2.Pumper.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_Pumper_H_
#define _VMP_L2_Pumper_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PumperPtr.h"

// ============================================================================

#include "BFC.MT.Thread.h"

#include "BFC.Time.Delta.h"

#include "VMP.L2.PullEngine.h"
#include "VMP.L2.PushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L2

class VMP_L2_DLL Pumper : public BFC::MT::Thread {

public :

	/// \brief Creates a new Pumper.

	Pumper(
	);

	/// \brief Destroys this object.

	virtual ~Pumper(
	);

	void setSourceEngine(
			PullEnginePtr	pProducer
	) {
		producer = pProducer;
	}

	void setTargetEngine(
			PushEnginePtr	pConsumer
	) {
		consumer = pConsumer;
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

	/// \brief Forces termination of the current session (if any), and
	///	prevents starting a new loop (if any).

	void cleanup(
		const	BFC::Time::Delta &
					pGrceTime = BFC::Time::Delta( 1, 0 )
	);

protected :

	virtual void run(
	);

	void doSession(
	);

private :

	BFC::Bool	niceExit;
	PullEnginePtr	producer;
	PushEnginePtr	consumer;
	BFC::Uint32	nbrLoops;
	BFC::Uint32	nbrFrmes;

	/// \brief Forbidden copy constructor.

	Pumper(
		const	Pumper &
	);

	/// \brief Forbidden copy operator.

	Pumper & operator = (
		const	Pumper &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_Pumper_H_

// ============================================================================

