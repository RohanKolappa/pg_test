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
//	TBFC.IO.SerialPort.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifdef PLATFORM_LINUX
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "TBFC.IO.SerialPort.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

#if 0

SerialPort::SerialPort() {



}

SerialPort::~SerialPort() {

	if (isConnected())
		disconnect();

}

void SerialPort::open(
	const Buffer&	FileName,
	const Uint32	mode,
	const bool	cl) {

	if (isConnected())
		throw AlreadyConnected();

	if (FileName.isEmpty())
		throw EmptyArgument();

	if (FileName.length() >= 128)
		throw ArgumentOverflow();

	if (!mode
	 || (mode & ~(Read | Write))
	 || !(mode & (Read | Write)))
		throw InvalidMode();

	bool	r = ((mode & Read    ) ? true : false),
		w = ((mode & Write   ) ? true : false);

	Uchar ptr[128];

	if (!FileName.dumpToBuffer(ptr, 128))
		throw OutOfMemory();

	int modeval =
		(r ? (w ? O_RDWR
		        : O_RDONLY)
		   : O_WRONLY);

	if ((fd = ::open((const char *)ptr,
	                 modeval,
	                 0644)) == -1)
		throw CantSystemOpen(FileName + ": " + strerror(errno));

	// Serial port setting...

	struct termios	s;

	if (::tcgetattr(fd, &s) == -1)
		throw CantSystemGetAttr(strerror(errno));
#if 0
	if (::cfsetospeed(&s, B9600) == -1)
		throw CantSystemSetSpeed(strerror(errno));

	s.c_cflag |= (CS7 | PARENB);
	s.c_cflag &= ~PARODD;

	if (::tcsetattr(fd, 0, &s) == -1)
		throw CantSystemSetAttr(strerror(errno));
#endif
	setConnected(true, (r ? ReadAccess  : 0)
	                 | (w ? WriteAccess : 0)
	                 , cl);

}

//	=========================================================
//	===              low-level IO methods                 ===
//	=========================================================

Uint32 SerialPort::rawSend(const Uchar *buffer, Uint32 towrite) {

	int	count;

#if !defined(PLATFORM_WIN32)

	if (Thread::self()->shouldStop()) {
		throw Stopped();
	}

	// If the scheduler stops us here, and we are then stopped by
	// another thread, we will block forever...

#endif

	if ((count = ::write(fd, buffer, towrite)) == -1)
		throw CantSystemWrite(strerror(errno));

	return (Uint32)count;

}

Uint32 SerialPort::rawRecv(Uchar *buffer, Uint32 toread) {

	int	count;

#if !defined(PLATFORM_WIN32)

	if (Thread::self()->shouldStop()) {
		throw Stopped();
	}

	// If the scheduler stops us here, and we are then stopped by
	// another thread, we will block forever...

#endif

	if ((count = ::read(fd, buffer, toread)) == -1)
		throw CantSystemRead(strerror(errno));

	return (Uint32)count;

}

Uint32 SerialPort::rawSeek(Int32 offset, SeekDirection direction) {

	int whence = 0;

	switch (direction) {
	case Begin:	whence = SEEK_SET; break;
	case Current:	whence = SEEK_CUR; break;
	case End:	whence = SEEK_END; break;
	}

	int o = (int)lseek(fd, (off_t)offset, whence);

	if (o == -1)
		throw CantSystemSeek(strerror(errno));

	return (Uint32)o;

}

void SerialPort::rawClose() {

	if (::close(fd) == -1)
		throw CantSystemClose(strerror(errno));

}



#endif


