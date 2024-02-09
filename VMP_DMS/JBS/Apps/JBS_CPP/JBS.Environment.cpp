// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "JBS".
//
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	JBS.Environment.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <stdlib.h>
#else
#include <unistd.h>
#endif

#include "BFC.Base.BufferParser.h"

#include "JBS.Context.h"
#include "JBS.Environment.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( Environment )

// ============================================================================

Environment::Environment() {

	setObjectName( "JBS.Environment" );
	setDebug( true );

}

Environment::~Environment() {



}

// ============================================================================

void Environment::init() {

	// Parse all environment variables...

	for ( char **tmp = environ ; tmp && *tmp ; tmp++ ) {
		Buffer b( *tmp );
//		msgDbg( "Environment: " + b );
		Uint32 pos = b.findFwd( '=' );
		if ( ! pos || pos >= b.length() ) {
			throw InternalError(
				"Invalid environment variable: \""
				+ b
				+ "\"!" );
		}
		else if ( pos == b.length() - 1 ) {
			// Empty variable...
			variables.add( b( 0, pos ), "" );
		}
		else {
			variables.add( b( 0, pos ), b( pos + 1 ) );
		}
	}

	// Fix Qt stuff...

	static const Buffer qtDirName = "QTDIR";
	static const Buffer qtDirBinName = "QTDIR_BIN";
	static const Buffer qtDirLibName = "QTDIR_LIB";
	static const Buffer qtDirIncName = "QTDIR_INC";

	if ( variables.contains( qtDirName ) ) {
		FSPath qtDir = variables[ qtDirName ];
		FSPath qtDirBin = qtDir + "bin";
		FSPath qtDirLib = qtDir + "lib";
		FSPath qtDirInc = qtDir + "include";
		if ( variables.contains( qtDirBinName ) ) {
			qtDirBin = variables[ qtDirBinName ];
		}
		if ( variables.contains( qtDirLibName ) ) {
			qtDirLib = variables[ qtDirLibName ];
		}
		if ( variables.contains( qtDirIncName ) ) {
			qtDirInc = variables[ qtDirIncName ];
		}
		variables.add( "JBS-QT-DIR", qtDir.toBuffer() );
		variables.add( "JBS-QT-DIR-BIN", qtDirBin.toBuffer() );
		variables.add( "JBS-QT-DIR-LIB", qtDirLib.toBuffer() );
		variables.add( "JBS-QT-DIR-INC", qtDirInc.toBuffer() );
	}
	else {
		msgWrn( qtDirName + ": no such environment variable!" );
	}

}

// ============================================================================

Bool Environment::exists(
	const	Buffer&		name ) const {

	return variables.contains( name );

}

Buffer Environment::get(
	const	Buffer&		name ) const {

	return variables[ name ];

}

FSPath Environment::getPath(
	const	Buffer&		name ) const {

	FSPath res;

	res.set( variables[ name ] );

	return res;

}

// ============================================================================

Buffer Environment::expandLine(
	const	Buffer&		str ) const {

	Buffer	res;

	for ( Uint32 beg = 0 ; beg < str.length() ; ) {
		// Expand the part of 'str', starting at pos 'beg', and append
		// the result to 'res'.
		Uint32 dollarPos = str.findFwd( '$', beg );
		if ( dollarPos >= str.length() ) {
			res += str( beg );
			break;
		}
		// We found a '$' sign! It should be followed by a '('!
		if ( dollarPos == str.length() - 1 ) {
			// Last char ?!
			throw InvalidArgument( "Can't expand: " + str );
		}
		Uint32 colBegPos = dollarPos + 1;
		Uint32 colEndPos = str.findClosingChar( colBegPos );
		if ( colEndPos == 0 ) {
			throw InvalidArgument( "Can't expand: " + str );
		}
		Buffer varName = expandLine( str( colBegPos + 1, colEndPos - colBegPos - 1 ) );
		if ( ! variables.contains( varName ) ) {
			throw InvalidArgument( "No such variable: " + varName );
		}
		res += str( beg, dollarPos - beg );
		res += variables[ varName ];
		beg = colEndPos + 1;
	}

	return res;

}

// ============================================================================

