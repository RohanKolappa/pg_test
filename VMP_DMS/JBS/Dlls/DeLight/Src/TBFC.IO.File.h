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
//	TBFC.IO.File.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_File_H_
#define _TBFC_Light_File_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

TBFC_PTR_DECL( TBFC_Light_DLL, File )

// ============================================================================

} // namespace IO
} // namespace TBFC

// ============================================================================

#include "TBFC.TL.Array.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.VFS.FSPath.h"

#include "TBFC.IO.FinalDevice.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

/// \brief A FinalDevice to read/write streams from/to files.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL File : public FinalDevice {

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

	/// Flags used in the constructors.

	enum FileMode {
		Read		= 0x01,	///< Open for reading.
		Write		= 0x02,	///< Open for writing.
		Create		= 0x04,	///< Create if not exist.
		Truncate	= 0x08,	///< Truncate if exist.
		New		= Write | Create | Truncate,	///< Write, create, truncate.
		Append		= 0x10,	///< Append to end if exist.
		Sync		= 0x20,	///< Keep phys in sync.
		ModeMask	= 0x3F	///< All together.
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
		const	Buffer&		fileName,
		const	Uint32		mode,
		const	Uint32		perm = 0644
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
		const	Buffer&		filename,
			Uint64&		length
	);

	static Bool exists(
		const	Buffer&		filename
	);

	/// Unconditionnally deletes a file.

	static void del(
		const	Buffer&		filename
	);

	/// \brief Returns the content of the filesystem node pointed to by
	///	\a filepath.

	static Buffer dump(
		const	FSPath&		filepath
	);

#if ! defined( PLATFORM_WIN32 )  && !defined( PLATFORM_DARWIN )

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

	virtual Buffer getBytes(
	);

	virtual void putBytes(
		const	Buffer&		input
	);

	virtual Uint64 seek(
		const	Uint64		offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("TBFC.IO.File");

	CLASS_EXCEPTION( CantDelete, "Can't delete" );
	CLASS_EXCEPTION( CantStat, "Can't stat" );

#if ! defined( PLATFORM_WIN32 )
	CLASS_EXCEPTION(CantFindMountPoint, "Can't find mount point");
	CLASS_EXCEPTION(CantGetMountedFileSystems,
			"Can't get mounted filesystems");
	CLASS_EXCEPTION(CantOpenSystemFile, "Can't open system file");
	CLASS_EXCEPTION(CantReadSystemFile, "Can't read system file");
	CLASS_EXCEPTION( CantSystemIoctl, "Can't call ::ioctl" );
	CLASS_EXCEPTION( CantSystemStatfs, "Can't call ::statfs" );
#endif

protected :

private :

	// internal state...

	Bool		ce;	///< should we close on exit ?
	int		fd;	///< POSIX file desc
	Buffer		fn;	///< (if ce) file name
	Uint32		md;	///< (if ce) POSIX file mode (OR-ed FileMode's)
	Uint32		pm;	///< (if ce) POSIX file perm
	Uint32		fl;	///< (if ce) Flags (OR-ed Access).

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
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_File_H_

// ============================================================================

