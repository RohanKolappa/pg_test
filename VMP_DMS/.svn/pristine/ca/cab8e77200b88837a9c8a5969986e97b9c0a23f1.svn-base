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
//	BFC.IO.File.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_File_H_
#define _BFC_IO_File_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.IO.FilePtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.IO.FinalDevice.h"

#include "BFC.TL.Array.h"

#include "BFC.VFS.FSPath.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

/// \brief A FinalDevice to read/write streams from/to files.
///
/// \ingroup BFC_IO

class BFC_IO_DLL File : public FinalDevice {

public :

	/// \brief Constructs a new file, associated with file descriptor \a fd.
	///
	/// This constructor lets you reuse an already open file descriptor,
	/// for example 0 ('stdin'), or 1 ('stdout').
	///
	/// The open() and close() operations will be NoOps in this case.
	///
	/// \warning Don't mess with this ctor! You have been warned!!!
	///
	/// \param fd
	///	An integer representing the file descriptor of an open file.
	///
	/// \param mode
	///	An OR-ed combination of Device::AccessMode flags, representing the
	///	access mode the file associated with \a fd supports.

	File(
		const	int		fd,
		const	Uint32		mode
	);

	/// \brief Enumerated list of flags specifying the file mode.

	enum FileMode {
		Read		= 0x01,	///< Open for reading.
		Write		= 0x02,	///< Open for writing.
		Create		= 0x04,	///< Create if not exist.
		Truncate	= 0x08,	///< Truncate if exist.
		New		= Write | Create | Truncate,	///< Write, create, truncate.
		Append		= 0x10,	///< Append to end if exist.
		Sync		= 0x20,	///< Keep phys in sync.
		NonBlocking	= 0x40,	///< Linux only: open non blocking (not supported yet in read/write functions...)
		ModeMask	= 0x7F	///< All together.,
	};

	/// \brief Enumerated list of flags specifying the permissions.

	enum PermMode {

#if defined( PLATFORM_WIN32 )

		SetUId		= 0,	///< Set user ID on execution.
		SetGId		= 0,	///< Set group ID on execution.
		StickyBit	= 0,	///< Save swapped text after use (sticky bit) (obsolete).

		OwnerRead	= 0400,		///< Read by owner.
		OwnerWrite	= 0200,		///< Write by owner.
		OwnerExec	= 0100,		///< Execute/search by owner.

		GroupRead	= 0,		///< Read by group.
		GroupWrite	= 0,		///< Write by group.
		GroupExec	= 0,		///< Execute by group.

		OtherRead	= 0,		///< Read by others.
		OtherWrite	= 0,		///< Write by others.
		OtherExec	= 0,		///< Execute by others.

#else

		SetUId		= 04000,	///< Set user ID on execution.
		SetGId		= 02000,	///< Set group ID on execution.
		StickyBit	= 01000,	///< Save swapped text after use (sticky bit) (obsolete).

		OwnerRead	= 0400,		///< Read by owner.
		OwnerWrite	= 0200,		///< Write by owner.
		OwnerExec	= 0100,		///< Execute by owner.

		GroupRead	= 0040,		///< Read by group.
		GroupWrite	= 0020,		///< Write by group.
		GroupExec	= 0010,		///< Execute by group.

		OtherRead	= 0004,		///< Read by others.
		OtherWrite	= 0002,		///< Write by others.
		OtherExec	= 0001,		///< Execute by others.

#endif

		OwnerAll	= OwnerRead | OwnerWrite | OwnerExec,	///< Read, write, and execute by owner.
		GroupAll	= GroupRead | GroupWrite | GroupExec,	///< Read, write, and execute by group.
		OtherAll	= OtherRead | OtherWrite | OtherExec,	///< Read, write, and execute by others.

		ReadOnly	= OwnerRead | GroupRead | OtherRead,
		ReadWrite	= OwnerRead | OwnerWrite | GroupRead | OtherRead,

		DefaultMode	= ReadWrite

	};

	/// \brief Constructs a new file, associated with filename \a fileName.
	///
	/// This constructor associates a filename with this object, but DOES
	/// NOT open the file yet! You will have to call the open() method
	/// later, and to close() it afterwards.
	///
	/// \param fileName
	///	The name of the file to use.
	/// \param mode
	///	The desired file access mode, given as an OR-ed combination
	///	of File::FileMode flags.
	/// \param perm
	///	The mode to use if the file has to be created.

	File(
		const	Buffer &	fileName,
		const	Uint32		mode,
		const	Uint32		perm = DefaultMode
	);

	/// \brief Destroys this File object.
	///
	/// If this File was still open, it is first flushed (if Write mode
	/// selected), then closed.

	virtual ~File(
	);

	/// \brief Returns 'true' iff the file named \a filename exists.
	///
	/// This method tries to find the file named \a filename, and to
	/// determine its total length.
	/// If the file is not found, this method returns 'false'.
	/// Otherwise, it returns true and the total length is returned
	/// in \e length. Note that this value could be 0 if the total
	/// length cannot be determined, even if the file exists!

	static Bool exists(
		const	Buffer &	filename,
			Uint64 &	length
	);

	static Bool exists(
		const	Buffer &	filename
	);

	/// Unconditionnally deletes a file.

	static void del(
		const	Buffer &	filename
	);

	/// \brief Returns the content of the filesystem node pointed to by
	///	\a filepath.

	static Buffer dump(
		const	FSPath &	filepath
	);

	/// \brief Writes \a pData to the filesystem node \a pPath.

	static void dump(
		const	FSPath &	pPath,
		const	Buffer &	pData
	);

#if ! defined( PLATFORM_WIN32 ) && ! defined( PLATFORM_DARWIN )

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
		const	Buffer &	pathname	// in bytes
	);

	static Bool isCharDevice(
		const	Buffer &	pathname,
			Uint32 &	major,
			Uint32 &	minor
	);

#endif

	virtual void open(
	);

	virtual void close(
	);

#if ! defined( PLATFORM_WIN32 )

	void ioctl(
		const	Int32		request,
			void		*param
	);

#endif

#if ! defined( PLATFORM_WIN32 )

	Bool hasData(
	);

#endif

	virtual Buffer getBytes(
	);

	virtual void putBytes(
		const	Buffer &	input
	);

	virtual Uint64 seek(
		const	Uint64		offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("BFC.IO.File");

	CLASS_EXCEPTION( CantDelete, "Can't delete" );
	CLASS_EXCEPTION( CantStat, "Can't stat" );
	CLASS_EXCEPTION( CantSystemSelect, "Can't system select()" );

#if ! defined( PLATFORM_WIN32 )
	CLASS_EXCEPTION( CantFindMountPoint, "Can't find mount point" );
	CLASS_EXCEPTION( CantGetMountedFileSystems,
			"Can't get mounted filesystems" );
	CLASS_EXCEPTION( CantOpenSystemFile, "Can't open system file" );
	CLASS_EXCEPTION( CantReadSystemFile, "Can't read system file" );
	CLASS_EXCEPTION( CantSystemIoctl, "Can't call ::ioctl" );
	CLASS_EXCEPTION( CantSystemStatfs, "Can't call ::statfs" );
#endif

protected :

	int		fd;	///< POSIX file desc

private :

	Bool		ce;	///< should we close on exit ?
	Buffer		fn;	///< (if ce) file name
	Uint32		md;	///< (if ce) POSIX file mode (OR-ed FileMode's)
	Uint32		pm;	///< (if ce) POSIX file perm
	Uint32		fl;	///< (if ce) Flags (OR-ed Access).
	Bool		sk;	///< Seekable (not a FIFO or a pipe) ?
	Uint64		cr;	///< Cursor (if not seekable).

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

#endif // _BFC_IO_File_H_

// ============================================================================

