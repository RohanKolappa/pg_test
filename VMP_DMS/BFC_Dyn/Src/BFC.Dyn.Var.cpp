// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.Var.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferSerializer.h"

#include "BFC.Dyn.Type.h"
#include "BFC.Dyn.Var.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, Var )

// ============================================================================

Dyn::Var::Var(
		TypeCPtr	pType,
	const	Buffer &	pName ) :

	type	( pType ),
	name	( pName ) {

}

Dyn::Var::Var(
	const	Var &		pOther ) :

	SObject		(),
	Cloneable	(),
	Serializable	(),
	Invokable	(),
	Observable	(),
	Observer	(),
	type		( pOther.type ),
	name		( pOther.name ) {

}

// ============================================================================

Dyn::Var & Dyn::Var::operator = (
	const	Var &		pOther ) {

	if ( this != &pOther ) {
		if ( type != pOther.type ) {
			throw InternalError( "Incompatible types!" );
		}
//		name = pOther.name;
	}

	return *this;

}

// ============================================================================

Dyn::VarPtr Dyn::Var::sub(
	const	Buffer &	pTag ) {

	throw InvalidArgument( "sub(): No such child (\"" + pTag + "\")!" );

}

void Dyn::Var::set(
	const	Buffer &	pTag,
	const	Buffer &	pVal ) {

	if ( pTag == "name" ) {
		setName( pVal );
	}
	else {
		throw InvalidArgument( "set(): No such child (\"" + pTag + "\")!" );
	}

}

Buffer Dyn::Var::get(
	const	Buffer &	pTag ) const {

	Buffer res;

	if ( pTag == "name" ) {
		res = getName();
	}
	else {
		throw InvalidArgument( "get(): No such child (\"" + pTag + "\")!" );
	}

	return res;

}

// ============================================================================

Msg::ResultPtr Dyn::Var::handleRequest(
	const	Msg::RequestCPtr &	/* pRequest */ ) {

	throw NotImplemented();

}

// ============================================================================

void Dyn::Var::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	BufferSerializer::doSerialize( name, pOStream );

}

void Dyn::Var::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	name = BufferSerializer::unSerialize( pIStream );

}

// ============================================================================

void Dyn::Var::splitTag(
	const	Buffer &	pTag,
		Buffer &	pPrefix,
		Buffer &	pSuffix ) {

	Uint32 pos = pTag.findFwd( '.' );

	if ( pos >= pTag.getLength() ) {
		pPrefix = pTag;
		pSuffix.kill();
	}
	else {
		pPrefix = pTag( 0, pos );
		pSuffix = pTag( pos + 1 );
	}

}

void Dyn::Var::splitCompact(
	const	Buffer &	pCompact,
		BufferArray &	pExpanded ) {

	pExpanded.kill();

	Uint32 i = 0 ;

	skipWS( pCompact, i );

	if ( i >= pCompact.getLength() ) {
		throw InvalidArgument( "No opening '('!" );
	}

	if ( pCompact[ i ] != '(' ) {
		throw InvalidArgument( "Invalid opening char: '"
			+ Buffer( pCompact[ i ] ) + "'" );
	}

	Uint32	posOpngPar = i;
	Uint32	posClngPar = findClosingChar( pCompact, i ) + 1;

	msgDbg( "splitCompact(): parsing: \""
		+ pCompact( posOpngPar, posClngPar - posOpngPar )
		+ "\"..." );

	i = posClngPar;

	skipWS( pCompact, i );

	if ( i < pCompact.getLength() ) {
		throw InvalidArgument( "Junk at end!" );
	}

	i = posOpngPar + 1;

	for (;;) {
		skipWS( pCompact, i );
		Uint32 posOpngArg = i;
		Uint32 posClngArg;
		if ( pCompact[ i ] == '(' ) {
			posClngArg = findClosingChar( pCompact, i ) + 1;
		}
		else if ( pCompact[ i ] == '"' ) {
			posClngArg = findClosingQuote( pCompact, i ) + 1;
		}
		else {
			posClngArg = pCompact.findAnyFwd( " \t,)", i );
		}
		i = posClngArg;
		skipWS( pCompact, i );
		pExpanded += pCompact( posOpngArg, posClngArg - posOpngArg );
		if ( pCompact[ i ] == ')' ) {
			break;
		}
		if ( pCompact[ i ] != ',' ) {
			throw InvalidArgument( "Invalid separator!" );
		}
		i++;
	}

}

void Dyn::Var::skipWS(
	const	Buffer &	pBuffer,
		Uint32 &	pIndex ) {

	const Uchar *	p = pBuffer.getCstPtr();
	Uint32		l = pBuffer.getLength();

	while ( pIndex < l
	     && ( p[ pIndex ] == ' '
	       || p[ pIndex ] == '\t' ) ) {
		pIndex++;
	}

}

Uint32 Dyn::Var::findClosingChar(
	const	Buffer &	b1,
	const	Uint32		start ) {

	Uint32	t1 = b1.getLength();

	if ( start >= t1 ) {
		throw InternalError();
	}

	const Uchar *	p1 = b1.getCstPtr();
	Uchar		rChar;
	Uchar		lChar = p1[ start ];

	switch ( lChar ) {
	case '(' : rChar = ')'; break;
	case '[' : rChar = ']'; break;
	case '{' : rChar = '}'; break;
	default  : throw InvalidArgument( "Unsupported opening char!" );
	}

	Uint32	levels = 1;
	Uint32	i = start + 1;
	Uchar	current;

	for (;;) {
		if ( i >= t1 ) {
			throw InvalidArgument( "Underflow!" );
		}
		current = p1[ i ];
		if ( current == '\\' ) {
			i++;
		}
		else if ( current == '"' || current == '\'' ) {
			i = findClosingQuote( b1, i );
		}
		else if ( current == lChar ) {
			levels++;
		}
		else if ( current == rChar ) {
			levels--;
		}
		if ( levels == 0 ) {
			break;
		}
		i++;
	}

	return i;

}

Uint32 Dyn::Var::findClosingQuote(
	const	Buffer &	b1,
	const	Uint32		start ) {

	Uint32	t1 = b1.getLength();

	if ( start >= t1 ) {
		throw InternalError();
	}

	const Uchar *	p1 = b1.getCstPtr();
	Uchar		lChar = p1[ start ];

	if ( lChar != '"' && lChar != '\'' ) {
		throw InvalidArgument( "Unsupported quoting type!" );
	}

	Uint32	i = start + 1;
	Uchar	current;

	for (;;) {
		if ( i >= t1 ) {
			throw InvalidArgument( "Underflow!" );
		}
		current = p1[ i ];
		if ( current == '\\' ) {
			i++;
		}
		else if ( current == lChar ) {
			break;
		}
		i++;
	}

	return i;

}

// ============================================================================

