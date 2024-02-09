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
//	VMP.L3.StreamTxcoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_StreamTxcoderBackend_H_
#define _VMP_L3_StreamTxcoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "VMP.L3.StreamTxcoderBackendPtr.h"

// ============================================================================

#include "VMP.Frame.h"
#include "VMP.Session.h"

#include "VMP.L3.TxcoderBackend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

///// \brief Base class of all stream-based transcoders.
/////
///// \ingroup VMP_L3
//
//class VMP_L3_DLL StreamTxcoderBackend : public TxcoderBackend {
//
//public :
//
//	/// \brief Returns the identity of this class.
//
//	static const BFC::DL::TypeCPtr & getClassType(
//	);
//
//protected :
//
//	/// \brief Creates a new StreamTxcoderBackend.
//
//	StreamTxcoderBackend(
//		const	BFC::DL::TypeCPtr &
//					pType
//	);
//
//public :
//
//	/// \brief Returns true iff this transcoder can transcode the session
//	///	described by \a pSession.
//	///
//	/// The default implementation always return true.
//
//	virtual BFC::Bool canEncode(
//			SessionPtr	pSession
//	);
//
//	virtual void putInputSession(
//			SessionPtr	pSession
//	) = 0;
//
//	virtual void endInputSession(
//	) = 0;
//
//	virtual void putInputFrame(
//			FramePtr	pFrame
//	) = 0;
//
//	BFC::Bool hasOutputFrame(
//	) const;
//
//	BFC::Bool hasOutputSession(
//	) const;
//
//	SessionPtr getOutputSession(
//	);
//
//	FramePtr getOutputFrame(
//	);
//
//protected :
//
//	/// \brief Put \a pOutputFrame on the output queue.
//
//	void putOutputFrame(
//			FramePtr	pFrame
//	)
//
//private :
//
//	/// \brief Forbidden copy constructor.
//
//	StreamTxcoderBackend(
//		const	StreamTxcoderBackend &
//	);
//
//	/// \brief Forbidden copy operator.
//
//	StreamTxcoderBackend & operator = (
//		const	StreamTxcoderBackend &
//	);
//
//};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_StreamTxcoderBackend_H_

// ============================================================================

