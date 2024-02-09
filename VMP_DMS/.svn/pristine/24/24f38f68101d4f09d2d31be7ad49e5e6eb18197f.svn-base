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
//	VMP.L3.StreamEncoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_StreamEncoderBackend_H_
#define _VMP_L3_StreamEncoderBackend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, StreamEncoderBackend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "VMP.Frame.h"
#include "VMP.BytesSession.h"

#include "VMP.L3.EncoderBackend.h"
#include "VMP.L3.StreamEncoderODevice.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all stream-based encoder backends.
///
/// An StreamEncoderBackend is an object capable of encoding some "uncompressed" or
/// "unencoded" session into a BytesSession, where the content of the Session is
/// provided in multiple chunks (multiple Frame objects).
///
/// \ingroup VMP_L3

class VMP_L3_DLL StreamEncoderBackend : public EncoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new StreamEncoderBackend.

	StreamEncoderBackend(
		const	BFC::DL::TypeCPtr &	pType
	);

public :

	virtual const BFC::DL::TypeCPtr & getInputType(
	) const = 0;

	/// \brief Attaches this encoder to its output queue.
	///
	/// The output queue can always be retrieved using getIODevice().

	void attachIODevice(
			StreamEncoderODevicePtr	pODevice
	);

	/// \brief Detaches this encoder from its output queue.

	void detachIODevice(
	);

	/// \brief Returns the attached output queue.

	StreamEncoderODevicePtr getIODevice(
	);

	/// \brief Returns true iff this encoder can encode the session
	///	described by \a pSession.
	///
	/// The default implementation always return true.

	virtual BFC::Bool canEncode(
			SessionPtr		pSession
	);

	/// \brief Starts encoding the Session described by \a pSession.
	///
	/// The output queue has NOT YET been attached, so it's NOT
	/// possible to write a header in this method. To do
	/// this, use the putHeader() method instead.
	///
	/// \returns
	///	The BytesSession that will be created by this object.
	///
	/// \throws NotSuitable
	///	If this object is not capable of encoding \a pSession.

	virtual BytesSessionPtr initSession(
			SessionPtr		pSession
	) = 0;

	/// \brief Stops encoding the current Session.
	///
	/// The output device is NOT attached anymore, so it's NOT
	/// possible to write a trailer in this method. To do
	/// this, use the putTrailer() method instead.
	///
	/// The default implementation does nothing.

	virtual void exitSession(
	);

	/// \brief Encodes the header.
	///
	/// The result(s) of encoding the header should be put on the output
	/// queue using StreamEncoderODevice::putBytes() or
	/// StreamEncoderODevice::putFrame().
	///
	/// The default implementation does nothing.
	///
	/// \throws NotSuitable
	///	If this encoder encountered some kind of internal problem. This
	///	encoder is still "in session" after such an exception, but most
	///	probably in an inconsistent state!
	///
	/// \throws EndOfStream
	///	If this encoder tried to put something on the output queue, but
	///	this resulted in some EndOfStream exception being thrown. This
	///	encoder is still "in session" after such an exception!
	///
	/// \throws BrokenPipe
	///	If this encoder tried to put something on the output queue, but
	///	this resulted in some BrokenPipe exception being thrown. This
	///	encoder is NOT "in session" anymore after such an exception!

	virtual void putHeader(
	);

	/// \brief Encodes Frame \a pFrame.
	///
	/// The result(s) of encoding \a pFrame should be put on the output
	/// queue using StreamEncoderODevice::putBytes() or
	/// StreamEncoderODevice::putFrame().
	///
	/// \param pFrame
	///	The Frame to encode.
	///
	/// \throws NotSuitable
	///	If this encoder encountered some kind of internal problem. This
	///	encoder is still "in session" after such an exception, but most
	///	probably in an inconsistent state!
	///
	/// \throws EndOfStream
	///	If this encoder tried to put something on the output queue, but
	///	this resulted in some EndOfStream exception being thrown. This
	///	encoder is still "in session" after such an exception!
	///
	/// \throws BrokenPipe
	///	If this encoder tried to put something on the output queue, but
	///	this resulted in some BrokenPipe exception being thrown. This
	///	encoder is NOT "in session" anymore after such an exception!

	virtual void putFrame(
			FramePtr		pFrame
	) = 0;

	/// \brief Encodes a trailer.
	///
	/// The result(s) of encoding the trailer should be put on the output
	/// queue StreamEncoderODevice::putBytes() or
	/// StreamEncoderODevice::putFrame().
	///
	/// The default implementation does nothing.
	///
	/// \throws NotSuitable
	///	If this encoder encountered some kind of internal problem. This
	///	encoder is still "in session" after such an exception, but most
	///	probably in an inconsistent state!
	///
	/// \throws EndOfStream
	///	If this encoder tried to put something on the output queue, but
	///	this resulted in some EndOfStream exception being thrown. This
	///	encoder is still "in session" after such an exception!
	///
	/// \throws BrokenPipe
	///	If this encoder tried to put something on the output queue, but
	///	this resulted in some BrokenPipe exception being thrown. This
	///	encoder is NOT "in session" anymore after such an exception!

	virtual void putTrailer(
	);

protected :

	/// \brief Put \a pOutputBuffer on the output queue.

	void pushBuffer(
		const	BFC::Buffer &		pOutputBuffer
	) {
		odevice->putBytes( pOutputBuffer );
	}

	/// \brief Put \a pOutputFrame on the output queue.

	void pushFrame(
			FramePtr		pOutputFrame
	) {
		odevice->putFrame( pOutputFrame );
	}

private :

	StreamEncoderODevicePtr	odevice;

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_StreamEncoderBackend_H_

// ============================================================================

