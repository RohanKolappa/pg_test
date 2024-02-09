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
//	BFC.IO.File.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if defined( PLATFORM_DARWIN )
#define _DARWIN_FEATURE_64_BIT_INODE
#include <sys/select.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#if defined( PLATFORM_WIN32 )
#include <io.h>
#else
#if ! defined( PLATFORM_DARWIN )
#include <mntent.h>
#include <sys/vfs.h>
#endif
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "BFC.Base.System.h"

#include "BFC.IO.File.h"
#include "BFC.Debug.ThreadFinder.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, File )

// ============================================================================

IO::File::File(
	const	int		descriptor,
	const	Uint32		flags ) {

	setObjectName( "BFC.IO.File" );

	ce = false;
	fd = descriptor;
	fl = flags;

}

IO::File::File(
	const	Buffer &	filename,
	const	Uint32		mode,
	const	Uint32		perm ) {

	setObjectName( "BFC.IO.File" );

	ce = true;
	fd = -1;
	fn = filename;
	md = mode;
	pm = perm;
	fl = NoAccess;

	if ( mode & Read ) {
		fl |= ReadAccess;
	}

	if ( mode & Write ) {
		fl |= WriteAccess;
	}

	fl |= RandomAccess;

}

IO::File::~File() {

	if ( isConnected() ) {
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "~File(): can't auto-close file!" );
		}
	}

}

// ============================================================================

Bool IO::File::exists(
	const	Buffer&		filename,
		Uint64&		length ) {

	Buffer tmp = filename;

#if defined( PLATFORM_WIN32 )

	struct _stati64 s;

	System::bzero( ( Uchar * )&s, sizeof( s ) );

	if ( ::_stati64( ( const char * )tmp.getCStringPtr(), &s ) == -1 ) {
		return false;
	}

#elif defined( PLATFORM_DARWIN )

	struct stat s;
	
	System::bzero( ( Uchar * )&s, sizeof( s ) );
	
	if ( stat( ( const char * )tmp.getCStringPtr(), &s ) == -1 ) {
		return false;
	}
	
#else

	struct stat64 s;

	System::bzero( ( Uchar * )&s, sizeof( s ) );

	if ( ::stat64( ( const char * )tmp.getCStringPtr(), &s ) == -1 ) {
		return false;
	}

#endif

	length = Uint64( s.st_size );

	return true;

}

Bool IO::File::exists(
	const	Buffer&		filename ) {

	Uint64 dummy;

	return exists(filename, dummy);

}

void IO::File::del(
	const	Buffer &	filename ) {

	Buffer full = filename;

	if ( ::remove( ( const char * )full.getCStringPtr() ) == -1 ) {
		throw CantDelete( filename + ": " + ::strerror( errno ) );
	}

}

// ============================================================================

Buffer IO::File::dump(
	const	FSPath &	filepath ) {

	File f( filepath.toBuffer(), Read );
	f.open();
	Buffer r = f.dumpToBuffer();
	f.close();
	return r;

}

void IO::File::dump(
	const	FSPath &	pPath,
	const	Buffer &	pData ) {

	File f( pPath.toBuffer(), New );
	f.open();
	f.putBytes( pData );
	f.close();

}

// ============================================================================

#if ! defined( PLATFORM_WIN32 ) && ! defined( PLATFORM_DARWIN )

BufferArray IO::File::getMountedFileSystemsList() {

	BufferArray list;

	FILE *fp = ::setmntent("/etc/mtab", "r");

	if (!fp) {
		throw CantOpenSystemFile(Buffer("/etc/mtab: ")
		                         + ::strerror(errno));
	}

	struct mntent *	mnt;

	while ((mnt = ::getmntent(fp))) {
		list += Buffer(mnt->mnt_dir);
	}

	::endmntent(fp);

	if (list.isEmpty()) {
		throw CantReadSystemFile("/etc/mtab");
	}

	return list;

}

IO::File::DirectoryInfo IO::File::getDirectoryInfo(
	const	Buffer&		pathname ) {

	DirectoryInfo res;

	// Let's first stat the given pathname ---> must be a valid directory!

	struct stat	pathnameStats;

	System::bzero( ( Uchar * )&pathnameStats, sizeof( pathnameStats ) );

	Buffer		pathfull = pathname;
	const Uchar *	buffer = (const Uchar *)pathfull.getCStringPtr();

	if ( ::stat( ( const char * )buffer, &pathnameStats ) == -1 ) {
		throw CantStat( pathname + ": " + ::strerror( errno ) );
	}

	if ( ! S_ISDIR( pathnameStats.st_mode ) ) {
		throw InvalidArgument();
	}

	// Now, we get a list of all mounted filesystems...

	BufferArray	mounts;

	try {
		mounts = getMountedFileSystemsList();
	}
	catch (Exception& e) {
		throw CantGetMountedFileSystems( e );
	}

	// And we try to find the correct mount point by stating each one...

	Bool		found = false;
	struct stat	mountStats;
	Uint32		i;

	System::bzero( ( Uchar * )&mountStats, sizeof( mountStats ) );

	for (i = 0 ; i < mounts.size() ; i++) {

		Buffer mountfull = mounts[i] + '\0';
		buffer = mountfull.getBufferAddr();

		if ( ::stat( (const char *)buffer, &mountStats ) == -1 ) {
			throw CantStat(
				mounts[i]
				+ ": "
				+ ::strerror( errno ) );
		}

		if (mountStats.st_dev == pathnameStats.st_dev) {
			found = true;
			break;
		}

	}

	if (!found) {
		throw CantFindMountPoint(pathname);
	}

	res.mountPoint = mounts[i];

	// Now, we have to get some stats about this filesystem!

	struct statfs	fsStats;

	System::bzero( ( Uchar * )&fsStats, sizeof( fsStats ) );

	if (::statfs((const char *)buffer, &fsStats) == -1) {
		throw CantSystemStatfs(res.mountPoint + ": " + ::strerror(errno));
	}

	res.totalSize = Uint64( fsStats.f_blocks )
	              * Uint64( fsStats.f_bsize );
	res.freeSpace = Uint64( fsStats.f_bavail )
	              * Uint64( fsStats.f_bsize );

	return res;

}

Bool IO::File::isCharDevice(
	const	Buffer&		pathname,
		Uint32&		major,
		Uint32&		minor) {

	// Let's first stat the given pathname...

	struct stat	stats;
	Buffer		path = pathname + '\0';

	System::bzero( ( Uchar * )&stats, sizeof( stats ) );

	if (::stat((const char *)path.getBufferAddr(), &stats) == -1) {
		return false;
	}

	if (!S_ISCHR(stats.st_mode)) {
		return false;
	}

	Uint32 majmin = (Uint32)stats.st_rdev;

	major = (majmin & 0x0000FF00) >> 8;
	minor = (majmin & 0x000000FF)     ;

	return true;

}

#endif

// ============================================================================

void IO::File::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	if ( ! ce ) {
		setAccess( fl );
		return;
	}

	Buffer tmpFileName = fn;

	if ( ! md				// no flags!
	 || ( md & ~ ModeMask )			// unknown flags!
	 || ! ( md & ( Read | Write ) ) ) {	// no Read || Write!
		throw CantOpen( "Invalid mode!" );
	}

	Bool	r = ( ( md & Read          ) != 0 );
	Bool	w = ( ( md & Write         ) != 0 );
	Bool	c = ( ( md & Create        ) != 0 );
	Bool	t = ( ( md & Truncate      ) != 0 );
	Bool	a = ( ( md & Append        ) != 0 );
	Bool	n = ( ( md & NonBlocking   ) != 0 );

#if defined( PLATFORM_WIN32 )

	int modeval = ( r ? ( w ? _O_RDWR : _O_RDONLY ) : _O_WRONLY )
	            | _O_BINARY;

	if ( c ) modeval |= _O_CREAT;
	if ( t ) modeval |= _O_TRUNC;
	if ( a ) modeval |= _O_APPEND;

	if ( ( fd = ::_open(
			( const char * )tmpFileName.getCStringPtr(),
			modeval,
			pm ) ) == -1 ) {
		throw CantOpen( "\"" + fn + "\": " + ::strerror( errno ) );
	}

#else

	Bool	s = ( ( md & Sync     ) != 0 );

	int modeval = ( r ? ( w ? O_RDWR : O_RDONLY ) : O_WRONLY );

	if ( c ) modeval |= O_CREAT;
	if ( t ) modeval |= O_TRUNC;
	if ( a ) modeval |= O_APPEND;
	if ( s ) modeval |= O_SYNC;
	if ( n ) modeval |= O_NONBLOCK;

	if ( ( fd = 
#if defined( PLATFORM_DARWIN )
		  ::open(
#else
		  ::open64(
#endif
			( const char * )tmpFileName.getCStringPtr(),
			modeval,
			pm ) ) == -1 ) {
		throw CantOpen( "\"" + fn + "\": " + ::strerror( errno ) );
	}

#endif

#if defined ( PLATFORM_WIN32 )
	sk = true;
#elif defined( PLATFORM_DARWIN )
	sk = true;
#else
	struct stat64 st;
	System::bzero( ( Uchar * )&st, sizeof( st ) );
	if ( ::fstat64( fd, &st ) == 0 ) {
		sk = S_ISREG( st.st_mode );
	}
	else {
		sk = false;
	}
#endif

	if ( ! sk ) {
		cr = 0;
		fl &= ~RandomAccess;
	}

	setAccess( fl );

}

void IO::File::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	resetAccess();

	if ( ce ) {
#if defined( PLATFORM_WIN32 )
		if ( ::_close( fd ) == -1 ) {
#else
		if ( ::close( fd ) == -1 ) {
#endif
			throw CantClose( ::strerror( errno ) );
		}
		fd = -1;
	}

}

// ============================================================================

#if ! defined( PLATFORM_WIN32 )

void IO::File::ioctl(
	const	Int32		request,
		void *		param ) {

	if ( ::ioctl( fd, request, param ) == -1 ) {
		throw CantSystemIoctl( ::strerror( errno ) );
	}

}

#endif

// ============================================================================

#if ! defined( PLATFORM_WIN32 )

Bool IO::File::hasData() {
	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasReadAccess() ) {
		throw NoReadAccess();
	}

	fd_set readfds;

	FD_ZERO( &readfds );
	FD_SET( fd, &readfds );

	int c;
	
	for (;;) {
		struct timeval timeout;

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		c = ::select(
			fd + 1,
			&readfds,
			0,
			0,
			&timeout
		);
		
		if ( c == -1 && errno == EINTR ) {
			// ignore EINT when not thread stopped
			if ( DObject::getThreadFinder()->selfShouldStop() ) {
				throw ThreadStopped();
			}
			continue;
		}

		if ( c == -1 ) {
			throw CantSystemSelect( ::strerror( errno ) );
		}
		
		break;
	}

	return c != 0;

}

#endif

Buffer IO::File::getBytes() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasReadAccess() ) {
		throw NoReadAccess();
	}

	Buffer		tmpBuf;
	Uint32		rs = getBlockSize();

	tmpBuf.resize( rs );

	Uchar *		tmpPtr = tmpBuf.getVarPtr();

	int count;

#if defined( PLATFORM_WIN32 )
	if ( ( count = ::_read( fd, tmpPtr, rs ) ) == -1 ) {
#else
	if ( ( count = ::read( fd, tmpPtr, rs ) ) == -1 ) {
#endif
		int e = errno;
		if ( e == EINTR ) {
			throw ThreadStopped();
		}
		safeClose();
		throw BrokenPipe( ::strerror( e ) );
	}

	if ( ! count ) {
		throw EndOfStream();
	}

	cr += count;

	if ( ( Uint32 )count == rs ) {
		return tmpBuf;
	}

	return tmpBuf( 0, ( Uint32 )count );

}

void IO::File::putBytes(
	const	Buffer &	input ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasWriteAccess() ) {
		throw NoWriteAccess();
	}

	const Uchar *	buffer = input.getCstPtr();
	const Uint32	towrite = input.getLength();

	int count;

#if defined( PLATFORM_WIN32 )
	if ( ( count = ::_write( fd, buffer, towrite ) ) == -1 ) {
#else
	if ( ( count = ::write( fd, buffer, towrite ) ) == -1 ) {
#endif
		int e = errno;
		if ( e == EINTR ) {
			throw ThreadStopped();
		}
		safeClose();
		throw BrokenPipe( ::strerror( e ) );
	}

	if ( ( Uint32 )count != towrite ) {
		safeClose();
		throw BrokenPipe();
	}

	cr += towrite;

}

// ============================================================================

Uint64 IO::File::seek(
	const	Uint64		offset) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasRandomAccess() ) {
		throw NoRandomAccess();
	}

#if defined( PLATFORM_WIN32 )
	Int64 o = ( Int64 )::_lseeki64( fd, offset, SEEK_SET );
#elif defined( PLATFORM_DARWIN )
	Int64 o = ( Int64 )::lseek( fd, offset, SEEK_SET );
#else
	Int64 o = ( Int64 )::lseek64( fd, offset, SEEK_SET );
#endif

	if ( o == -1 ) {
		throw CantSeek( ::strerror( errno ) );
	}

	return ( Uint64 )o;

}

Uint64 IO::File::tell() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! sk ) {
		// If not seekable, rely on our own counter!
		return cr;
	}

#if defined( PLATFORM_WIN32 )
	Int64 o = ( Int64 )::_tell( fd );
#elif defined( PLATFORM_DARWIN )
	Int64 o = ( Int64 )::lseek( fd, 0, SEEK_CUR );
#else
	Int64 o = ( Int64 )::lseek64( fd, 0, SEEK_CUR );
#endif

	if ( o == -1 ) {
		throw CantTell( ::strerror( errno ) );
	}

	return ( Uint64 )o;

}

Uint64 IO::File::length() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

#if defined( PLATFORM_WIN32 )

	struct _stati64 s;

	System::bzero( ( Uchar * )&s, sizeof( s ) );

	if ( ::_fstati64( fd, &s ) == -1 ) {
		return Uint64( 0 );
	}

#elif defined( PLATFORM_DARWIN )
	
	struct stat s;
	
	System::bzero( ( Uchar * )&s, sizeof( s ) );
	
	if ( ::fstat( fd, &s ) == -1 ) {
		return Uint64( 0 );
	}
	
#else

	struct stat64 s;

	System::bzero( ( Uchar * )&s, sizeof( s ) );

	if ( ::fstat64( fd, &s ) == -1 ) {
		return Uint64( 0 );
	}

#endif

	return ( Uint64 )s.st_size;

}

// ============================================================================

