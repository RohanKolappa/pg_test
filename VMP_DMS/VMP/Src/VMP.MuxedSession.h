// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedSession.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_MuxedSession_H_
#define _VMP_MuxedSession_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.MuxedSessionPtr.h"

// ============================================================================

#include "VMP.Session.h"

#include "VMP.MuxedIndex.h"
#include "VMP.MuxedStream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief Represents a multiplexed session, formed by joining together any
///	number of generic Session objects.
///
/// This type of session is used for example by MuxedEncoderBackend and
/// MuxedEncoderBackend objects to represent the multiplexed session they are
/// reading and/or writing.
///
/// The Stream objects of all the child Session objects are joined together to
/// form a new MuxedStream, which is the semantic content of this session,
/// returned by getStream(), or by getMuxedStream().
///
/// The components of the MuxedStream are indexed using the same order as the
/// components of the MuxedSession, that is,
/// for any i in 0..getNbrComponents()-1:
///
/// \code
///	getComponent( i )->getStream() == getMuxedStream()->getComponent( i )
/// \endcode
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL MuxedSession : public Session {

public :

	/// \brief Unique DL::Type object identifying this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new MuxedSession.
	///
	/// \param pFlags
	///	Type of session this object represents (default is to create
	///	a Pausable session).

	MuxedSession(
		const	Flags		pFlags = Pausable
	);

	/// \brief Creates a copy of \a pOther.

	MuxedSession(
		const	MuxedSession &	pOther
	);

	virtual SessionPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			SessionCPtr	pSession
	) const;

	/// \brief Sets the nominal bitrate of this MuxedSession.
	///
	/// The bitrate is only meaningful if this MuxedSession does not
	/// transport anything else than BytesSession objects, in which case
	/// the value of the bitrate could be used to indicate to some
	/// MuxedEncoderBackend the indented final bitrate of this whole
	/// MuxedSession, after encoding in the specific format used by this
	/// encoder.
	///
	/// \param pBitRate
	///	The nominal bitrate, in B/s.

	void setBitRate(
		const	BFC::Uint32	pBitRate
	) {
		bitRate = pBitRate;
	}

	/// \brief Returns true iff a nominal bitrate has been set.

	BFC::Bool hasBitRate(
	) const {
		return ( bitRate != 0 );
	}

	/// \brief Returns the nominal bitrate.

	BFC::Uint32 getBitRate(
	) const {
		return bitRate;
	}

	/// \brief Returns the MuxedStream formed by joining the
	///	Stream objects of all the child Session objects.

	MuxedStreamPtr getMuxedStream(
	) {
		return getStream();
	}

	/// \brief Returns the MuxedStream formed by joining the
	///	Stream objects of all the child Session objects.

	MuxedStreamCPtr getMuxedStream(
	) const {
		return getStream();
	}

	/// \brief Returns the number of Session objects in this
	///	MuxedSession.

	BFC::Uint32 getNbrComponents(
	) const {
		return session.getSize();
	}

	/// \brief Adds a new Session object to this MuxedSession.
	///
	/// The Stream of the Session is automatically
	/// added to the Stream of this MuxedSession.
	///
	/// \param pSession
	///	The Session object to add.
	///
	/// \returns
	///	The index of this new component (to be used with getComponent()
	///	for example).

	BFC::Uint32 addComponent(
			SessionPtr	pSession
	);

	/// \brief Returns the Session whose id is \a pSessionId.

	SessionPtr getComponent(
		const	BFC::Uint32	pSessionId
	) {
		return session[ pSessionId ];
	}

	/// \brief Returns the Session whose id is \a pSessionId.

	SessionCPtr getComponent(
		const	BFC::Uint32	pSessionId
	) const {
		return session[ pSessionId ];
	}

	SessionCPtr findComponentWithStream(
			StreamCPtr	pStream
	) const;

	void setMuxedIndex(
			MuxedIndexPtr	pMuxedIndex
	) {
		muxedIndex = pMuxedIndex;
	}

	BFC::Bool hasMuxedIndex(
	) const {
		return ( BFC::Bool )muxedIndex;
	}

	MuxedIndexCPtr getMuxedIndex(
	) const {
		return muxedIndex;
	}

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	BFC::Uint32	bitRate;	///< Nominal bitrate (B/s).
	BFC::Array< SessionPtr >
			session;	///< Array of Session children.
	MuxedIndexPtr	muxedIndex;	///< Index of this Session.

	/// \brief Forbidden copy operator.

	MuxedSession & operator = (
		const	MuxedSession &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_MuxedSession_H_

// ============================================================================

