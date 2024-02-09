// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.EnmuxerCore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_EnmuxerCore_H_
#define _VMP_L2_EnmuxerCore_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.EnmuxerCorePtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.MT.Condition.h"
#include "BFC.MT.Mutex.h"

#include "VMP.Frame.h"
#include "VMP.Session.h"

#include "VMP.L2.PushEnginePtr.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L2

class VMP_L2_DLL EnmuxerCore : public BFC::DObject {

public :

	/// \brief Creates a new EnmuxerCore.

	EnmuxerCore(
	);

	/// \brief Destroys this object.

	virtual ~EnmuxerCore(
	);

	/// \brief Sets the grace time length to be \a pGrceTime.
	///
	/// \param pGrceTime
	///	The grace time length. Use 0 to get an infinite grace time.

	void setGraceTime(
		const	BFC::Time::Delta &
					pGrceTime
	);

	/// \brief Sets the grace time length to be 0 (no grace time).

	void resetGraceTime(
	);

	void setBlockingPutSession(
		const	BFC::Bool	pBlckPutS
	) {
		blckPutS = pBlckPutS;
	}

	void setBlockingPutFrame(
		const	BFC::Bool	pBlckPutF
	) {
		blckPutF = pBlckPutF;
	}

	void setForceReorder(
		const	BFC::Bool	pFrceOrdr,
		const	BFC::Uint32	pOrdrMain = ( BFC::Uint32 )-1
	) {
		frceOrdr = pFrceOrdr;
		ordrMain = pOrdrMain;
	}

protected :

	friend class EnmuxerEngine;

	BFC::Uint32 addSlot(
	);

	void delSlot(
		const	BFC::Uint32	pSlotIndx
	);

	void attachPeerEngine(
		const	BFC::Uint32	pSlotIndx,
			L2::PushEnginePtr
					pPeerEngn
	);

	void detachPeerEngine(
		const	BFC::Uint32	pSlotIndx
	);

	void putSession(
		const	BFC::Uint32	pSlotIndx,
			SessionPtr	pSlotSess
	);

	void endSession(
		const	BFC::Uint32	pSlotIndx
	);

	void putFrame(
		const	BFC::Uint32	pSlotIndx,
			FramePtr	pSlotFrme
	);

	SessionPtr computeOutputSession(
	);

private :

	/// \brief List of possible Core states.

	enum CoreStatus {
		InitStat,	///< Core assembling the output session.
		BusyStat,	///< Core putting frames to the peer engine.
		GrceStat,	///< Grace time slot started.
		DoneStat	///< Core terminating all input sessions.
	};

	BFC_PTR_DECL( VMP_L2_DLL, Slot )

	class VMP_L2_DLL Slot : virtual public BFC::SObject {

	public :

		Slot(
		) :
			indx	( ( BFC::Uint32 )-1 ),
			cntr	( ( BFC::Uint32 )-1 )
		{
		}

		L2::PushEnginePtr	peer;	///< Peer engine.
		SessionPtr		sess;	///< Input session.
		FramePtr		frme;	///< Output frame.
		BFC::Uint32		indx;	///< Session index.
		BFC::Uint32		cntr;	///< Session counter (+1 on each new session).

	};

	BFC::Bool		useGrTme;	///< Use grace time ?
	BFC::Bool		blckPutS;	///< Blocking putSession() ?
	BFC::Bool		blckPutF;	///< Blocking putFrame() ?
	BFC::Bool		frceOrdr;	///< Force frame reordering based on timestamps ?
	BFC::Uint32		ordrMain;	///< If frceOrdr, index of main component.
	BFC::Time::Delta	grceTime;	///< Grace time (0: infinite).
	BFC::MT::Mutex		statMutx;	///< Table mutex.
	BFC::MT::Condition	statCond;	///< Core status changed.
	CoreStatus		coreStat;	///< Core status.
	BFC::Array< SlotPtr >	slotList;	///< The slot array.
	L2::PushEnginePtr	peerEngn;	///< Current peer engine.
	BFC::Uint32		slotCntr;	///< How many slot used in slotList ?
	BFC::Uint32		linkCntr;	///< How many attached ?
	BFC::Uint32		busyCntr;	///< How many in session ?
	BFC::Uint32		nbrStrms;	///< How many streams in current session ?
	BFC::Uint32		frmeCntr;	///< How many putting a frame ?
	BFC::Time::Clock	frstStop;	///< When did the first input leave the session ?

	/// \brief Forbidden copy constructor.

	EnmuxerCore(
		const	EnmuxerCore &
	);

	/// \brief Forbidden copy operator.

	EnmuxerCore & operator = (
		const	EnmuxerCore &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_EnmuxerCore_H_

// ============================================================================

