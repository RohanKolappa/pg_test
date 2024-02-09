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
//	BFC.IO.File.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#if defined(PLATFORM_WIN32)
#include <io.h>
#else
#include <mntent.h>
#include <unistd.h>
#include <sys/vfs.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "BFC.IO.File.h"

// ============================================================================

using namespace BFC;
using namespace BFC::IO;

// ============================================================================

BFC_PTR_IMPL(File)

// ============================================================================

File::File() {

	fd = -1;

}

File::~File() {

	if (isConnected()) {
		try {
			close();
		}
		catch (Exception& ) {
			// msgExc( e, "Can't close file!" );
		}
	}

}

// ============================================================================

Bool File::exists(
	const	Buffer&		filename,
		Uint32&		length) {

	if (filename.isEmpty()) {
		throw EmptyArgument();
	}

	if (filename.length() >= 128) {
		throw ArgumentOverflow();
	}

	Uchar	buffer[128];

	filename.dumpToBuffer(buffer, 128);

	struct stat s;

	if (::stat((const char *)buffer, &s) == -1) {
		return false;
	}

	length = (Uint32)s.st_size;

	return true;

}

Bool File::exists(
	const	Buffer&		filename) {

	Uint32 dummy;

	return exists(filename, dummy);

}

void File::del(
	const	Buffer&		filename) {

	if (filename.isEmpty()) {
		throw EmptyArgument();
	}

	Uchar buffer[4096];

	filename.dumpToBuffer(buffer, 4096);

	if (::remove((const char *)buffer) == -1) {
		throw CantSystemRemove(filename + ": " + strerror(errno));
	}

}

// ============================================================================

#if !defined(PLATFORM_WIN32)

BufferArray File::getMountedFileSystemsList() {

	BufferArray list;

	FILE *fp = ::setmntent("/etc/mtab", "r");

	if (!fp) {
		throw CantOpenSystemFile(Buffer("/etc/mtab: ")
		                         + strerror(errno));
	}

	struct mntent	*mnt;

	while ((mnt = ::getmntent(fp))) {
		list += Buffer(mnt->mnt_dir);
	}

	::endmntent(fp);

	if (list.isEmpty()) {
		throw CantReadSystemFile("/etc/mtab");
	}

	return list;

}

File::DirectoryInfo File::getDirectoryInfo(
	const	Buffer&		pathname) {

	DirectoryInfo res;

	// Let's first stat the given pathname ---> must be a valid directory!

	struct stat	pathnameStats;
	Uchar		buffer[4096];

	if (!pathname.dumpToBuffer(buffer, 4096)) {
		throw ArgumentOverflow();
	}

	if (::stat((const char *)buffer, &pathnameStats) == -1) {
		throw CantSystemStat(pathname + ": " + strerror(errno));
	}

	if (!S_ISDIR(pathnameStats.st_mode)) {
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

	for (i = 0 ; i < mounts.size() ; i++) {

		if (!mounts[i].dumpToBuffer(buffer, 4096)) {
			throw InvalidArgument(
					Buffer(mounts[i].length())+"/4096");
		}

		if (::stat((const char *)buffer, &mountStats) == -1) {
			throw CantSystemStat(mounts[i]
			                     + ": "
			                     + strerror(errno));
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

	if (::statfs((const char *)buffer, &fsStats) == -1) {
		throw CantSystemStatfs(res.mountPoint + ": " + strerror(errno));
	}

	res.totalSize = (Uint64)fsStats.f_blocks
	              * (Uint64)fsStats.f_bsize;
	res.freeSpace = (Uint64)fsStats.f_bavail
	              * (Uint64)fsStats.f_bsize;

	return res;

}

Bool File::isCharDevice(
	const	Buffer&		pathname,
		Uint32&		major,
		Uint32&		minor) {

	// Let's first stat the given pathname...

	struct stat	stats;
	Buffer		path = pathname + '\0';

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

void File::open(
	const	Buffer&		FileName,
	const	Uint32		mode,
	const	Uint32		perm) {

	Bool cl = true;

//	msgDbg("... opening " + FileName);

	if (isConnected()) {
		throw AlreadyConnected();
	}

	if (FileName.isEmpty()) {
		throw EmptyArgument();
	}

	if (FileName.length() >= 128) {
		throw ArgumentOverflow();
	}

	if (!mode
	 || (mode & ~(Read | Write | Create | Truncate | Append | Sync))
	 || !(mode & (Read | Write))) {
		throw InvalidArgument();
	}

	Bool	r = ((mode & Read    ) ? true : false),
		w = ((mode & Write   ) ? true : false),
		c = ((mode & Create  ) ? true : false),
		t = ((mode & Truncate) ? true : false),
		a = ((mode & Append  ) ? true : false),
		s = ((mode & Sync    ) ? true : false);

	Uchar ptr[128];

	FileName.dumpToBuffer(ptr, 128);

#if defined(PLATFORM_WIN32)

	int modeval = (r ? (w ? _O_RDWR : _O_RDONLY) : _O_WRONLY)
	            | _O_BINARY;

	if (c) modeval |= _O_CREAT;
	if (t) modeval |= _O_TRUNC;
	if (a) modeval |= _O_APPEND;

	if ((fd = ::_open((const char *)ptr,
	                 modeval,
	                 _S_IREAD | _S_IWRITE)) == -1) {
		throw CantSystemOpen(FileName + ": " + strerror(errno));
	}

#else

	int modeval = (r ? (w ? O_RDWR : O_RDONLY) : O_WRONLY);

	if (c) modeval |= O_CREAT;
	if (t) modeval |= O_TRUNC;
	if (a) modeval |= O_APPEND;
	if (s) modeval |= O_SYNC;

	if ((fd = ::open((const char *)ptr,
	                 modeval,
	                 perm)) == -1) {
		throw CantSystemOpen(FileName + ": " + strerror(errno));
	}

#endif

	setConnected(true);

	closeOnExit  = cl;
	readAccess   = r;
	writeAccess  = w;
	randomAccess = true; // FIXME: not always the case!!!

}

void File::use(
	const	int		descriptor,
	const	Uint32		mode,
	const	Bool		cl) {

	if (isConnected()) {
		throw AlreadyConnected();
	}

	if (!mode || (mode & ~(Read | Write))) {
		throw InvalidArgument("Invalid bits");
	}

	Bool	r = ((mode & Read ) ? true : false),
		w = ((mode & Write) ? true : false);

#if !defined(PLATFORM_WIN32)

	int	rm;	// real mode

	if ((rm = ::fcntl(descriptor, F_GETFL)) == -1) {
		throw InvalidArgument(strerror(errno));
	}

//	if (r && !(rm & O_RDONLY) && !(rm & O_RDWR))
//		throw InvalidMode(Buffer("Read not in real-mode (")
//		                         + rm + ")");

//	if (w && !(rm & O_WRONLY) && !(rm & O_RDWR))
//		throw InvalidMode(Buffer("Write not in real-mode (")
//		                         + rm + ")");

#endif

	fd = descriptor;

	setConnected(true);

	closeOnExit  = cl;
	readAccess   = r;
	writeAccess  = w;
	randomAccess = true; // FIXME

}

void File::close() {

	if (!isConnected()) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32
	if (closeOnExit && ::_close(fd) == -1) {
#else
	if (closeOnExit && ::close(fd) == -1) {
#endif
		throw CantSystemClose(strerror(errno));
	}

	fd = -1;

	setConnected(false);

}

// ============================================================================

Buffer File::getBytes() {

	if (!isConnected()) {
		throw NotConnected();
	}

	if (!readAccess) {
		throw NoReadAccess();
	}

//	const Uint32	toRead = 4096;
	const Uint32	toRead = ( 1 << 16 ); // 64 KBytes
	Uchar		buffer[toRead];

	int count;

#ifdef PLATFORM_WIN32
	if ((count = ::_read(fd, buffer, toRead)) == -1) {
#else
	if ((count = ::read(fd, buffer, toRead)) == -1) {
#endif
		if (errno == EINTR) {
			throw Interrupted();
		}
		throw CantSystemRead(strerror(errno));
	}

	if (!count) {
		throw EndOfStream();
	}

	return Buffer(buffer, count);

}

void File::putBytes(
	const	Buffer&		input) {

	if (fd == -1) {
		throw NotConnected();
	}

	if (!writeAccess) {
		throw NoWriteAccess();
	}

	const Uchar	*buffer = input.getBufferAddr();
	const Uint32	towrite = input.length();

	int count;

#ifdef PLATFORM_WIN32
	if ((count = ::_write(fd, buffer, towrite)) == -1) {
#else
	if ((count = ::write(fd, buffer, towrite)) == -1) {
#endif
		if (errno == EINTR) {
			throw Interrupted();
		}
		throw CantSystemWrite(strerror(errno));
	}

	if ((Uint32)count != towrite) {
		throw Underflow();
	}

}

// ============================================================================

Uint32 File::seek(
	const	Uint32		offset) {

	if (fd == -1) {
		throw NotConnected();
	}

	if (!randomAccess) {
		throw NoRandomAccess();
	}

#ifdef PLATFORM_WIN32
	int o = (int)::_lseek(fd, (off_t)offset, SEEK_SET);
#else
	int o = (int)::lseek(fd, (off_t)offset, SEEK_SET);
#endif

	if (o == -1) {
		throw CantSystemSeek(strerror(errno));
	}

	return (Uint32)o;

}

Uint32 File::tell() const {

	if (fd == -1) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32
	int o = (int)::_tell(fd);
#else
	int o = (int)::lseek(fd, 0, SEEK_CUR);
#endif

	if (o == -1) {
		throw CantSystemTell(strerror(errno));
	}

	return (Uint32)o;

}

Uint32 File::length() const {

	if (fd == -1) {
		throw NotConnected();
	}

	struct stat s;

	if (::fstat(fd, &s) == -1) {
		throw CantSystemFstat(strerror(errno));
	}

	return (Uint32)s.st_size;

}

// ============================================================================

