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
//	VMP.L2.DemuxerCore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_DemuxerCore_H_
#define _VMP_L2_DemuxerCore_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.DemuxerCorePtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.MT.Condition.h"
#include "BFC.MT.Mutex.h"

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L2.PullEngine.h"

#include "VMP.L2.DemuxerFilter.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Helper class.
///
/// A DemuxerCore synchronizes requests coming from DemuxerEngine objects (open
/// and close sessions, get frame) and from the controlling system (set and get
/// user settings, attach and detach peer L2 engine).
///
/// \ingroup VMP_L2

class VMP_L2_DLL DemuxerCore : public BFC::DObject {

public :

	/// \brief Creates a new Core, using \a backend as format specific
	///	backend processor.

	DemuxerCore(
	);

	/// \brief Destroys this Core, automatically terminating any input
	///	session left behind.

	virtual ~DemuxerCore(
	);

	/// \brief Registers a new Engine.
	///
	/// \param pMuxFilter
	///	Filter to use to select the appropriate Session.
	///
	/// \returns A unique identifier (SlotId) to be used in all subsequent
	///	calls made by the same caller Engine to this Core.

	BFC::Uint32 getSlot(
			DemuxerFilterPtr	pMuxFilter
	);

	/// \brief Unregisters a registered Engine.
	///
	/// Once an Engine has unregistered itself, it may not use any of the
	/// methods of this Core anymore.
	///
	/// \param pSlotIndex
	///	The unique identifier returned by a previous call to getSlot().

	void delSlot(
		const	BFC::Uint32		pSlotIndex
	);

	/// \brief Attaches a new peer (left) PullEngine.
	///
	/// A peer PullEngine has to be attached before trying to opening
	/// a session.

	void attachPeerEngine(
			L2::PullEnginePtr	engine
	);

	/// \brief Detaches the currently used peer PullEngine, or the
	///	waiting peer.
	///
	/// All Engines will automatically get notified that the session is
	/// broken the next time they try to fetch fresh data (if in session).

	void detachPeerEngine(
	);

	/// \brief Gets a new output session.
	///
	/// This method blocks until a new input session has been opened,
	/// and one of its elementary streams has been associated to the
	/// calling Engine.
	///
	/// \param pSlotIndex
	///	The unique identifier returned by a previous call to getSlot().
	///
	/// \returns
	///	The output session for the calling Engine.

	SessionPtr getOutputSession(
		const	BFC::Uint32		pSlotIndex
	);

	/// \brief [Engines only] Ends an output session.
	///
	/// \param pSlotIndex
	///	The unique identifier returned by a previous call to getSlot().

	void endOutputSession(
		const	BFC::Uint32		pSlotIndex
	);

//	BFC::Bool isInSession(
//		const	BFC::Uint32		pSlotIndex
//	);

	/// \brief [Engines only] Gets a new output frame.
	///
	/// \param pSlotIndex
	///	The unique identifier returned by a previous call to getSlot().
	///
	/// \throws BrokenSession
	///	In case of broken session.
	///
	/// \throws EndOfStream
	///	In case of end of stream.

	FramePtr getOutputFrame(
		const	BFC::Uint32		pSlotIndex
	);

	/// \brief [Engines only] Seeks to a new position.
	///
	/// \param pSlotIndex
	///	The unique identifier returned by a previous call to getSlot().
	///
	/// \param frameIndex
	///	The index in the open Session! Passing
	///	Frame::NoOffset means any offset is welcome (useful for slaves,
	///	with one master Engine doing all the random access management).

	void seekOutputFrame(
		const	BFC::Uint32		pSlotIndex,
		const	BFC::Uint64		frameIndex
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.L2.DemuxerCore" );

	CLASS_EXCEPTION( BrokenSession, "Broken session" );
	CLASS_EXCEPTION( EndOfStream, "End of stream" );
	CLASS_EXCEPTION( InternalError, "Internal error" );
	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );
	CLASS_EXCEPTION( InvalidCall, "Invalid call" );
	CLASS_EXCEPTION( NotSuitable, "Not suitable" );

protected :

	void purgeCurFrame(
		const	BFC::Uint32		pSessionIndex
	);

	void checkCounters(
	);

private :

	enum SlotStatus {
		SlotStop,
		SlotBusy,
		SlotWait
	};

	struct SlotEntry {
		DemuxerFilterPtr	muxFilter;	///< MuxedSession filter.
		SlotStatus		curStatus;	///< In session ?
		BFC::Uint32		sessIndex;	///< Session index.
	};

	typedef BFC::Array< SlotEntry >		SlotArray;
	typedef BFC::Array< BFC::Uint32 >	Uint32Array;

	BFC::MT::Mutex		coreMutex;	///< Global Core serializer.
	SlotArray		slotArray;	///< The slot array.
	BFC::Uint32		slotCount;	///< Used in slotArray.
	L2::PullEnginePtr	curEngine;	///< Current peer engine.
	BFC::Uint32		nbrEngine;	///< How many times curEngine has been attached.
	BFC::Uint32		countStop;
	BFC::Uint32		countBusy;
	BFC::Uint32		countWait;
	BFC::MT::Condition		countCond;	///< Counters changed.
	MuxedSessionPtr		muxedSess;	///< Active muxed session.
	MuxedIndexCPtr		muxedIndx;	///< Active muxed index.
	Uint32Array		strmArray;	///< # strms in session.
	MuxedFramePtr		currFrame;	///< Current frame to dispatch.
	BFC::MT::Condition		frameCond;	///< Frames fifo changed.
	BFC::Uint64		nxtOffset;	///< Next offset to ask to peer.

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DemuxerCore(
		const	DemuxerCore&
	);

	DemuxerCore& operator = (
		const	DemuxerCore&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_DemuxerCore_H_

// ============================================================================

