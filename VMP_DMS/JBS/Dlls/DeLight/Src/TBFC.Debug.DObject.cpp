// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Debug".
// 
// "TBFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Debug.DObject.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

//#include <iostream>

#include "TBFC.Base.BufferIO.h"

#include "TBFC.Debug.DObject.h"
#include "TBFC.Debug.Debugger.h"
#include "TBFC.Debug.ThreadFinder.h"

#include "TBFC.Time.Clock.h"

// ============================================================================

//using namespace std;
using namespace TBFC;

// ============================================================================

void TBFC::msgDbg(
	const	Buffer		& __msg
) {
	DObject::staticMsgDbg( __msg );
}

void TBFC::msgWrn(
	const	Buffer		& __msg
) {
	DObject::staticMsgWrn( __msg );
}

void TBFC::msgErr(
	const	Buffer		& __msg
) {
	DObject::staticMsgErr( __msg );
}

void TBFC::msgExc(
		Exception	& __exception
) {
	DObject::staticMsgExc( __exception );
}

void TBFC::msgExc(
	const	Exception	& __exception,
	const	Buffer		& __msg
) {
	DObject::staticMsgExc( __exception, __msg );
}

// ============================================================================

DObject::~DObject() {



}

// ============================================================================

void DObject::staticMsgDbg(
	const	Buffer		& __msg ) {

	staticMsg( __msg, DebugLevel );

}

void DObject::staticMsgWrn(
	const	Buffer		& __msg ) {

	staticMsg( __msg, WarningLevel );

}

void DObject::staticMsgErr(
	const	Buffer		& __msg ) {

	staticMsg( __msg, ErrorLevel );

}

void DObject::staticMsg(
	const	Buffer		& __msg,
	const	Level		__level ) {

	//ColoredBuffer	timePrefix = getDateTimePart( __level );
	//ColoredBuffer	thrdPrefix = getThreadPart( __level );

	//ColoredBuffer	prefix = getDateTimePart( __level )
	//		       + getThreadPart( __level );

	//cerr << "DObject::staticMsg(): time: \""
	//	<< timePrefix.getBuffer() << "\"" << endl;
	//cerr << "DObject::staticMsg(): thrd: \""
	//	<< thrdPrefix.getBuffer() << "\"" << endl;

	sendMsg(
		getDateTimePart( __level )
		+ getThreadPart( __level ),
		false,
		ColoredBuffer(
			__msg,
			partColor[ __level ][ 0 ][ MessagePart ],
			partColor[ __level ][ 1 ][ MessagePart ] ) );

}

void DObject::staticMsgExc(
	const	Exception	& __exc ) {

	sendMsg(
		getDateTimePart( ErrorLevel )
		+ getThreadPart( ErrorLevel ),
		false,
		ColoredBuffer(
			__exc.what(),
			partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
			partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );

}

void DObject::staticMsgExc(
	const	Exception	& __exc,
	const	Buffer		& __pref ) {

	sendMsg(
		getDateTimePart( ErrorLevel )
		+ getThreadPart( ErrorLevel ),
		false,
		ColoredBuffer(
			__pref,
			partColor[ ErrorLevel ][ 0 ][ MessagePart ],
			partColor[ ErrorLevel ][ 1 ][ MessagePart ] )
		+ ColoredBuffer( "\n" )
		+ ColoredBuffer(
			__exc.what(),
			partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
			partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );

}

// ============================================================================

void DObject::msgDbg(
	const	Buffer		& __msg ) const {

	if ( flag ) {
		msg( __msg, DebugLevel );
	}

}

void DObject::msgWrn(
	const	Buffer		& __msg ) const {

	if ( flag ) {
		msg( __msg, WarningLevel );
	}

}

void DObject::msgErr(
	const	Buffer		& __msg ) const {

	msg( __msg, ErrorLevel );

}

void DObject::msg(
	const	Buffer		& __msg,
	const	Level		__level ) const {

	sendMsg(
		getMessagePrefix( __level ),
		true,
		ColoredBuffer(
			__msg,
			partColor[ __level ][ 0 ][ MessagePart ],
			partColor[ __level ][ 1 ][ MessagePart ] ) );

}

void DObject::msgExc(
		Exception	& __exc ) const {

	sendMsg(
		getMessagePrefix( ErrorLevel ),
		true,
		ColoredBuffer(
			__exc.what(),
			partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
			partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );

}

void DObject::msgExc(
		Exception	& __exc,
	const	Buffer		& __pref ) const {

	sendMsg(
		getMessagePrefix( ErrorLevel ),
		true,
		ColoredBuffer(
			__pref,
			partColor[ ErrorLevel ][ 0 ][ MessagePart ],
			partColor[ ErrorLevel ][ 1 ][ MessagePart ] )
		+ ColoredBuffer( "\n" )
		+ ColoredBuffer(
			__exc.what(),
			partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
			partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );

}

// ============================================================================

ColoredBuffer DObject::getDateTimePart(
	const	Level		__level ) {

	return ColoredBuffer(
			longFormat
				? Time::now().dateTimeToBuffer()
				: Time::now().timeToBuffer(),
			partColor[ __level ][ 0 ][ HeaderPart ],
			partColor[ __level ][ 1 ][ HeaderPart ] )
		+ ColoredBuffer( " " );

}

ColoredBuffer DObject::getThreadPart(
	const	Level		__level ) {

	Buffer b = finder->getThreadName();

	if ( ! b.isEmpty() ) {
		b += " ";
	}

	return ColoredBuffer(
			b,
			partColor[ __level ][ 0 ][ ThreadPart ],
			partColor[ __level ][ 1 ][ ThreadPart ] );

}

ColoredBuffer DObject::getObjectPart(
	const	Level		__level ) const {

	return ColoredBuffer(
			getObjectName(),
			partColor[ __level ][ 0 ][ ObjectPart ],
			partColor[ __level ][ 1 ][ ObjectPart ] );

}

ColoredBuffer DObject::getMessagePrefix(
	const	Level		__level ) const {

	return getDateTimePart( __level )
		+ getThreadPart( __level )
		+ getObjectPart( __level );

}

// ============================================================================

ColoredBuffer DObject::prefixedBy(
	const	ColoredBuffer	& prefix,
	const	Bool		inter,
	const	ColoredBuffer	& multiLine ) {

	ColoredBuffer	res;
	Uint32		i = 0;
	Uint32		j;
	Uint32		end = multiLine.getLength();
	Buffer		buf = multiLine.getBuffer();

	i = 0;

	for (;;) {
		// find the longest string NOT containing a NL...
		j = i;
		while (j < end && buf[ j ] != '\n') {
			j++;
		}
		// now, j is on a NL or outside...
		res += prefix;
		if (inter) {
			if (!i)
				res += ColoredBuffer(" ===> ");
			else
				res += ColoredBuffer(" .... ");
		}
		if (j > i) {
			res += multiLine(i, j);
		}
		if (j >= end) {
			break;
		}
		res += Buffer("\n");
		i = j + 1;
	}

	return res;

}

// ============================================================================

void DObject::sendMsg(
	const	ColoredBuffer	& prefix,
	const	Bool		inter,
	const	ColoredBuffer	& multiline ) {

	// cerr << "DObject::sendMsg(): prefix : \"" << prefix.getBuffer() << "\"" << endl;
	// cerr << "DObject::sendMsg(): message: \"" << multiline.getBuffer() << "\"" << endl;

	StderrDebugger::instance()->consumeMessage(
		prefixedBy(
			prefix,
			inter,
			multiline ) );

}

// ============================================================================

Bool DObject::longFormat;

ConsoleColor DObject::partColor[ 4 ][ 2 ][ 5 ] = {

	/* level, fore/back, part */

	// Debug colors
{
	{ Yellow,	LightRed,	LightGreen,	White,		White		},
	{ Black,	Black,		Black,		Black,		LightCyan	}
},

	// Warning colors
{
	{ Yellow,	LightRed,	LightGreen,	Yellow,		White		},
	{ Black,	Black,		Black,		Black,		LightCyan	}
},
	// Error colors
{
	{ Yellow,	LightRed,	LightGreen,	Yellow,		White		},
	{ Black,	Black,		Black,		LightRed,	LightCyan	}
},
	// User colors
{
	{ Yellow,	LightRed,	LightGreen,	White,		White		},
	{ Black,	Black,		Black,		Black,		LightCyan	}
}
};

ThreadFinderPtr DObject::finder = new ThreadFinder;

// ============================================================================





