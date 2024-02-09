// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	File.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#if defined(PLATFORM_WIN32)
#include <io.h>
#else
#include <unistd.h>
#if !defined( PLATFORM_DARWIN )
#include <mntent.h>
#include <sys/vfs.h>
#endif
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "File.h"

// ============================================================================

File::File() {

	fd = -1;

}

File::~File() {

	if ( fd != -1 ) {
#if defined( PLATFORM_WIN32 )
		::_close( fd );
#else
		::close( fd );
#endif
	}

}

// ============================================================================

bool File::exists( const Buffer& filename ) {

	if ( filename.isEmpty() ) {
		return false;
	}

	Buffer tmp = filename + '\0';

	struct stat s;

	return ( ::stat( tmp.getBase(), &s ) != -1 );

}

// ============================================================================

bool File::open( const Buffer& FileName, const int mode ) {

	Buffer tmp = FileName + '\0';

#if defined(PLATFORM_WIN32)

	int modeval =
		( mode == Read
		? _O_RDONLY | _O_BINARY
		: _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC
		);

	if ( ( fd = ::_open( tmp.getBase(),
	                 modeval,
	                 _S_IREAD | _S_IWRITE ) ) == -1 ) {
		return false;
	}

#else

	int modeval =
		( mode == Read
		? O_RDONLY
		: O_WRONLY | O_CREAT | O_TRUNC
		);

	if ( ( fd = ::open( tmp.getBase(),
	                 modeval,
	                 0644 ) ) == -1 ) {
		return false;
	}

#endif

	return true;

}

// ============================================================================

Buffer File::dumpToBuffer() const {

	Buffer output;

	for (;;) {
		Buffer temp;
		temp = getBytes();
		if ( temp.isEmpty() ) {
			break;
		}
		output += temp;
	}

	return output;

}

// ============================================================================

Buffer File::getBytes() const {

	const int	toRead = ( 1 << 16 ); // 64 KBytes
	char		buffer[toRead];

	int count;

#if defined( PLATFORM_WIN32 )
	if ( ( count = ::_read( fd, buffer, toRead ) ) < 1 ) {
#else
	if ( ( count = ::read( fd, buffer, toRead ) ) < 1 ) {
#endif
		return Buffer();
	}

	return Buffer( buffer, count );

}

void File::putBytes( const Buffer& input ) const {

#if defined( PLATFORM_WIN32 )
	::_write( fd, input.getBase(), input.length() );
#else
	// Stupid 'if' for "error: ignoring return value of 'ssize_t write(int,
	// const void*, size_t)', declared with attribute warn_unused_result"
	if ( ::write( fd, input.getBase(), input.length() ) ) {
	}
#endif

}

// ============================================================================

