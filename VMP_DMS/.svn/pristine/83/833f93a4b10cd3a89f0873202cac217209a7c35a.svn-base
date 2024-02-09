// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.IOPushOutputDevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_IOPushOutputDevice_H_
#define _VMP_L2_IOPushOutputDevice_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L2 {

BFC_PTR_DECL( VMP_L2_DLL, IOPushOutputDevice )

} // namespace L2
} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.IO.FinalDevice.h"

#include "VMP.BytesSession.h"

#include "VMP.L2.PushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Write-only BFC::IO::Device wrapping an L2::PushEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL IOPushOutputDevice : public BFC::IO::FinalDevice {

public :

	/// \brief Constructs a new IOPushOutputDevice, that will convert
	///	incoming BFC::Buffer objects (BFC::IO interface) into
	///	VMP::BytesFrame objects (VMP::L2 interface) sent to \a engine.
	///
	/// The attached engine should be accepting BytesSession only!

	IOPushOutputDevice(
			PushEnginePtr		engine
	);

	/// \brief Destroys this object.

	virtual ~IOPushOutputDevice(
	);

	// Implementation of the BFC::IO::Device interface...

	/// \brief Opens this object.
	///
	/// This opening operation is converted into a BytesSession
	/// opening, in the VMP::L2 sense.
	///
	/// The actual Session that gets put should have been previously set
	/// using the 'putSession' method below. If no such session has been
	/// set, an "empty" BytesSession will be used (no attached Stream,
	/// no Index, ...).
	///
	/// \throws AlreadyConnected
	///
	/// \throws CantPutSession
	///	This Device received an exception while trying to put a new
	///	Session.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.

	virtual void open(
	);

	/// \brief Installs the Session to be used when opening this Device.
	///
	/// You should use this method to install the BytesSession that this
	/// Device will put (in the VMP::L2 sense) on opening.
	///
	/// \attention Using this method successfully doesn't mean that this
	/// Device has been opened! You still have to open() it after calling
	/// this method!
	///
	/// The same Session will be reused over and over, until the
	/// 'resetSession' is called.

	void putSession(
			BytesSessionPtr		session
	);

	/// \brief Resets the Session to be used when opening this Device.

	void resetSession(
	);

	/// \brief Closes this Device.
	///
	/// This closing operation is converted into a Session closing, in
	/// the VMP::L2 sense.
	///
	/// \throws NotConnected
	///
	/// \throws CantEndSession
	///	This Device received an exception while trying to end the
	///	current BytesSession.

	virtual void close(
	);

	/// \brief Writes bytes to this Device.
	///
	/// This writing operation is converted into a BytesFrame pushing
	/// operation, in the VMP sense (see Node::putFrame()).
	///
	/// \throws NotConnected
	///	This Device needs to be opened first!
	///
	/// \throws BrokenPipe
	///	This Device received a VMP::Node::BrokenSession exception.
	///
	/// \throws EndOfStream
	///	This Device received a VMP::Node::EndOfStream exception.
	///
	/// \throws CantPutBytes
	///	This Device received another exception while trying to put
	///	a BytesFrame.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.

	virtual void putBytes(
		const	BFC::Buffer&		data
	);

	/// \brief Repositions the cursor inside the bytes stream.
	///
	/// This seeking operation gives only a hint to this Device, that
	/// the next 'putFrame' VMP call should try to put data using a
	/// different byte index.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.

	virtual BFC::Uint64 seek(
		const	BFC::Uint64		offset
	);

	/// \brief Returns the actual position in the byte stream.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.

	virtual BFC::Uint64 tell(
	) const;

	/// \brief Returns the stream length (in bytes), or 0 if unknown.
	///
	/// \throws NoAttachedEngine
	///	You didn't attach an engine to this device.

	virtual BFC::Uint64 length(
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.L2.IOPushOutputDevice" );

	CLASS_EXCEPTION( CantEndSession, "Can't end session" );
	CLASS_EXCEPTION( CantPutSession, "Can't put session" );
	CLASS_EXCEPTION( NoAttachedEngine, "No attached engine" );

protected :

private :

	PushEnginePtr			engine;
	BFC::Uint64			nextToPut;
	BFC::Uint64			totalLength;
	BytesSessionPtr			bSession;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	IOPushOutputDevice(
	);

	IOPushOutputDevice(
		const	IOPushOutputDevice&
	);

	IOPushOutputDevice& operator = (
		const	IOPushOutputDevice&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_IOPushOutputDevice_H_

// ============================================================================

