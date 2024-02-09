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
//	TBFC.VFS.FSPath.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "TBFC.Base.BufferParser.h"
#include "TBFC.VFS.FSDir.h"
#include "TBFC.VFS.FSPath.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

const Buffer FSPath::dot1 = ".";
const Buffer FSPath::dot2 = "..";

// ============================================================================

FSPath::FSPath() {

	setObjectName("TBFC.VFS.FSPath");
	setDebug(true);

	valid = false;

}

FSPath::FSPath(
	const	Buffer&		path ) {

	setObjectName("TBFC.VFS.FSPath");
	setDebug(true);

	valid = false;

	try {
		set( path );
	}
	catch ( Exception& e ) {
		msgExc( e, "FSPath(): invalid path! (" + path + ")" );
	}

}

FSPath::FSPath(
	const	FSPath&		o )
	: SObject()
	, DObject() {

	setObjectName("TBFC.VFS.FSPath");
	setDebug(true);

	if ( ( valid = o.valid ) ) {
		relative = o.relative;
#if defined(PLATFORM_WIN32)
		if ( ! relative ) {
			driveLetter = o.driveLetter;
			serverName = o.serverName;
			shareName = o.shareName;
		}
#endif
		components = o.components;
		fullPath = o.fullPath;
	}

}

FSPath::~FSPath() {



}

// ============================================================================

FSPath& FSPath::operator = (
	const	FSPath&		o ) {

	if ( this != &o ) {
		if ( ( valid = o.valid ) ) {
			relative = o.relative;
#if defined(PLATFORM_WIN32)
			if ( ! relative ) {
				driveLetter = o.driveLetter;
				serverName = o.serverName;
				shareName = o.shareName;
			}
#endif
			components = o.components;
			fullPath = o.fullPath;
		}
	}

	return *this;

}

// ============================================================================

Bool FSPath::operator == (
	const	FSPath&		other ) const {

	return ( isValid()
	      && other.isValid()
	      && components.size() == other.components.size()
	      && fullPath == other.fullPath );

}

// ============================================================================

FSPath FSPath::operator + (
	const	Buffer&		component ) const {

	FSPath res = *this;

	res.append( component );

	return res;

}

FSPath FSPath::operator + (
	const	FSPath&		subpath ) const {

	FSPath res = *this;

	res.append( subpath );

	return res;

}

// ============================================================================

void FSPath::set(
	const	Buffer&		path ) {

//	msgDbg("set(\"" + path + "\")");

	// Sanity checks...

	if ( path.isEmpty() ) {
		throw InvalidArgument();
	}

	// Convert all backslashes to slashes...

	Buffer current = slashify( path );

	// Split path into tokens separated by slashes...

	components = BufferParser::tokenize( current, '/' );

	if ( components.isEmpty() ) {
		// We received only slashes ---> root dir!
		valid = true;
		relative = false;
#if defined(PLATFORM_WIN32)
		driveLetter = 'C';
		serverName.kill();
		shareName.kill();
		fullPath = "C:/";
#else
		fullPath = "/";
#endif
		return;
	}

#if defined(PLATFORM_WIN32)

	// Handle share names and drive names...

	if ( current.length() >= 3
	  && current[0] == '/'
	  && current[1] == '/'
	  && current[2] != '/' ) {
		if ( components.size() < 2 ) {
			throw InvalidArgument("Missing share name!");
		}
		valid = true;
		relative = false;
		driveLetter = '\0';
		serverName = components[0];
		shareName = components[1];
		components.pop(0);
		components.pop(0);
		compactComponents();
		computeFullPath();
		return;
	}

	if ( isLetter( current[0] )
	  && components[0].length() == 2
	  && current[1] == ':' ) {
		valid = true;
		relative = false;
		driveLetter = toUpper(current[0]);
		serverName.kill();
		shareName.kill();
		components.pop(0);
		compactComponents();
		computeFullPath();
		return;
	}

#endif

	valid = true;
	relative = ( current[0] != '/' );

	compactComponents();

//#if defined(PLATFORM_WIN32)
//	if ( ! relative ) {
//		if ( current dir on share )
//		else get current drive letter...
//	}
//#endif

	computeFullPath();

}

// ============================================================================

void FSPath::append(
	const	Buffer&		cmpt) {

	if ( ! isValid() ) {
		throw NotValid( "In append(" + cmpt + ")" );
	}

	if ( cmpt.isEmpty() ) {
		throw InvalidArgument("Component is empty!");
	}

	components += cmpt;

	compactComponents();
	computeFullPath();

}

void FSPath::append(
	const	FSPath&		path) {

	if ( ! path.isValid() ) {
		throw InvalidArgument("Path is not valid!");
	}

	if ( ! isValid() ) {
		throw NotValid( "In append(" + path.toBuffer() + ")" );
	}

	if ( path.isAbsolute() ) {
		throw InvalidArgument("Path is absolute!");
	}

	components += path.components;

	compactComponents();
	computeFullPath();

}

void FSPath::removeLast() {

	if ( ! isValid() ) {
		throw NotValid( "In removeLast()" );
	}

	if ( components.size() ) {
		components.pop( components.size() - 1 );
	}

	computeFullPath();

}

Bool FSPath::isValid() const {

	return valid;

}

Bool FSPath::isRelative() const {

	return relative;

}

Bool FSPath::isAbsolute() const {

	return ( ! relative );

}

// ============================================================================

void FSPath::forceAbsolute() {

	if ( ! isValid() ) {
		throw NotValid( "In forceAbsolute()" );
	}

	if ( ! relative ) {
		return;
	}

	FSPath tmp = *this;

	set( FSDir::getCurrentDir() );

	append( tmp );

}

// ============================================================================

Uint32 FSPath::size() const {

	return components.size();

}

Bool FSPath::isParentDirOf(
	const	FSPath&		source ) const {

	FSPath absSource = source;

	absSource.forceAbsolute();

	FSPath absThis = *this;

	absThis.forceAbsolute();

	if ( absSource.size() <= absThis.size() ) {
		return false;
	}

	return ( absSource.getFirst( absThis.size() ) == absThis );

}

const Buffer& FSPath::operator [] (
	const	Uint32		index) const {

	return components[index];

}

const Buffer& FSPath::getLastComponent() const {

	if ( ! isValid() ) {
		throw NotValid( "In getLastComponent()" );
	}

	return ( size() != 0
		? components[ components.size() - 1 ]
		: dot1 );

}

void FSPath::replaceLastComponent(
	const	Buffer&		p ) {

	if ( ! isValid() ) {
		throw NotValid( "In replaceLastComponent( " + p + " );" );
	}

	if ( size() == 0 ) {
		components += p;
	}
	else {
		components[ components.size() - 1 ] = p;
	}

	computeFullPath();

}

// ============================================================================

FSPath FSPath::getDirPath() const {

	if ( ! isValid() ) {
		throw NotValid( "In getDirPath()" );
	}

	FSPath c( *this );

	c.removeLast();

	return c;

}

FSPath FSPath::getFirst(
	const	Uint32		nbr ) const {

	if ( ! isValid() ) {
		throw NotValid();
	}

	if ( nbr > components.size() ) {
		throw InvalidArgument();
	}

	FSPath p;

	p.valid = true;
	p.relative = relative;
#if defined( PLATFORM_WIN32 )
	if ( ! relative ) {
		p.driveLetter = driveLetter;
		p.serverName = serverName;
		p.shareName = shareName;
	}
#endif
	p.components.resize( nbr );
	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		p.components[ i ] = components[ i ];
	}

	p.computeFullPath();

	return p;

}

FSPath FSPath::getLast(
	const	Uint32		nbr ) const {

	if ( ! isValid() ) {
		throw NotValid();
	}

	if ( nbr > components.size() ) {
		throw InvalidArgument();
	}

	FSPath p;

	p.valid = true;
	p.relative = true;
	p.components.resize( nbr );
	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		p.components[ i ] = components[ components.size() - nbr + i ];
	}

	p.computeFullPath();

	return p;

}

Buffer FSPath::getBaseName(
	const	Buffer&		expectedExtension ) const {

	if ( ! isValid() ) {
		throw NotValid( "In getBaseName()" );
	}

	Buffer res = getLastComponent();

	if ( ! res.endsWith( expectedExtension ) ) {
		throw InvalidArgument( "Not the same extension!" );
	}

	return res( 0, res.length() - expectedExtension.length() );

}

Buffer FSPath::getBaseName() const {

	if ( ! isValid() ) {
		throw NotValid( "In getBaseName()" );
	}

	Buffer res = getLastComponent();
	Uint32 pos = res.findRev( '.' );

	if ( pos == (Uint32)-1 || pos == 0 ) {
		return res;
	}

	return res( 0, pos );

}

// ============================================================================

const Buffer& FSPath::toBuffer() const {

	if ( ! isValid() ) {
		throw NotValid( "In toBuffer()" );
	}

	return fullPath;

}

// ============================================================================

// KAE not tested yet
#if 0 
FSPath FSPath::getRelative(const FSPath& source) const {
	if ( ! isValid() ) {
		throw NotValid();
	}

	//msgDbg("Entering " + toBuffer() + " getRelative(" + source.toBuffer() + ")");

	FSPath tmp(*this);
	tmp.forceAbsolute();
	FSPath absSource(source);
	absSource.forceAbsolute();

	Uint32 max = tmp.size();
	if (max > absSource.size())
		max = absSource.size();

	Uint32 nrEqualComponents = 0;
	while (nrEqualComponents < max) {
		if (tmp.getFirst(nrEqualComponents+1) == absSource.getFirst(nrEqualComponents+1))
			++nrEqualComponents;
		else
			break;
	}

	Buffer p;
	Uint32 i;
	for (i=0; i<absSource.size()-nrEqualComponents; ++i)
		p += dot2 + "/";
	if (nrEqualComponents)
		tmp = tmp.getLast(tmp.size()-nrEqualComponents);

	FSPath result;
	if (!p.isEmpty())
		result.set(p);
	if (result.isValid())
		result.append(tmp);
	else
		result = tmp;
	return result;
}
#endif // KAE not tested yet

// ============================================================================

void FSPath::kill() {

	valid = false;

}

// ============================================================================

Uchar FSPath::getSeparator() {

#if defined(PLATFORM_WIN32)
	return '\\';
#else
	return '/';
#endif

}

// ============================================================================

Buffer FSPath::slashify(
	const	Buffer&		path) {

	Buffer current = path;

	for ( Uint32 i = 0 ; i < current.length() ; i++ ) {
		if ( current[i] == '\\' ) {
			current[i] = '/';
		}
	}

	return current;

}

// ============================================================================

Bool FSPath::isLetter(
	const	Uchar		c ) {

	return ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) );

}

Uchar FSPath::toUpper(
	const	Uchar		c ) {

	return (Uchar)( ( c >= 'a' && c <= 'z' )
	       ? ( c - 'a' + 'A' )
	       : c );

}

// ============================================================================

void FSPath::compactComponents() {

	for ( Uint32 i = 0 ; i < components.size() ; ) {

		if ( components[i] == dot1 ) {
			components.pop( i );
			continue;
		}
		if ( components[i] != dot2 ) {
			i++;
			continue;
		}
		if ( i && components[i-1] != dot2 ) {
			i--;
			components.pop( i );
			components.pop( i );
			continue;
		}
		if ( ! i && isAbsolute() ) {
			components.pop( 0 );
			continue;
		}
		i++;

	}

}

void FSPath::computeFullPath() {

	fullPath.kill();

	if ( isRelative() ) {
		if ( components.size() ) {
			for ( Uint32 i = 0 ; i < components.size() ; i++ ) {
				if ( i ) {
					fullPath += '/';
				}
				fullPath += components[i];
			}
		}
		else {
			fullPath = dot1;
		}
	}
	else {
#if defined(PLATFORM_WIN32)
		if ( ! serverName.isEmpty() ) {
			fullPath = "//";
			fullPath += serverName;
			fullPath += '/';
			fullPath += shareName;
		}
		else {
			fullPath = driveLetter;
			fullPath += ':';
		}
#endif
		fullPath += '/';
		for ( Uint32 i = 0 ; i < components.size() ; i++ ) {
			if ( i ) {
				fullPath += '/';
			}
			fullPath += components[i];
		}
	}

}

// ============================================================================

