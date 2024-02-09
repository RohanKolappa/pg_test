// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::IO".
// 
// "BFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::IO"; if not, write to:
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

#ifndef _BFC_IO_Device_H_
#define _BFC_IO_Device_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.IO.DevicePtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Debug.DObject.h"

#include "BFC.IO.BrokenPipe.h"
#include "BFC.IO.EndOfStream.h"
#include "BFC.IO.WouldBlock.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace IO {

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
/// \ingroup BFC_IO

class BFC_IO_DLL Device : virtual public DObject {

protected :

	/// \brief Constructs a new Device, whose access mode is NoAccess,
	///	and which is not connected.

	Device(
	);

public :

	enum AccessMode {
		NoAccess	= 0x00,
		ReadAccess	= 0x01,	///< Do we have read access ?
		WriteAccess	= 0x02,	///< Do we have write access ?
		RandomAccess	= 0x04,	///< Do we have random access ?
		AccessMask	= 0x07
	};

	Uint32 getAccessMode(
	) const;

	/// \brief Returns true iff this Device is connected, and has read
	///	access.

	Bool hasReadAccess(
	) const;

	/// \brief Returns true iff this Device is connected, and has write
	///	access.

	Bool hasWriteAccess(
	) const;

	/// \brief Returns true iff this Device is connected, and has random
	///	access.

	Bool hasRandomAccess(
	) const;

	/// \brief Tests if this Device is equal to \a src.
	///
	/// Two devices are considered equal iff they are represented by the
	/// same object. Thus, two different IO::File using the
	/// same physical file will not be considered as equal.

	Bool operator == (
		const	Device &	src
	) const;

	/// \brief Tests if this Device is not equal to \a src.

	Bool operator != (
		const	Device &	src
	) const;

	/// \brief Opens this object.
	///
	/// This method tries to open this Device. The semantics of this
	/// operation is subclass-dependent.
	///
	/// \throws AlreadyConnected
	///	This Device has already been opened.
	///
	/// \throws CantOpen
	///	Something prevented this Device from performing this operation.

	virtual void open(
	) = 0;

	/// \brief Closes this object.
	///
	/// This method tries to close this Device. The semantics of this
	/// operation is subclass-dependent.
	///
	/// \throws NotConnected
	///	This Device has not been successfully opened firsT.
	///
	/// \throws CantClose
	///	Something prevented this Device from performing this operation.

	virtual void close(
	) = 0;

protected :

	/// \brief Declares this device to be open, and setup its access mode.
	///
	/// This method should be used when opening this device.
	///
	/// \param flags
	///	An OR-ed combination of flags from the AccessMode enum.

	void setAccess(
		const	Uint32		flags
	);

	/// \brief Declares this device to not be open anymore, setting its
	///	access mode to NoAccess.

	void resetAccess(
	);

public :

	/// \brief Tests if this Device is connected (i.e. open).

	virtual Bool isConnected(
	) const {
		return connected;
	}

	/// \brief Synonym of isConnected().

	Bool isOpen(
	) const {
		return isConnected();
	}

	/// \brief Sets the preferred size (in bytes) of the blocks returned
	///	by getBytes().
	///
	/// This is only a hint for the Device!

	virtual void setBlockSize(
		const	Uint32		pSize
	) = 0;

	/// \brief Returns the preferred size (in bytes) of the blocks
	///	returned by getBytes().

	virtual Uint32 getBlockSize(
	) const = 0;

	static const Time::Clock NoTimeout;

	virtual Bool hasGetTimeout(
	) const;

	virtual const Time::Clock & getAbsGetTimeout(
	) const = 0;

	virtual Time::Delta getRelGetTimeout(
	) const;

	virtual void setAbsGetTimeout(
		const	Time::Clock &	pAbsTime
	) = 0;

	virtual void setRelGetTimeout(
		const	Time::Delta &	pRelTime
	);

	virtual void resetGetTimeout(
	);

	virtual Bool hasPutTimeout(
	) const;

	virtual const Time::Clock & getAbsPutTimeout(
	) const = 0;

	virtual Time::Delta getRelPutTimeout(
	) const;

	virtual void setAbsPutTimeout(
		const	Time::Clock &	pAbsTime
	) = 0;

	virtual void setRelPutTimeout(
		const	Time::Delta &	pRelTime
	);

	virtual void resetPutTimeout(
	);

	/// \brief Synchronously gets a random number of bytes from this
	///	Device, and returns them in a Buffer.
	///
	/// The number of bytes returned should be equal to the value
	/// returned by getBlockSize(), but this is not guaranteed.
	///
	/// \throws NotConnected
	///	This Device is not connected.
	///
	/// \throws NoReadAccess
	///	This Device cannot be used in this mode.
	///
	/// \throws EndOfStream
	///	There is nothing to be read left! This could be a temporary
	///	condition.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!
	///
	/// \throws BrokenPipe
	///	This Device has been shut down by some other object, and should
	///	be considered as not connected anymore.
	///
	/// \throws CantGetBytes
	///	Something else prevented this Device from getting some bytes.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!
	///
	/// \throws OperationNotImplemented
	///	This Device doesn't implement this method yet.
	///	THIS IS AN IMPLEMENTATION ERROR, NOT A RUNTIME ERROR!

	virtual Buffer getBytes(
	);

	/// \brief Synchronously puts Buffer \e input to this Device.
	///
	/// \throws NotConnected
	///	This Device is not connected.
	///
	/// \throws NoWriteAccess
	///	This Device cannot be used in this mode.
	///
	/// \throws BrokenPipe
	///	The Device has been shut down by some other object, and should
	///	be considered as not connected anymore.
	///
	/// \throws CantPutBytes
	///	Something else prevented this Device from putting bytes.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!
	///
	/// \throws OperationNotImplemented
	///	The Device doesn't implement this method.
	///	THIS IS AN IMPLEMENTATION ERROR, NOT A RUNTIME ERROR!

	virtual void putBytes(
		const	Buffer&		input
	);

	/// \brief Flushes the output buffer.
	///
	/// Default behaviour is to do nothing. You should reimplement this
	/// method in subclasses.
	///
	/// \throws NotConnected
	///	This Device is not connected.
	///
	/// \throws NoWriteAccess
	///	This Device cannot be used in this mode.
	///
	/// \throws BrokenPipe
	///	The Device has been shut down by some other object, and should
	///	be considered as not connected anymore.
	///
	/// \throws CantPutBytes
	///	Something else prevented this Device from putting bytes.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!

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
	/// \throws BrokenPipe
	///	This Device has been shut down by some other object, and should
	///	be considered as not connected anymore.
	///
	/// \throws CantGetBytes
	///	Something else prevented this Device from getting some bytes.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!

	Buffer dumpToBuffer(
	);

	void dumpToDevice(
			DevicePtr	pODevice
	);

	/// \brief Repositions the cursor to absolute position \a offset.
	///
	/// \throws NotConnected
	///	This Device has not been previously opened.
	///
	/// \throws NoRandomAccess
	///	This Device cannot be used as an input device.
	///
	/// \throws BrokenPipe
	///	This Device has been shut down by some other object, and should
	///	be considered as not connected anymore.
	///
	/// \throws CantSeek
	///	Something else prevented this Device from seeking to the
	///	final position.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!

	virtual Uint64 seek(
		const	Uint64		offset
	) = 0;

	/// \brief Repositions the cursor by skipping the next \a offset bytes.
	///
	/// \throws NotConnected
	///	This Device has not been previously opened.
	///
	/// \throws NoRandomAccess
	///	This Device cannot be used as an input device.
	///
	/// \throws BrokenPipe
	///	This Device has been shut down by some other object, and should
	///	be considered as not connected anymore.
	///
	/// \throws CantSeek
	///	Something else prevented this Device from seeking to the
	///	final position.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!

	virtual Uint64 skip(
		const	Uint64		offset
	);

	/// \brief Repositions the cursor to the beginning of the stream.
	///
	/// \throws NotConnected
	///	This Device has not been previously opened.
	///
	/// \throws NoRandomAccess
	///	This Device cannot be used as an input device.
	///
	/// \throws BrokenPipe
	///	This Device has been shut down by some other object, and should
	///	be considered as not connected anymore.
	///
	/// \throws CantSeek
	///	Something else prevented this Device from seeking to the
	///	final position.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!

	virtual void rewind(
	);

	/// \brief Returns the position of the cursor in the stream.
	///
	/// \throws NotConnected
	///	This Device has not been previously opened.
	///
	/// \throws CantTell
	///	Something else prevented this Device from retrieving the
	///	current position.
	///	The Device is still open after this exception has
	///	been thrown and it's legal to call this method again!

	virtual Uint64 tell(
	) const = 0;

	/// \brief Returns the length of this stream (in bytes), or 0 if
	///	unknown.
	///
	/// \throws NotConnected
	///	This Device has not been previously opened.

	virtual Uint64 length(
	) const = 0;

	/// \brief Tests if the cursor is at the end of the stream.
	///
	/// \throws NotConnected
	///	This Device has not been previously opened.

	virtual Bool atEOS(
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("BFC.IO.Device");

	CLASS_EXCEPTION( AlreadyConnected, "Already connected" );
	CLASS_EXCEPTION( CantClose, "Can't close" );
	CLASS_EXCEPTION( CantFlush, "Can't flush" );
	CLASS_EXCEPTION( CantGetBytes, "Can't get bytes" );
	CLASS_EXCEPTION( CantOpen, "Can't open" );
	CLASS_EXCEPTION( CantPutBytes, "Can't put bytes" );
	CLASS_EXCEPTION( CantSeek, "Can't seek" );
	CLASS_EXCEPTION( CantTell, "Can't tell" );
	CLASS_EXCEPTION( NoRandomAccess, "No random access" );
	CLASS_EXCEPTION( NoReadAccess, "No read access" );
	CLASS_EXCEPTION( NoWriteAccess, "No write access" );
	CLASS_EXCEPTION( NotConnected, "Not connected" );

protected :

	void safeClose(
	);

private :

	Uint32		accessMode;	///< OR-ed flags.
	Bool		connected;	///< Are we connected ?

	// others...

	Device(
		const	Device&
	);

	Device& operator = (
		const	Device&
	);

};

// ============================================================================

/// \brief C++-like interface.
///
/// \ingroup BFC_IO

BFC_IO_DLL Device & operator << (
		Device		& dev,
	const	Buffer		& buf
);

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_Device_H_

// ============================================================================

