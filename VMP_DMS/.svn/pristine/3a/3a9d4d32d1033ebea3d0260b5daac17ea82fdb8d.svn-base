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
//	VMP.L2.DemuxerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_DemuxerEngine_H_
#define _VMP_L2_DemuxerEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.DemuxerEnginePtr.h"

// ============================================================================

#include "VMP.L2.DemuxerCore.h"
#include "VMP.L2.DemuxerFilter.h"
#include "VMP.L2.ProxyPullEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPullEngine proxying a MuxedDecoderEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL DemuxerEngine : public ProxyPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Constructs a new DemuxerEngine.
	///
	/// This constructor builds a new DemuxerEngine.
	///
	/// The constructed object will extract the packets of the elementary
	/// stream whose type is, or inherits from, \a pStrmClass, and whose
	/// type index is \a pStrmIndex.
	///
	/// \param pStrmClass
	///	The class type of the elementary stream the caller is
	///	interested in. For example, VideoStream::getClassType(),
	///	AudioStream::getClassType(), or even Stream::getClassType(), in which
	///	case the units of the \p pStrmIndex-th session will be returned.
	///
	/// \param pStrmIndex
	///	The offset of the session to use in the list of all sessions
	///	matching \a pStrmClass. For example, 1 would tune to the
	///	second audio, video, or other stream (specified by \a
	///	pStrmClass).

	DemuxerEngine(
		const	BFC::DL::TypeCPtr &	pStrmClass,
		const	BFC::Uint32		pStrmIndex
	);

	/// \brief Constructs a new DemuxerEngine.
	///
	/// This constructor builds a new DemuxerEngine.
	///
	/// The constructed object will extract the packets of the elementary
	/// stream whose type is, or inherits from, \a pStrmClass, and whose
	/// type index is \a pStrmIndex.
	///
	/// \param pDmxEngine
	///	The demuxer engine whose core should be used.
	///
	/// \param pStrmClass
	///	The class type of the elementary stream the caller is
	///	interested in. For example, VideoStream::getClassType(),
	///	AudioStream::getClassType(), or even Stream::getClassType(), in which
	///	case the units of the \p pStrmIndex-th session will be returned.
	///
	/// \param pStrmIndex
	///	The offset of the session to use in the list of all sessions
	///	matching \a pStrmClass. For example, 1 would tune to the
	///	second audio, video, or other stream (specified by \a
	///	pStrmClass).

	DemuxerEngine(
			DemuxerEnginePtr	pDmxEngine,
		const	BFC::DL::TypeCPtr &	pStrmClass,
		const	BFC::Uint32		pStrmIndex
	);

	/// \brief Constructs a new DemuxerEngine.
	///
	/// This constructor builds a new DemuxerEngine.
	///
	/// The constructed object will extract the packets of the elementary
	/// stream whose type is, or inherits from, \a pStrmClass, and whose
	/// type index is \a pStrmIndex.
	///
	/// \param pDemuxCore
	///	The demuxer core to use.
	///
	/// \param pStrmClass
	///	The class type of the elementary stream the caller is
	///	interested in. For example, VideoStream::getClassType(),
	///	AudioStream::getClassType(), or even Stream::getClassType(), in which
	///	case the units of the \p pStrmIndex-th session will be returned.
	///
	/// \param pStrmIndex
	///	The offset of the session to use in the list of all sessions
	///	matching \a pStrmClass. For example, 1 would tune to the
	///	second audio, video, or other stream (specified by \a
	///	pStrmClass).

	DemuxerEngine(
			DemuxerCorePtr		pDemuxCore,
		const	BFC::DL::TypeCPtr &	pStrmClass,
		const	BFC::Uint32		pStrmIndex
	);

	/// \brief Constructs a new DemuxerEngine.
	///
	/// This constructor builds a new DemuxerEngine.
	///
	/// The constructed object will extract the packets of the elementary
	/// stream selected by \a pStrFilter.
	///
	/// \param pDemuxCore
	///	The demuxer core to use.
	///
	/// \param pStrFilter
	///	The filter to use to select the elementary stream to read
	///	packets from.

	DemuxerEngine(
			DemuxerCorePtr		pDemuxCore,
			DemuxerFilterPtr	pStrFilter
	);

	/// \brief Destroys this object.

	virtual ~DemuxerEngine(
	);

	virtual void attachPeerEngine(
		const	BFC::Uint32		slot,
			PullEnginePtr		engine
	);

	virtual void detachPeerEngine(
		const	BFC::Uint32		slot
	);

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		index
	);

	/// \brief Returns the next available frame in sequence.

	FramePtr getFrameCallback(
	);

protected :

	/// \brief Constructor helper.

	void privInit(
			DemuxerCorePtr		pDemuxCore,
			DemuxerFilterPtr	strFilter
	);

	/// \brief Destructor helper.

	void privExit(
	);

private :

	DemuxerCorePtr		demuxCore;	///< Demuxer core.
	BFC::Uint32		slotIndex;	///< Slot index in the core.
	BFC::Uint64		nextIndex;	///< Next frame index.

	/// \brief Forbidden default constructor.

	DemuxerEngine(
	);

	/// \brief Forbidden copy constructor.

	DemuxerEngine(
		const	DemuxerEngine&
	);

	/// \brief Forbidden copy operator.

	DemuxerEngine& operator = (
		const	DemuxerEngine&
	);

};

// ============================================================================

/// \brief Specialized DemuxerEngine, that is only interested in VideoStream.
///
/// \ingroup VMP_L2

class VMP_L2_DLL VideoDemuxerEngine : public DemuxerEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	VideoDemuxerEngine(
		const	BFC::Uint32	pStrmIndex = 0
	);

};

// ============================================================================

/// \brief Specialized DemuxerEngine, that is only interested in AudioStream.
///
/// \ingroup VMP_L2

class VMP_L2_DLL AudioDemuxerEngine : public DemuxerEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	AudioDemuxerEngine(
		const	BFC::Uint32	pStrmIndex = 0
	);

};

// ============================================================================

/// \brief Specialized DemuxerEngine, that is only interested in DataStream.
///
/// \ingroup VMP_L2

class VMP_L2_DLL DataDemuxerEngine : public DemuxerEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	DataDemuxerEngine(
		const	BFC::Uint32	pStrmIndex = 0
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_DemuxerEngine_H_

// ============================================================================

