// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::VFS".
//
// "BFC::VFS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::VFS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::VFS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.VFS.FSGlobber.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <io.h>
#else
#include <errno.h>
#include <glob.h>
#include <string.h>
#endif

//#include "BFC.Base.BufferIO.h"
#include "BFC.VFS.FSGlobber.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL( FSGlobber )

// ============================================================================

FSGlobber::FSGlobber() {



}

FSGlobber::~FSGlobber() {



}

// ============================================================================

void FSGlobber::clearMatches() {

	matches.kill();

}

Uint32 FSGlobber::findMatches(
	const	Buffer&		pattern ) {

	return findMatches( pattern, matches );

}

// ============================================================================

Uint32 FSGlobber::findMatches(
	const	Buffer&		pattern,
		FSPathList&	matches ) {

#if defined( PLATFORM_WIN32 )

//	msgDbg( "findMatches(): pattern == \"" + pattern + "\"" );

	FSPath tempPath = pattern;

	return findMatches( tempPath, 0, matches );

#else

	glob_t	g;
	Buffer	b = pattern + '\0';
	Uint32	i = matches.size();

	int res = ::glob(
		(const char *)b.getBufferAddr(),
#if defined( GLOB_PERIOD )
		GLOB_PERIOD,
#else
		0,
#endif
		0,
		&g );

	if ( res ) {
		if ( res == GLOB_NOMATCH ) {
			return 0;
		}
		throw CantFindMatches( ::strerror( res ) );
	}

	int    argc = g.gl_pathc;
	char **argv = g.gl_pathv;

	FSPath path;

	while ( argc-- ) {
		Buffer cmpt = *argv++;
//		msgDbg( cmpt );
		path = cmpt;
//		msgDbg( path.toBuffer() );
		if ( path.getLastComponent() != FSPath::dot1
		  && path.getLastComponent() != FSPath::dot2 ) {
			matches += path;
		}
	}

	globfree( &g );

	return ( matches.size() - i );

#endif

}

Uint32 FSGlobber::getNumMatches() const {

	return matches.size();

}

// ============================================================================

#if defined( PLATFORM_WIN32 )

Uint32 FSGlobber::findMatches(
	const	FSPath&		pattern,
	const	Uint32		_startAt,
		FSPathList&	matches ) {

//	msgDbg(
//		"findMatches(): pattern == \""
//		+ pattern.toBuffer()
//		+ "\", start at: "
//		+ Buffer( startAt ) );

	//remove non globbing items at start...
	Uint32 startAt = _startAt;
	for (; startAt<pattern.size()-1; ++startAt) {
		if (pattern[startAt].contains('*') || pattern[startAt].contains('?'))
			break;
	}

	FSPath		tempPrefix = pattern.getFirst( startAt );
	FSPath		tempPattern = pattern.getFirst( startAt + 1 );
	FSPathList	tempList;

	struct _finddata_t fileinfo;
	Buffer b = tempPattern.toBuffer() + '\0';
	Uint32 in = matches.size();

	long handle = (long)_findfirst(
			(const char *)b.getBufferAddr(),
			&fileinfo );

	if ( handle == -1 ) {
		return 0;
	}

	FSPath path = tempPrefix + Buffer( fileinfo.name );

	if ( path.getLastComponent() != FSPath::dot1
	  && path.getLastComponent() != FSPath::dot2
	  && path.size() >= tempPattern.size() ) {
//		msgDbg(
//			"... match: " + path.toBuffer() );
		tempList += path;
	}

	while ( _findnext( handle, &fileinfo ) == 0 ) {
		path = tempPrefix + Buffer( fileinfo.name );
		if ( path.getLastComponent() != FSPath::dot1
		  && path.getLastComponent() != FSPath::dot2
		  && path.size() >= tempPattern.size() ) {
//			msgDbg(
//				"... match: " + path.toBuffer() );
			tempList += path;
		}
	}

	_findclose( handle );

	if ( startAt == pattern.size() - 1 ) {
		matches += tempList;
	}
	else {
		FSPath suffix = pattern.getLast( pattern.size() - startAt - 1 );
		for ( FSPathList::Iterator it = tempList.firstElt() ; it ; it++ ) {
			FSPath tempPattern = *it + suffix;
			findMatches( tempPattern, startAt + 1, matches );
		}
	}

	return ( matches.size() - in );

}

#endif

// ============================================================================

