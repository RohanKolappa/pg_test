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
//	TBFC.VFS.FSNode.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#if defined(PLATFORM_WIN32)
#include <windows.h>
#include <io.h>
#include <sys/stat.h>
#define _POSIX_
#include <limits.h>
#else
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "TBFC.VFS.FSDir.h"
#include "TBFC.VFS.FSNode.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

struct TBFC::FSNodeData {

	FSNodeData() : present( false ) {}

//	Buffer		name;
	FSPath		path;
	Bool		present;
	Bool		symlink;
	Bool		readable;
	Bool		writable;
	Bool		executable;

#if defined( PLATFORM_WIN32 )
	struct _stat	stats;
#else
	struct stat	stats;
#endif

};

#if defined( PLATFORM_WIN32 )
#define	FN_ACCESS	::_access
#define	FN_STAT		::_stat
#define	STAT_IFREG	_S_IFREG
#define	STAT_IFMT	_S_IFMT
#define	F_OK		0
#define	X_OK		1
#define	W_OK		2
#define	R_OK		4
#else
#define	FN_ACCESS	::access
#define	FN_STAT		::stat
#define	STAT_IFREG	S_IFREG
#define	STAT_IFMT	S_IFMT
#endif

// ============================================================================

FSNode::FSNode() {

	setObjectName( "TBFC.VFS.FSNode" );
	setDebug( true );

	data = new FSNodeData;

}

FSNode::FSNode(
	const	FSPath&		path ) {

	setObjectName( "TBFC.VFS.FSNode" );
	setDebug( true );

	data = new FSNodeData;
	data->path = path;

	doCheck();

}

FSNode::FSNode(
	const	FSNode&		o )
	: SObject()
	, DObject() {

	setObjectName( "TBFC.VFS.FSNode" );
	setDebug( true );

	data = new FSNodeData;
	*data = *o.data;

}

// ============================================================================

FSNode::~FSNode() {

	delete data;

}

// ============================================================================

FSNode& FSNode::operator = (
	const	FSNode&		o ) {

	*data = *o.data;

	return *this;

}

// ============================================================================

void FSNode::setPath(
	const	FSPath&		path ) {

	data->path = path;

	doCheck();

}

const FSPath& FSNode::getPath() const {

	return data->path;

}

// ============================================================================

Bool FSNode::exists() const {

	return data->present;

}

// ============================================================================

void FSNode::refresh() {

	doCheck();

}

// ============================================================================

Bool FSNode::isReadable() const {

	return ( data->present && data->readable );

}

Bool FSNode::isWritable() const {

	return ( data->present && data->writable );

}

Bool FSNode::isExecutable() const {

	return ( data->present && data->executable );

}

// ============================================================================

Bool FSNode::isFile() const {

	return ( data->present
	      && ( data->stats.st_mode & STAT_IFMT ) == STAT_IFREG );

}

Bool FSNode::isDir() const {

	return ( data->present
	      && ( data->stats.st_mode & STAT_IFMT ) == S_IFDIR );

}

Bool FSNode::isSymLink() const {

	return ( data->present
	      && data->symlink );

}

Buffer FSNode::readLink() const {

	Buffer res;

#if ! defined( PLATFORM_WIN32 )

	if ( ! data->present || ! data->symlink ) {
		return res;
	}

	Buffer b = data->path.toBuffer() + '\0';
	const char *ptr = (const char *)b.getBufferAddr();
	char tmp[PATH_MAX + 1];

	int len = ::readlink( ptr, tmp, PATH_MAX );

	if ( len >= 0 ) {
		res = Buffer( (const Uchar *)tmp, len, true );
	}

#endif

	return res;

}

// ============================================================================

void FSNode::remove() {

	if ( ! data->present ) {
		return;
	}

//	msgWrn( "remove(): deleting \"" + data->path.toBuffer() + "\"!" );

	if ( isDir() ) {
		// Open this directory as a FSDir object...
		FSDir d;
		d.open( data->path );
		// Remove all entries, if any!
		for ( Uint32 i = 0 ; i < d.size() ; i++ ) {
			Buffer e = d[ i ];
			if ( e == FSPath::dot1 || e == FSPath::dot2 ) {
				continue;
			}
			// Recursively destroy this entry!
			FSNode n;
			n.setPath( d.getEntryPath( e ) );
			n.remove();
		}
		d.close();
		// Now, remove this entry too!
		d.open( data->path.getDirPath() );
		d.rmdir( data->path.getLastComponent() );
		d.close();
	}
	else if ( isFile() ) {
		Buffer b = data->path.toBuffer() + '\0';
		const char *ptr = (const char *)b.getBufferAddr();
#if defined( PLATFORM_WIN32 )
		if ( ::_unlink( ptr ) ) {
#else
		if ( ::unlink( ptr ) ) {
#endif
			throw CantRemove( ::strerror( errno ) );
		}
	}
	else {
		throw NotAFile();
	}

	data->present = false;

}

// ============================================================================

Time::Clock FSNode::getLastAccessTime() const {

	return Time::Clock(
		data->present
		? (Uint32)data->stats.st_atime
		: (Uint32)0 );

}

Time::Clock FSNode::getLastModifTime() const {

	return Time::Clock(
		data->present
		? (Uint32)data->stats.st_mtime
		: (Uint32)0 );

}

Time::Clock FSNode::getLastChangeTime() const {

	return Time::Clock(
		data->present
		? (Uint32)data->stats.st_ctime
		: (Uint32)0 );

}

// ============================================================================

void FSNode::doCheck() {

	if ( ! data->path.isValid() ) {
		data->present = false;
		return;
	}

	Buffer b = data->path.toBuffer() + '\0';
	const char *ptr = (const char *)b.getBufferAddr();

	data->present = ( FN_ACCESS( ptr, F_OK ) == 0 );

	if ( ! data->present ) {
		return;
	}

	data->readable = ( FN_ACCESS( ptr, R_OK ) == 0 );
	data->writable = ( FN_ACCESS( ptr, W_OK ) == 0 );
#if defined( PLATFORM_WIN32 )
	data->executable = data->readable;
#else
	data->executable = ( FN_ACCESS( ptr, X_OK ) == 0 );
#endif

#if defined( PLATFORM_WIN32 )
	data->symlink = false;
#else
	if ( ::lstat( ptr, &data->stats ) != 0 ) {
		data->present = false;
		return;
	}

	data->symlink = ( S_ISLNK( data->stats.st_mode ) );

	if ( ! data->symlink ) {
		return;
	}
#endif

	if ( FN_STAT( ptr, &data->stats ) != 0 ) {
		msgWrn( "File \""
			+ data->path.toBuffer()
			+ "\" not found! ("
			+ ::strerror(errno) + ")");
		data->present = false;
	}

}

// ============================================================================

