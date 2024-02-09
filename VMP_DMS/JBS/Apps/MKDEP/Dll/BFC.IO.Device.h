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
//	BFC.IO.Device.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_Device_H
#define _BFC_IO_Device_H

// ============================================================================

#include "BFC.Base.SObject.h"
#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

BFC_PTR_DECL(JBS_DLL_NONE,Device)

// ============================================================================

/// \brief Abstract base class of all IO devices (files, sockets, ...).
///
/// This abstract class if the base class inherited by all IO devices.
///
/// An IO device has the following characteristics:
/// - supports only reading, only writing, or both, using a streaming mode,
/// - supports random access or not,
/// - handles only one chunk of bytes ('buffer') at a time.
///
/// In other words, a Device is first open, then you read or write whole
/// buffers to/from it, then you close it.
///
/// Logically, a Device can be seen as a stream of bytes, with the following
/// characteristics:
/// - it is a sequence of length() bytes
/// - it has a cursor inside this sequence, whose position is given by
///   tell()
/// - it a set of flags describing the access the user has to this device: read,
///   write, random, ...
///
/// Furthermore, a Device maintains a flag ('closeOnExit') telling whether or
/// not the Device will perform a 'close()' operation upon destruction. This
/// will usually be 'true', although setting it to 'false' can be particularly
/// interesting in some circumstances (see File::use()).
///
/// Notes:
/// - all InputDevices are growable, meaning that they could potentially throw
///   multiple EOS exceptions on getBytes() calls, and after that return some
///   more fresh bytes.
/// - all OutputDevices are growable too.
///
/// \ingroup Common_IO

class Device : virtual public SObject {

public :

	/// \brief Constructs a new Device.

	Device(
	);

	/// \brief Destroys this Device.

	virtual ~Device(
	);

	/// \brief Tests if this Device is equal to \a src.
	///
	/// Two devices are considered equal iff they are represented by the
	/// same object. Thus, two different BFC::IO::File using the
	/// same physical file will not be considered as equal.

	virtual Bool operator == (
		const	Device&		src
	) const;

	/// \brief Tests if this Device is not equal to \a src.

	virtual Bool operator != (
		const	Device&		src
	) const;

	/// \brief Tests if this Device is connected (i.e. open).

	virtual Bool isConnected(
	);

	/// \brief Tests if this Device will automatically perform a 'close'
	///	operation upon destruction.

	Bool willCloseOnExit(
	) const;

	Bool hasReadAccess(
	) const;

	Bool hasWriteAccess(
	) const;

	Bool hasRandomAccess(
	) const;

	/// \brief Closes the device.

	virtual void close(
	) = 0;

	/// \brief Synchronously gets a random number of bytes from this
	///	Device, and returns them in a Buffer.
	///
	/// \throws NotImplemented
	///	The Device doesn't implement this method.

	virtual Buffer getBytes(
	);

	/// \brief Synchronously puts Buffer \e input to this Device.
	///
	/// \throws NotImplemented
	///	The Device doesn't implement this method.

	virtual void putBytes(
		const	Buffer&		input
	);

	/// \brief Flushes the output buffer.
	///
	/// Default behaviour is to do nothing. You should reimplement this
	/// method in subclasses.

	virtual void flush(
	);

	/// \brief Dumps the whole content to a buffer.
	///
	/// \throws NotConnected
	///	This Device has not been previously opened.
	///
	/// \throws NoReadAccess
	///	This Device cannot be used as an input device.
	///
	/// \throws CantGetBytes
	///	An error occurred while reading bytes from this Device.

	Buffer dumpToBuffer(
	);

	/// \brief Repositions the cursor to absolute position \a offset.

	virtual Uint32 seek(
		const	Uint32		offset
	) = 0;

	/// \brief Repositions the cursor by skipping the next \a offset bytes.
	///
	/// \throws others
	///	Exceptions generated by seek().

	virtual Uint32 skip(
		const	Uint32		offset
	);

	/// \brief Repositions the cursor to the beginning of the stream.
	///
	/// \throws others
	///	Exceptions generated by seek().

	virtual void rewind(
	);

	/// \brief Returns the position of the cursor in the stream.

	virtual Uint32 tell(
	) const = 0;

	/// \brief Returns the length of this stream (in bytes), or 0 if
	///	unknown.

	virtual Uint32 length(
	) const = 0;

	/// \brief Tests if the cursor is at the end of the stream.

	virtual Bool atEOS(
	) const;

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("BFC.IO.Device");

	CLASS_EXCEPTION(AlreadyConnected, "Already connected");
	CLASS_EXCEPTION(ArgumentOverflow, "Argument overflow");
	CLASS_EXCEPTION(BrokenPipe, "Broken pipe");
	CLASS_EXCEPTION(CantGetBytes, "Can't get bytes");
	CLASS_EXCEPTION(CantPutBytes, "Can't put bytes");
	CLASS_EXCEPTION(EmptyArgument, "Empty argument");
	CLASS_EXCEPTION(EndOfStream, "End of stream");
	CLASS_EXCEPTION(InternalError, "Internal error");
	CLASS_EXCEPTION(Interrupted, "Interrupted");
	CLASS_EXCEPTION(InvalidArgument, "Invalid argument");
	CLASS_EXCEPTION(NoRandomAccess, "No random access");
	CLASS_EXCEPTION(NoReadAccess, "No read access");
	CLASS_EXCEPTION(NoWriteAccess, "No write access");
	CLASS_EXCEPTION(NotConnected, "Not connected");
	CLASS_EXCEPTION(NotImplemented, "Operation not implemented");
	CLASS_EXCEPTION(OperationNotSupported, "Operation not supported");
	CLASS_EXCEPTION(Underflow, "Underflow");

protected :

	// internal state...

	Bool	closeOnExit;	///< should we close on exit ?
	Bool	readAccess;	///< do we have read access ?
	Bool	writeAccess;	///< do we have write access ?
	Bool	randomAccess;	///< do we have random access ?

	void setConnected(Bool);

private :

	// internal state...

	Bool		connected;	///< are we connected ?

	// others...

	Device(
		const	Device&
	);

	Device& operator = (
		const	Device&
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_Device_H

// ============================================================================

