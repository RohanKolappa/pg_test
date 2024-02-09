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
//	BFC.IO.File.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_File_H
#define _BFC_IO_File_H

// ============================================================================

#include "BFC.TL.Array.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SPtr.h"

#include "BFC.IO.FinalDevice.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

BFC_PTR_DECL(JBS_DLL_NONE,File)

// ============================================================================

/// \brief A FinalDevice to read/write streams from/to files.
///
/// \ingroup Common_IO

class File : public FinalDevice {

public :

	/// \brief Constructs a new File.

	File(
	);

	/// \brief Destroys this File object.

	virtual ~File(
	);

	/// \brief Returns 'true' iff 'filename' exists.
	///
	/// This method tries to find the file named 'filename' and to
	/// determine its total length.
	/// If the file is not found, this method returns 'false'.
	/// Otherwise, it returns true and the total length is returned
	/// in \e length. Note that this value could be 0 if the total
	/// length cannot be determined, even if the file exists!

	static Bool exists(
		const	Buffer&		filename,
			Uint32&		length
	);

	static Bool exists(
		const	Buffer&		filename
	);

	/// Unconditionnally deletes a file.

	static void del(
		const	Buffer&		filename
	);

#if !defined(PLATFORM_WIN32)

	/// Returns mounted filesystems.

	/// On Unix systems, this method returns a list of all the mount
	/// points of the mounted filesystems.

	static BufferArray getMountedFileSystemsList(
	);

	/// Returns info about a directory.

	struct DirectoryInfo {
		Buffer		mountPoint;
		Uint64		totalSize;
		Uint64		freeSpace;
	};

	static DirectoryInfo getDirectoryInfo(
		const	Buffer&		pathname	// in bytes
	);

	static Bool isCharDevice(
		const	Buffer&		pathname,
			Uint32&		major,
			Uint32&		minor
	);

#endif

	/// Flags for 'open()'ing a file or re'use()'ing an existing fd.

	enum {
		Read		= 0x01,	///< open for reading
		Write		= 0x02,	///< open for writing
		Create		= 0x04,	///< create if not exist
		Truncate	= 0x08,	///< truncate if exist
		Append		= 0x10,	///< append to end if exist
		Sync		= 0x20	///< keep phys in sync
	};

	/// \brief Opens a file.
	///
	/// This method tries to open the file named \a filename, with mode
	/// given by \a mode. If \a closeOnExit is 'true' (the default), the
	/// file will be closed when this object is destroyed.
	///
	/// \throws AlreadyConnected
	///
	/// \throws EmptyArgument
	///
	/// \throws ArgumentOverflow
	///
	/// \throws InvalidArgument
	///
	/// \throws CantSystemOpen

	void open(
		const	Buffer&		fileName,
		const	Uint32		mode,
		const	Uint32		perm = 0644
	);

	/// \brief Reuses a fd.
	///
	/// \warning Don't mess with this method! You have been warned!!!
	///
	/// This method tries to reuse the file descriptor 'fd' that has to be
	/// opened in a mode allowing mode 'mode'. If 'closeOnExit' is true,
	/// this file will be closed when this object is destroyed. Default is
	/// NOT to close that file, for example to prevent closing stdin, ...
	///
	/// \throws AlreadyConnected
	///
	/// \throws InvalidArgument

	void use(
		const	int		fd,
		const	Uint32		mode,
		const 	Bool		closeOnExit = false
	);

	/// \brief Closes this file.
	///
	/// \throws NotConnected
	///
	/// \throws CantSystemClose

	virtual void close(
	);

	/// \brief Reads up to 64 KBytes from the opened file.
	///
	/// \returns The bytes that were read.
	///
	/// \throws NotConnected
	///
	/// \throws NoReadAccess
	///
	/// \throws Interrupted
	///
	/// \throws CantSystemRead
	///
	/// \throws EndOfStream
	///	This exception indicates that
	///	no more bytes could be read. This DOES NOT indicate that
	///	further calls to getBytes() will also fail! It could be only a
	///	TEMPORARY condition, for example if the file can grow
	///	dynamically!

	virtual Buffer getBytes(
	);

	/// \brief Writes \a input into the opened file.
	///
	/// \throws NotConnected
	///
	/// \throws NoWriteAccess
	///
	/// \throws Interrupted
	///
	/// \throws CantSystemWrite
	///
	/// \throws Underflow

	virtual void putBytes(
		const	Buffer&		input
	);

	/// \brief Repositions the cursor inside the opened file.
	///
	/// \returns The new position.
	///
	/// \throws NotConnected
	///
	/// \throws NoRandomAccess
	///
	/// \throws CantSystemSeek

	virtual Uint32 seek(
		const	Uint32		offset
	);

	/// \brief Returns the actual position of the cursor into the file.
	///
	/// \returns The position.
	///
	/// \throws NotConnected
	///
	/// \throws CantSystemTell

	virtual Uint32 tell(
	) const;

	/// \brief Returns the length of the opened file.
	///
	/// \returns The length of the file.
	///
	/// \throws NotConnected
	///
	/// \throws CantSystemFstat

	virtual Uint32 length(
	) const;

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("BFC.IO.File");

#if ! defined( PLATFORM_WIN32 )

	CLASS_EXCEPTION(CantFindMountPoint, "Can't find mount point");
	CLASS_EXCEPTION(CantGetMountedFileSystems,
			"Can't get mounted filesystems");
	CLASS_EXCEPTION(CantOpenSystemFile, "Can't open system file");
	CLASS_EXCEPTION(CantReadSystemFile, "Can't read system file");

#endif

protected :

private :

	// internal state...

	int		fd;	///< POSIX file descriptor

	// others...

	File(
		const	File&
	);

	File& operator = (
		const	File&
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_File_H

// ============================================================================

