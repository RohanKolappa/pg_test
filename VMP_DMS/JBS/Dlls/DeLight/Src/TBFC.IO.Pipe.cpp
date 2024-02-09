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
//	TBFC.IO.Pipe.cpp
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
#include <string.h>
#if defined(PLATFORM_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "TBFC.IO.Pipe.h"

// ============================================================================

using namespace TBFC;
using namespace IO;

// ============================================================================

#if 0

Pipe::Pipe() {

	fd0 = -1;
	fd1 = -1;

}

Pipe::~Pipe() {

	if (isConnected())
		disconnect();

}

void Pipe::create() {

	if (isConnected())
		throw AlreadyCreated();

	int	filedes[2];

#if defined(PLATFORM_WIN32)
	if (::_pipe(filedes, 4096, O_BINARY) == -1)
#else
	if (::pipe(filedes) == -1)
#endif
		throw CantSystemCreate(strerror(errno));

	fd0 = filedes[0];
	fd1 = filedes[1];

#if !defined(PLATFORM_WIN32)

	// Now, let's make our fds non-blocking (to prevent deadlocks in
	// reads or writes).

	if (::fcntl(fd0, F_SETFL, O_NONBLOCK) == -1) {
		(void)::close(fd0);
		(void)::close(fd1);
		throw CantSetNonBlockingReceives(strerror(errno));
	}

	if (::fcntl(fd1, F_SETFL, O_NONBLOCK) == -1) {
		(void)::close(fd0);
		(void)::close(fd1);
		throw CantSetNonBlockingSends(strerror(errno));
	}

#endif

	setConnected(
		true,
		ReadAccess | WriteAccess,
		true);

}

void Pipe::disconnectSend() {

	if (fd1 == -1)
		throw NotConnected();

	if (::close(fd1) == -1) {
		fd1 = -1;
		throw CantRawClose(strerror(errno));
	}

	fd1 = -1;

	if (fd0 == -1)
		setConnected(false);

}

void Pipe::disconnectRecv() {

	if (fd0 == -1)
		throw NotConnected();

	if (::close(fd0) == -1) {
		fd0 = -1;
		throw CantRawClose(strerror(errno));
	}

	fd0 = -1;

	if (fd1 == -1)
		setConnected(false);

}

//	=========================================================
//	===              low-level IO methods                 ===
//	=========================================================

Uint32 Pipe::rawSend(const Uchar *buffer, Uint32 towrite) {

	Int32	count;

	if (Thread::self()->shouldStop()) {
		throw Stopped();
	}

	// If the scheduler stops us here, and we are then stopped by
	// another thread, we will block forever...

	if ((count = ::write(fd1, buffer, towrite)) == -1)
		throw CantSystemWrite(strerror(errno));

	return (Uint32)count;

}

Uint32 Pipe::rawRecv(Uchar *buffer, Uint32 toread) {

	Int32	count;

	if (Thread::self()->shouldStop()) {
		throw Stopped();
	}

	// If the scheduler stops us here, and we are then stopped by
	// another thread, we will block forever...

	if ((count = ::read(fd0, buffer, toread)) == -1)
		throw CantSystemRead(strerror(errno));

	return (Uint32)count;

}

Uint32 Pipe::rawSeek(Int32, SeekDirection) {

	throw NotImplemented();

	return 0;

}

void Pipe::rawClose() {

	if (fd0 != -1) {
		if (::close(fd0) == -1) {
			fd0 = -1;
			(void)::close(fd1);
			fd1 = -1;
			throw CantSystemClose();
		}
		fd0 = -1;
	}

	if (fd1 != -1) {
		if (::close(fd1) == -1) {
			fd1 = -1;
			throw CantSystemClose();
		}
		fd1 = -1;
	}

}

#endif

// ============================================================================

