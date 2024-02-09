// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.IOPullInputDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_IOPullInputDevice_H_
#define _VMP_L2_IOPullInputDevice_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L2 {

BFC_PTR_DECL( VMP_L2_DLL, IOPullInputDevice )

} // namespace L2
} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.IO.FinalDevice.h"

#include "VMP.BytesSession.h"

#include "VMP.L2.PullEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Read-only BFC::IO::Device wrapping an L2::PullEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL IOPullInputDevice : public BFC::IO::FinalDevice {

public :

	/// \brief Constructs a new IOPullInputDevice, that will convert
	///	incoming VMP::BytesFrame objects received from \a engine (VMP::L2
	///	interface) into BFC::Buffer objects (BFC::IO interface).
	///
	/// The attached engine should be producing BytesSession only!

	IOPullInputDevice(
			PullEnginePtr		engine
	);

	/// \brief Destroys this object.

	virtual ~IOPullInputDevice(
	);

	// Implementation of the BFC::IO::Device interface...

	/// \brief Opens this object.
	///
	/// This opening operation is converted into a BytesSession
	/// opening, in the VMP::L2 sense.
	///
	/// The actual Session that gets negociated can be queried using the
	/// 'getSession()' method.
	///
	/// \throws AlreadyConnected
	///
	/// \throws CantGetSession
	///	This Device received an exception while trying to get a new
	///	Session.
	///
	/// \throws CantBreakSession
	///	This Device received an exception while trying to break a
	///	non BytesSession.
	///
	/// \throws InvalidSession
	///	The engine didn't deliver a valid BytesSession as expected.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.

	virtual void open(
	);

	/// \brief Closes this Device.
	///
	/// This closing operation is converted into a Session closing, in
	/// the VMP::L2 sense.
	///
	/// \throws CantEndSession
	///	This Device received an exception while trying to end the
	///	current BytesSession.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.
	///
	/// \throws NotConnected

	virtual void close(
	);

	/// \brief Returns the actual BytesSession.

	BytesSessionPtr getSession(
	) const {
		return bSession;
	}

	/// \brief Reads bytes from this Device.
	///
	/// This reading operation is converted into a BytesFrame fetching
	/// operation, in the VMP::L2 sense.
	///
	/// \throws BrokenPipe
	///	This Device received a VMP::Node::BrokenSession exception,
	///	or it received a BytesFrame with an invalid index.
	///
	/// \throws EndOfStream
	///	This Device received a VMP::Node::EndOfStream exception.
	///
	/// \throws InvalidFrame
	///	This Device received a Frame that is not a BytesFrame.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.
	///
	/// \throws NotConnected
	///	This Device needs to be opened first!

	virtual BFC::Buffer getBytes(
	);

	/// \brief Repositions the cursor inside the bytes stream.
	///
	/// This seeking operation gives only a hint to this Device, that
	/// the next 'getFrame' VMP call should try to get data using a
	/// different byte index.
	///
	/// If the open BytesSession is not seekable, a call to this method
	/// won't fail, but the next call to 'getBytes' will fail.

	virtual BFC::Uint64 seek(
		const	BFC::Uint64		offset
	);

	/// \brief Returns the actual position in the byte stream.

	virtual BFC::Uint64 tell(
	) const;

	/// \brief Returns the stream length (in bytes), or 0 if unknown.

	virtual BFC::Uint64 length(
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.L2.IOPullInputDevice" );

	CLASS_EXCEPTION( InvalidFrame, "Invalid input frame" );
	CLASS_EXCEPTION( InvalidSession, "Invalid input session" );
	CLASS_EXCEPTION( NoAttachedEngine, "No attached engine" );

protected :

private :

	PullEnginePtr			engine;
	BFC::Uint64			nextToAsk;
	BFC::Uint64			totalLength;
	BytesSessionPtr			bSession;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	IOPullInputDevice(
	);

	IOPullInputDevice(
		const	IOPullInputDevice&
	);

	IOPullInputDevice& operator = (
		const	IOPullInputDevice&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_IOPullInputDevice_H_

// ============================================================================

