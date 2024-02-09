// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.SessionConsumer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_SessionConsumer_H_
#define _VMP_L2_SessionConsumer_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "BFC.IO.ByteOutputStream.h"
#include "BFC.IO.File.h"

#include "VMP.L2.FinalPushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief FinalPushEngine dumping BytesFrame objects to a file.
///
/// \ingroup VMP_L2

class VMP_L2_DLL BytesSessionConsumer : public FinalPushEngine {

public :

	BytesSessionConsumer(
		const	BFC::Buffer&		filename
	);

	virtual void putSessionCallback(
			SessionPtr		session
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr		frame
	);

protected :

private :

	BFC::IO::FilePtr	file;
	BFC::Buffer		name;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	BytesSessionConsumer(
		const	BytesSessionConsumer&
	);

	BytesSessionConsumer& operator = (
		const	BytesSessionConsumer&
	);

};

// ============================================================================

/// \brief FinalPushEngine dumping VideoFrame objects to a file.
///
/// \ingroup VMP_L2

class VMP_L2_DLL VideoSessionConsumer : public FinalPushEngine {

public :

	VideoSessionConsumer(
		const	BFC::Buffer&		filename
	);

	virtual ~VideoSessionConsumer(
	);

	virtual void putSessionCallback(
			SessionPtr		session
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr		frame
	);

protected :

private :

	BFC::Buffer			name;
	BFC::IO::FilePtr		file;

	BFC::Uint32			picW;
	BFC::Uint32			picH;
	BFC::Uint32			picS;
	BFC::Image::PictureFormat			picF;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	VideoSessionConsumer(
		const	VideoSessionConsumer&
	);

	VideoSessionConsumer& operator = (
		const	VideoSessionConsumer&
	);

};

// ============================================================================

/// \brief FinalPushEngine serializing sessions to some IO device.
///
/// \ingroup VMP_L2

class VMP_L2_DLL SessionSerializer : public FinalPushEngine {

public :

	/// \brief Creates a new SessionSerializer, serializing input sessions
	///	to output device \a odevice, reopening this device for each new
	///	session if \a reopen is true.

	SessionSerializer(
			BFC::IO::DevicePtr	odevice,
		const	BFC::Bool		reopen
	);

	/// \brief Destroys this object.

	virtual ~SessionSerializer(
	);

	virtual void putSessionCallback(
			SessionPtr		session
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr		frame
	);

protected :

private :

	BFC::Bool			reopen;
	BFC::IO::ByteOutputStreamPtr	stream;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	SessionSerializer(
		const	SessionSerializer&
	);

	SessionSerializer& operator = (
		const	SessionSerializer&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_SessionConsumer_H_

// ============================================================================

