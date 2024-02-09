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
//	VMP.L2.Sink.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_Sink_H_
#define _VMP_L2_Sink_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.SinkPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.MT.Condition.h"
#include "BFC.MT.Mutex.h"

#include "BFC.TL.FIFO.h"

#include "VMP.L2.PullEngine.h"
#include "VMP.L2.PushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Bridge exposing a PushEngine interface on its left side, and a
///	PullEngine on its right side.
///
/// Example: connect a PictureEncoderEngine (Push) to a PictureDecoderEngine.
///
/// \code
///	ProxyPushEnginePtr	encoder = new PictureEncoderEngine( ... );
///	ProxyPullEnginePtr	decoder = new PictureDecoderEngine( ... );
///	SinkPtr			sink = new Sink;
///	encoder->attachPeerEngine( 0, sink->getInputEngine() );
///	decoder->attachPeerEngine( 0, sink->getOutputEngine() );
///	// Now, start 2 threads, one for feeding data to the encoder, and one to read
///	// data from the decoder.
/// \endcode
///
/// \ingroup VMP_L2

class VMP_L2_DLL Sink : virtual public BFC::SObject {

public :

	/// \brief Creates a new Sink.

	Sink(
		const	BFC::Uint32	pMaxSize = 16
	);

	PushEnginePtr getInputEngine(
	) const {
		return iEngine;
	}

	PullEnginePtr getOutputEngine(
	) const {
		return oEngine;
	}

protected :

private :

	BFC_PTR_DECL( VMP_L2_DLL, Fifo )

	class Fifo : virtual public BFC::SObject {

	public :

		Fifo(
			const	BFC::Uint32	pMaxSize
		);

		void putSessionCallback(
				SessionPtr	session
		);
	
		SessionPtr getSessionCallback(
		);
	
		void leftEndSessionCallback(
		);
	
		void rightEndSessionCallback(
		);
	
		void putFrameCallback(
				FramePtr	data
		);
	
		FramePtr getFrameCallback(
			const	BFC::Uint64		index
		);
	
	private :
	
		// internal state...
	
		BFC::MT::Mutex		mutx;
		BFC::MT::Condition		cond;
	
		enum {
			NoneInSession,
			LeftInSession,
			BothInSession,
			LeftOutOfSession,
			RightOutOfSession
		}			stat;
	
		SessionPtr		sess;
		BFC::FIFO< FramePtr >	fifo;

	};

	BFC_PTR_DECL( VMP_L2_DLL, IEngine )

	class IEngine : public PushEngine {

	public :

		IEngine(
				FifoPtr		pFifo
		);

		virtual void putSessionCallback(
				SessionPtr
		);

		virtual void endSessionCallback(
		);

		virtual void putFrameCallback(
				FramePtr
		);

	private :

		FifoPtr		fifo;

	};

	BFC_PTR_DECL( VMP_L2_DLL, OEngine )

	class OEngine : public PullEngine {

	public :

		OEngine(
				FifoPtr		pFifo
		);

		virtual SessionPtr getSessionCallback(
		);

		virtual void endSessionCallback(
		);

		virtual FramePtr getFrameCallback(
			const	BFC::Uint64	pIndex
		);

	private :

		FifoPtr		fifo;

	};

	FifoPtr		fifo;
	PushEnginePtr	iEngine;
	PullEnginePtr	oEngine;
	
	/// \brief Forbidden copy constructor.

	Sink(
		const	Sink &
	);

	/// \brief Forbidden copy operator.

	Sink & operator = (
		const	Sink &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_Sink_H_

// ============================================================================

