// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::IO".
// 
// "TBFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.IO.SerialPort.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_SerialPort_H_
#define _TBFC_Light_SerialPort_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.IO.FinalDevice.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

// FIXME: to reimplement!!!

#if 0

/// A serial port.

///
/// \ingroup TBFC_Light

class TBFC_Light_DLL SerialPort : public FinalDevice {

public :

	SerialPort();

	virtual ~SerialPort();

	//
	//	enum {
	//		Read,
	//		Write
	//	};
	//
	//	Various flags used when `open()'ing a file or re`use()'ing an
	//	existing file descriptor.
	//	Values:
	//	- Read  : the file has read access;
	//	- Write : the file has write access.
	//

	enum {
		Read		= 0x01,
		Write		= 0x02
	};

	//
	//	void open(
	//		const Buffer&	filename,
	//		const int	mode,
	//		const bool	closeOnExit = true);
	//
	//	Tries to open the file named `filename', with mode given by
	//	`mode'. If `closeOnExit' is `true' (the default), the file
	//	will be closed when this object is destroyed.
	//

	virtual void open(
		const Buffer&	fileName,
		const Uint32	mode,
		const bool	closeOnExit = true
	);

	//	=========================================================
	//	===                   input methods                   ===
	//	=========================================================

	//	for the error codes...

	BASE_CLASS_GEN_EXCEPTION("TBFC.SerialPort");

	CLASS_EXCEPTION(AlreadyConnected, "Already connected");
	CLASS_EXCEPTION(AlreadyReceiving, "Already receiving");
	CLASS_EXCEPTION(AlreadySending, "Already sending");
	CLASS_EXCEPTION(ArgumentOverflow, "Argument overflow");
	CLASS_EXCEPTION(CantFindMountPoint, "Can't find mount point");
	CLASS_EXCEPTION(CantGetMountedFileSystems, "Can't get mounted filesystems");
	CLASS_EXCEPTION(CantOpenSystemFile, "Can't open system file");
	CLASS_EXCEPTION(CantRawRecv, "Can't raw receive");
	CLASS_EXCEPTION(CantReadSystemFile, "Can't read system file");
	CLASS_EXCEPTION(CantSystemClose, "Can't system close()");
	CLASS_EXCEPTION(CantSystemGetAttr, "Can't system tcgetattr()");
	CLASS_EXCEPTION(CantSystemOpen, "Can't system open()");
	CLASS_EXCEPTION(CantSystemRead, "Can't system read()");
	CLASS_EXCEPTION(CantSystemRemove, "Can't system remove()");
	CLASS_EXCEPTION(CantSystemSeek, "Can't system seek()");
	CLASS_EXCEPTION(CantSystemSetAttr, "Can't system tcsetattr()");
	CLASS_EXCEPTION(CantSystemSetSpeed, "Can't system cfsetospeed()");
	CLASS_EXCEPTION(CantSystemStat, "Can't system stat()");
	CLASS_EXCEPTION(CantSystemStatfs, "Can't system statfs()");
	CLASS_EXCEPTION(CantSystemWrite, "Can't system write()");
	CLASS_EXCEPTION(EmptyArgument, "Empty argument");
	CLASS_EXCEPTION(InvalidDescriptor, "Invalid file descriptor");
	CLASS_EXCEPTION(InvalidMode, "Invalid mode");
	CLASS_EXCEPTION(MountPointPathnameTooLong, "Mount point pathname too long");
	CLASS_EXCEPTION(NoReadAccess, "No read access");
	CLASS_EXCEPTION(NotAValidDirectory, "Not a valid directory");
	CLASS_EXCEPTION(NotConnected, "Not connected");
	CLASS_EXCEPTION(OutOfMemory, "Out of virtual memory");
	CLASS_EXCEPTION(PathnameTooLong, "Pathname too long");

protected :

	//	=========================================================
	//	===              low-level IO methods                 ===
	//	=========================================================

	virtual Uint32 rawSend(
		const	Uchar	*buffer,
			Uint32	towrite
	);

	virtual Uint32 rawRecv(
			Uchar	*buffer,
			Uint32	toread
	);

	virtual Uint32 rawSeek(
			Int32	offset,
			SeekDirection	direction
	);

	virtual void rawClose(
	);

private :

	// internals

	int		fd;

	// others...

	SerialPort(
		const SerialPort&
	);

	SerialPort& operator = (
		const SerialPort&
	);

};

#endif

// ============================================================================

} // namespace IO
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_SerialPort_H_

// ============================================================================

