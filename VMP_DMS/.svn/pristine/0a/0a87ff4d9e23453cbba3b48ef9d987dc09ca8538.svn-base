// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "TBFC::VFS".
//
// "TBFC::VFS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "TBFC::VFS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::VFS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	TBFC.VFS.FSDir.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#if defined( PLATFORM_WIN32 )
#	include <windows.h>
#	include <io.h>
#	include <direct.h>
#	define _POSIX_
#	include <limits.h>
#	include <sys/stat.h>
#else
#	include <dirent.h>
#	include <errno.h>
#	include <fcntl.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
#	include <unistd.h>
#	include <sys/stat.h>
#	include <sys/types.h>
#endif

#include "TBFC.VFS.FSDir.h"
#include "TBFC.VFS.FSNode.h"

// ============================================================================

using namespace std;
using namespace TBFC;

// ============================================================================

FSDir::FSDir() {

	setObjectName("TBFC.VFS.FSDir");
	setDebug(true);

}

FSDir::~FSDir() {



}

// ============================================================================

void FSDir::open(
	const	FSPath&		newPath ) {

	open( newPath.toBuffer() );

}

void FSDir::open(
	const	Buffer&		newPath ) {

	if ( isOpen() ) {
		throw AlreadyOpen();
	}

	if ( newPath.isEmpty() ) {
		throw InvalidArgument( "Empty path!" );
	}

	try {
		path.set( newPath );
	}
	catch ( Exception& e ) {
		throw InvalidArgument( e );
	}

	try {
		refresh();
	}
	catch ( Exception& e ) {
		throw CantRefreshDir( e );
	}

}

Bool FSDir::isOpen() const {

	return ( path.isValid() );

}

void FSDir::refresh() {

	if ( ! isOpen() ) {
		throw NotOpen();
	}

	content.kill();

#if defined(PLATFORM_WIN32)

	Buffer dirName = path.toBuffer();

//	msgDbg( "FSDir::refresh(\"" + dirName + "\")" );

	if ( dirName[dirName.length()-1] != '/' ) {
		dirName += '/';
	}

	dirName += '*';

	Buffer b = dirName + '\0';
	const char *p = (const char *)b.getBufferAddr();

	HANDLE			hndl;
	WIN32_FIND_DATA		data;

	if ( ( hndl = FindFirstFile( p, &data ) ) == INVALID_HANDLE_VALUE ) {
		throw CantOpenDir( ::strerror( GetLastError() )
			+ Buffer( " (file: " ) + dirName + ")" );
	}

	content += Buffer( data.cFileName );

	while ( FindNextFile( hndl, &data ) ) {
		content += Buffer( data.cFileName );
	}

	FindClose( hndl );

#else

	DIR		*dir;
	Buffer		dirName = path.toBuffer() + '\0';
	const char	*dirPtr = (const char *)dirName.getBufferAddr();

	dir = ::opendir( dirPtr );

	if ( ! dir ) {
		throw CantOpenDir( ::strerror( errno ) );
	}

	struct dirent	*file;

	while ( ( file = ::readdir( dir ) ) ) {
		content += Buffer( file->d_name );
	}

	::closedir( dir );

#endif

	// Now, sort the array of entries...

	for ( Uint32 i = 0 ; i+1 < content.size() ; ) {
		if ( content[ i ] > content[ i+1 ] ) {
			Buffer tmp = content[ i ];
			content[ i ] = content[ i+1 ];
			content[ i+1 ] = tmp;
			if ( i ) {
				i--;
				continue;
			}
		}
		i++;
	}

}

void FSDir::close() {

	path.kill();

}

// ============================================================================

Bool FSDir::operator == (
	const	FSDir&		o ) const {

	return ( path.toBuffer() == o.path.toBuffer() );

}

Bool FSDir::operator != (
	const	FSDir&		o ) const {

	return ( path.toBuffer() != o.path.toBuffer() );

}

// ============================================================================

const FSPath& FSDir::getPath() const {

	return path;

}

FSPath FSDir::getAbsPath() const {

	FSPath res;

	if ( path.isRelative() ) {
		res.set( getCurrentDir() );
		res.append( path );
	}
	else {
		res = path;
	}

	return res;

}

FSPath FSDir::getEntryPath(
	const	Buffer&		entryName ) const {

	if ( entryName.isEmpty() ) {
		throw InvalidArgument( "entryName empy!" );
	}

	FSPath res;

	res = path;
	res.append( entryName );

	return res;

}

// ============================================================================

void FSDir::cd(
	const	Buffer&		dirName ) {

//	msgDbg("cd(" + dirName + ")");
//	msgDbg("... old: "
//		+ path.toBuffer() + " ("
//		+ Buffer(path.toBuffer().length()) + " bytes)");

	if ( dirName.isEmpty() ) {
		throw InvalidArgument("dirName is empty!");
	}

	if ( ! isOpen() ) {
		throw NotOpen();
	}

	FSPath oldPath = path;

	path.append( dirName );

//	msgDbg("... new: " + path.toBuffer() + " ("
//		+ Buffer(path.toBuffer().length()) + " bytes)");

	try {
		refresh();
	}
	catch (Exception& e) {
		path = oldPath;
		throw CantRefreshDir( e );
	}

}

// ============================================================================

Uint32 FSDir::size() const {

	return content.size();

}

const Buffer& FSDir::operator [] (
	const	Uint32		index ) const {

	return content[index];

}

const BufferArray& FSDir::getAllEntries() const {

	return content;

}

// ============================================================================

void FSDir::create(
	const	Buffer&		fileName ) {

	FSPath tmpPath = path + fileName;

	Buffer b = tmpPath.toBuffer() + '\0';
	const char *p = (const char *)b.getBufferAddr();

	int fd;

#if defined( PLATFORM_WIN32 )
	fd = ::_creat( p, _S_IREAD | _S_IWRITE );
#else
	fd = ::creat( p, 0666 );
#endif
	if ( fd == -1 ) {
		throw CantTouch( ::strerror( errno ) );
	}

#if defined( PLATFORM_WIN32 )
	::_close( fd );
#else
	::close( fd );
#endif

	refresh();

}

void FSDir::mkdir(
	const	Buffer&		dirName ) {

	FSPath tmpPath = path + dirName;

	Buffer b = tmpPath.toBuffer() + '\0';
	const char *p = (const char *)b.getBufferAddr();

#if defined(PLATFORM_WIN32)
	if ( ::_mkdir( p ) ) {
#else
	if ( ::mkdir( p, 0777 ) ) {
#endif
		throw CantMkdir( ::strerror( errno ) );
	}

	refresh();

}

void FSDir::rmdir(
	const	Buffer&		dirName ) {

	FSPath tmpPath = path + dirName;

	Buffer b = tmpPath.toBuffer() + '\0';
	const char *p = (const char *)b.getBufferAddr();

#if defined(PLATFORM_WIN32)
	if ( ::_rmdir( p ) ) {
#else
	if ( ::rmdir( p ) ) {
#endif
		throw CantRmdir( ::strerror( errno ) );
	}

	refresh();

}

void FSDir::rm(
	const	Buffer&		name ) {

	FSPath tmpPath = path;
	tmpPath.append(name);

	Buffer b = tmpPath.toBuffer() + '\0';
	const char *p = (const char *)b.getBufferAddr();

#if defined( PLATFORM_WIN32 )
	if ( ::_unlink( p ) == -1 ) {
#else
	if ( ::unlink( p ) == -1 ) {
#endif
		throw CantUnlink( ::strerror(errno) );
	}

	refresh();

}

void FSDir::mv(
	const	Buffer&		oldName,
	const	Buffer&		newName) {

	FSPath tmpPath1 = path;
	FSPath tmpPath2 = path;
	tmpPath1.append(oldName);
	tmpPath2.append(newName);

	Buffer fn1 = tmpPath1.toBuffer() + '\0';
	Buffer fn2 = tmpPath2.toBuffer() + '\0';

	const char *p1 = (const char *)fn1.getBufferAddr();
	const char *p2 = (const char *)fn2.getBufferAddr();

	if ( ::rename( p1, p2 ) ) {
		throw CantRename( strerror( errno ) );
	}

	refresh();

}

// ============================================================================

void FSDir::setCurrentDir(
	const	FSPath&		path ) {

	setCurrentDir( path.toBuffer() );

}

void FSDir::setCurrentDir(
	const	Buffer&		path ) {

	Buffer b = path + '\0';
	const char *p = (const char *)b.getBufferAddr();

#if defined( PLATFORM_WIN32 )
	if ( ::_chdir( p ) == -1 ) {
#else
	if ( ::chdir( p ) == -1 ) {
#endif
		throw CantChdir( strerror(errno) );
	}

}

// ============================================================================

#ifndef PATH_MAX
#ifdef _MAX_PATH
#define PATH_MAX _MAX_PATH
#endif
#endif

Buffer FSDir::getCurrentDir() {

	Buffer res;

	char currentName[PATH_MAX+1];

#if defined( PLATFORM_WIN32 )
	if ( ::_getcwd( currentName, PATH_MAX ) ) {
#else
	if ( ::getcwd( currentName, PATH_MAX ) ) {
#endif
		res = Buffer( currentName );
	}

	return FSPath::slashify( res );

}

Buffer FSDir::getHomeDir() {

	Buffer d;

	d = Buffer( ::getenv( "HOME" ) );

#if defined(PLATFORM_WIN32)
	if ( d.isEmpty() ) {
		d = Buffer( ::getenv( "HOMEDRIVE" ) )
		  + Buffer( ::getenv( "HOMEPATH" ) );
	}
#endif

	if ( d.isEmpty() ) {
		d = getRootDir();
	}

#if defined(PLATFORM_WIN32)
	if ( d.isEmpty() ) {
		d = Buffer( ::getenv( "SYSTEMDRIVE" ) ) + '\\';
	}
#endif

	return FSPath::slashify( d );

}

Buffer FSDir::getRootDir() {

	Buffer d;

#if defined(PLATFORM_WIN32)
	d = Buffer( ::getenv( "SYSTEMDRIVE" ) ) + '\\';
#else
	d = Buffer( "/" );
#endif

	return FSPath::slashify( d );

}

// ============================================================================

BufferArray FSDir::getDrives() {

	BufferArray res;

#if defined(PLATFORM_WIN32)

	DWORD bitMask = GetLogicalDrives() & ( ( 1 << 26 ) - 1 );
	Buffer name;
	Uchar letter;

	for ( Uint32 i = 0 ; i < 26 ; i++ ) {
		if ( bitMask & ( 1 << i ) ) {
			letter = (Uchar)( 'A' + i );
			name = letter;
			name += ":/";
			res += name;
		}
	}

#else

	res += getRootDir();

#endif

	return res;

}

// ============================================================================

