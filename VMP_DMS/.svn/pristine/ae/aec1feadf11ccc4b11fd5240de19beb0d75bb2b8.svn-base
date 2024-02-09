// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.SessionProducer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_SessionProducer_H_
#define _VMP_L2_SessionProducer_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "BFC.IO.ByteInputStream.h"

#include "VMP.L2.FinalPullEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief FinalPullEngine producing VideoFrame objects read from a file.
///
/// \ingroup VMP_L2

class VMP_L2_DLL VideoSessionProducer : public FinalPullEngine {

public :

	/// \brief Creates a new VideoSessionProducer, which will extract the
	///	pictures from a file.
	///
	/// \param filename
	///	Must be "<filename>:<width>:<height>:<format>".
	///	Ex: "mypict.yuv:640:480:YUV420".

	VideoSessionProducer(
		const	BFC::Buffer &		filename
	);

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		index
	);

protected :

private :

	BFC::Buffer			name;
	BFC::IO::ByteInputStreamPtr	stream;

	BFC::Uint32			picW;
	BFC::Uint32			picH;
	BFC::Uint32			picS;
	BFC::Image::PictureFormat			picF;

};

// ============================================================================

/// \brief FinalPullEngine unserializing session from some IO device.
///
/// \ingroup VMP_L2

class VMP_L2_DLL SessionUnserializer : public FinalPullEngine {

public :

	/// \brief Creates a new SessionUnserializer, unserializing output sessions
	///	from input device \a idevice, reopening this device for each new
	///	session if \a reopen is true.

	SessionUnserializer(
			BFC::IO::DevicePtr	idevice,
		const	BFC::Bool		reopen,
		const	BFC::DL::TypeCPtr &	classId = Session::getClassType()
	);

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		index
	);

	BASE_CLASS_GEN_EXCEPTION( "VMP.L2.SessionUnserializer" );

	CLASS_EXCEPTION( InputError, "Input device error" );

protected :

private :

	BFC::Bool			reopen;
	BFC::IO::ByteInputStreamPtr	stream;
	BFC::DL::TypeCPtr		classId;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	SessionUnserializer(
		const	SessionUnserializer&
	);

	SessionUnserializer& operator = (
		const	SessionUnserializer&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_SessionProducer_H_

// ============================================================================

