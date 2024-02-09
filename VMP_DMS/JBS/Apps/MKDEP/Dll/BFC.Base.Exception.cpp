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
//	BFC.Exception.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.System.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Exception::Exception() {

	className = 0;
	errorName = 0;
	adMessage = 0;
	prevExcep = 0;
	whatCache = 0;

}

Exception::Exception(
	const	Char		*cn,
	const	Char		*en,
	const	Char		*am,
	const	Exception	*pe ) {

	className = strDup( cn );
	errorName = strDup( en );
	adMessage = strDup( am );
	prevExcep = excDup( pe );
	whatCache = 0;

}

Exception::Exception(
	const	Char		*cn,
	const	Char		*en,
	const	Buffer&		am,
	const	Exception	*pe ) {

	className = strDup( cn );
	errorName = strDup( en );
	adMessage = strDup( ( const Char * )am.getBufferAddr(), am.length() );
	prevExcep = excDup( pe );
	whatCache = 0;

}

Exception::Exception(
	const	Exception&	o )
: std::exception() {

	className = strDup( o.className );
	errorName = strDup( o.errorName );
	adMessage = strDup( o.adMessage );
	prevExcep = excDup( o.prevExcep );
	whatCache = 0;

}

Exception::~Exception() throw () {

	kill();

}

Exception& Exception::operator = (
	const	Exception&	o ) {

	if ( this != &o ) {
		kill();
		className = strDup( o.className );
		errorName = strDup( o.errorName );
		adMessage = strDup( o.adMessage );
		prevExcep = excDup( o.prevExcep );
		whatCache = 0;
	}

	return *this;

}

// ============================================================================

const Char *Exception::what() {

	Buffer	line;
	Buffer	out;

	line = "[EXC] " + Buffer( className )
		+ ": " + Buffer( errorName );

	if ( adMessage ) {
		line += " (" + Buffer( adMessage ) + ")";
	}

	out = line;

	Exception *p = prevExcep;

	while ( p ) {
		line = "\n[EXC] \\___" + Buffer( p->className )
			+ ": " + Buffer( p->errorName );
		if ( p->adMessage ) {
			line += " (" + Buffer( p->adMessage ) + ")";
		}
		out += line;
		p = p->prevExcep;
	}

	whatCache = strDup( ( const Char * )out.getBufferAddr(), out.length() );

	return whatCache;

}

// ============================================================================

void Exception::kill() {

	if ( className ) { delete[] className; className = 0; }
	if ( errorName ) { delete[] errorName; errorName = 0; }
	if ( adMessage ) { delete[] adMessage; adMessage = 0; }
	if ( prevExcep ) { delete   prevExcep; prevExcep = 0; }
	if ( whatCache ) { delete[] whatCache; whatCache = 0; }

}

// ============================================================================

Char *Exception::strDup(
	const	Char		*src,
	const	Uint32		len ) {

	if ( ! src ) {
		return 0;
	}

	Uint32	rlen;

	if ( len == (Uint32)-1 ) {
		rlen = System::strlen( ( const Uchar * )src );
	}
	else {
		rlen = len;
	}

	Char *tmp = new Char[ rlen + 1 ];

	System::memcpy(
		( Uchar * )tmp,
		( const Uchar * )src,
		rlen );

	tmp[ rlen ] = '\0';

	return tmp;

}

Exception *Exception::excDup(
	const	Exception	*src ) {

	return ( src
		? new Exception(
			src->className,
			src->errorName,
			src->adMessage,
			src->prevExcep )
		: 0 );

}

// ============================================================================

