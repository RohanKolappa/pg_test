// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.StreamDecoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_StreamDecoderBackend_H_
#define _VMP_L3_StreamDecoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, StreamDecoderBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "BFC.IO.ByteInputStream.h"

#include "VMP.Frame.h"
#include "VMP.BytesSession.h"

#include "VMP.L3.DecoderBackend.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all decoder backends.
///
/// A StreamDecoderBackend is an object capable of decoding a BytesSession into
/// some "uncompressed" or "unencoded" Session (AudioSession, VideoSession,
/// MuxedSession, ...).
///
/// The following diagram represents a typical situation where an L3
/// VideoStreamDecoderBackend is being used by an L2 VideoDecoderEngine, which
/// has created an IO Device to serve as input of encoded content
/// for the backend.
///
/// \verbatim
///+ - - - - - - - - +       .getBytes()     +------------------------------+
///. BFC::IO::ByteIS . <-------------------- | VMP::L3::VideoDecoderBackend |
///+ - - - - - - - - +         .seek()       +------------------------------+
///           |   ^            .tell()                       ^ .attachIODevice()
///           |    \_____________                            | .detachIODevice()
///           |                  \                           |
///           | .getFrameCB()     \____________________      | .initSession()
///           |                           .open()      \     | .exitSession()
///           |                           .close()      \    | .getFrame()
///           v                                          \   | .seekFrame()
///+ - - - - - - - - - - +                   + - - - - - - - - - - - - - - +
///. VMP::L2::PullEngine . <---------------- . VMP::L2::VideoDecoderEngine .
///+ - - - - - - - - - - +  .getSessionCB()  + - - - - - - - - - - - - - - +
///                         .endSessionCB()
/// \endverbatim
///
/// The order in which operations should be performed is:
/// -	the user (engine for example) creates a new IO Device AND opens it
/// -	the user attaches this Device to the backend (attachIODevice())
/// -	the user creates a new BytesSession descriptor, and presents it to
///	the backend (initSession()). The returned Session descriptor
///	should reflect the output of the backend, and specifies if it supports
///	seeking or not.
/// -	the user asks the backend to decode some frames (getFrame())
/// -	if the backend supports seeking, the user may use the seekFrame() to
///	access random frames, otherwise the user will call the seekIODevice()
///	to ask the backend to move to some externally computed position, and
///	resets itself.
/// -	the user closes the backend (exitSession())
/// -	the user detaches the IO Device (detachIODevice())
/// -	the user closes the IO Device, if not already done by the backend.
///
/// \ingroup VMP_L3

class VMP_L3_DLL StreamDecoderBackend : public DecoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new StreamDecoderBackend.

	StreamDecoderBackend(
		const	BFC::DL::TypeCPtr &	pType
	) :
		DecoderBackend( pType ) {
	}

public :

	virtual const BFC::DL::TypeCPtr & getOutputType(
	) const = 0;

	/// \brief Connects this codec to its source of bytes.
	///
	/// The opening and closing of this device will be taken care of
	/// outside of the backend. This object should never try to open
	/// or close this device itself!
	///
	/// \param idevice
	///	The input device that this decoder shall use to get encoded
	///	content.

	virtual void attachIODevice(
			BFC::IO::ByteInputStreamPtr	idevice
	);

	/// \brief Returns the source of bytes attached to this object.
	///
	/// This method should be used by subclasses to retrieve their source
	/// of encoded content.

	virtual BFC::IO::ByteInputStreamPtr getIODevice(
	);

	/// \brief Disconnects this codec from its source of bytes.
	///
	/// This is typically the last operation this object will perform
	/// before being destroyed.

	virtual void detachIODevice(
	);

	/// \brief Returns true iff this decoder can decode the bytes session
	///	described by \a pIBSession, and accessible through the input
	///	device \a pIDevice.

	virtual BFC::Bool canDecode(
			BytesSessionPtr		pIBSession
	) = 0;

	/// \brief Initializes this codec.
	///
	/// Initializing this object is mandatory before trying to
	/// decode content.
	///
	/// The input device (see attachIODevice()) has previously been setup
	/// to be ready to return the first byte of the input stream.
	///
	/// This is the place where a backend can state that it will handle
	/// seeking commands (see seekFrame()) or not, by using suitable
	/// flags while constructing the output video session descriptor.
	///
	/// \param pIBSession
	///	A VMP description of the input Session. If provided (which is
	///	not guaranteed), this descriptor could also transport a Stream
	///	descriptor (see BytesSession::getContent()), but this is not
	///	guaranteed too.
	///
	/// \throws AlreadyInitialized
	///	This codec has already been successfully initialized.
	///
	/// \throws NotSuitable
	///	This codec has read enough of the input stream to be sure that
	///	it is not suitable to decode it. If the same stream is
	///	presented again, the same exception will be thrown. This is
	///	thus a permanent failure.

	virtual SessionPtr initSession(
			BytesSessionPtr		pIBSession
	) = 0;

	/// \brief Finalizes this codec.
	///
	/// \throws NotInitialized

	virtual void exitSession(
	) = 0;

	/// \brief Resets this codec.
	///
	/// This method indicates to this decoder that the input
	/// device (see attachIODevice()) has been modified outside of this
	/// class, and points again
	/// to some 'synchronization point' (start of a keyframe, ...).
	///
	/// \throws NotInitialized

	virtual void resetSession(
	);

	/// \brief Decodes exactly one video frame.
	///
	/// This backend is free to return any Frame it could decode from the
	/// input device, but should try its best to return the next frame in
	/// sequence.
	///
	/// \throws NotInitialized
	///	This backend has not been previously initialized.
	///
	/// \throws BrokenPipe
	///	This backend could not read anything because the input device
	///	has been closed by someone else.
	///
	/// \throws EndOfStream
	///	This backend could not decode any new picture because the input
	///	device has reached the EOF.

	virtual FramePtr getFrame(
	) = 0;

	/// \brief Repositions this object on the frame numbered \a index.
	///
	/// This method should only be reimplemented by backends who declared
	/// they support seeking (see initSession()).
	///
	/// The default implementation simply throws an exception.
	///
	/// \throws NotSuitable
	///	This backend doesn't support seeking.

	virtual void seekFrame(
		const	BFC::Uint64		index
	) = 0;

private :

	BFC::IO::ByteInputStreamPtr	idevice;	///< Input device.

	/// \brief Forbidden default constructor.

	StreamDecoderBackend(
	);

	/// \brief Forbidden copy constructor.

	StreamDecoderBackend(
		const	StreamDecoderBackend &
	);

	/// \brief Forbidden copy operator.

	StreamDecoderBackend & operator = (
		const	StreamDecoderBackend &
	);

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_StreamDecoderBackend_H_

// ============================================================================

