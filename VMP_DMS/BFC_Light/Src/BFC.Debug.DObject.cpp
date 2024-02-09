// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Debug".
// 
// "BFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Debug.DObject.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferIO.h"
#include "BFC.Base.SPtrImpl.h"

#include "BFC.Debug.DObject.h"
#include "BFC.Debug.Debugger.h"
#include "BFC.Debug.ThreadFinder.h"

#include "BFC.Time.Clock.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( BFC, DObject )

// ============================================================================

void BFC::msgDbg(
	const	Buffer &	pMsg ) {

	DObject::staticMsgDbg( pMsg );

}

void BFC::msgInf(
	const	Buffer &	pMsg ) {

	DObject::staticMsgInf( pMsg );

}

void BFC::msgWrn(
	const	Buffer &	pMsg ) {

	DObject::staticMsgWrn( pMsg );

}

void BFC::msgErr(
	const	Buffer &	pMsg ) {

	DObject::staticMsgErr( pMsg );

}

void BFC::msgExc(
		Exception &	pExc ) {

	DObject::staticMsgExc( pExc );

}

void BFC::msgExc(
	const	Exception &	pExc,
	const	Buffer &	pMsg ) {

	DObject::staticMsgExc( pExc, pMsg );

}

// ============================================================================

ThreadFinderPtr & DObject::getThreadFinder() {

	static ThreadFinderPtr finder = new ThreadFinder;

	return finder;

}

// ============================================================================

void DObject::staticMsgDbg(
	const	Buffer &	pMsg ) {

	staticMsg( pMsg, DebugLevel );

}

void DObject::staticMsgInf(
	const	Buffer &	pMsg ) {

	staticMsg( pMsg, InfoLevel );

}

void DObject::staticMsgWrn(
	const	Buffer &	pMsg ) {

	staticMsg( pMsg, WarningLevel );

}

void DObject::staticMsgErr(
	const	Buffer &	pMsg ) {

	staticMsg( pMsg, ErrorLevel );

}

void DObject::staticMsg(
	const	Buffer &	pMsg,
	const	LogLevel	pLvl ) {

	if ( glbLevel >= pLvl
	  && pLvl >= ErrorLevel
	  && pLvl <= DebugLevel ) {
		sendMsg(
			getDateTimePart( pLvl )
			+ getThreadPart( pLvl ),
			false,
			ColoredBuffer(
				pMsg,
				partColor[ pLvl ][ 0 ][ MessagePart ],
				partColor[ pLvl ][ 1 ][ MessagePart ] ) );
	}

}

void DObject::staticMsgExc(
	const	Exception &	pExc ) {

	if ( glbLevel >= ErrorLevel ) {
		sendMsg(
			getDateTimePart( ErrorLevel )
			+ getThreadPart( ErrorLevel ),
			false,
			ColoredBuffer(
				pExc.what(),
				partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
				partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );
	}

}

void DObject::staticMsgExc(
	const	Exception &	pExc,
	const	Buffer &	pPrf ) {

	if ( glbLevel >= ErrorLevel ) {
		sendMsg(
			getDateTimePart( ErrorLevel )
			+ getThreadPart( ErrorLevel ),
			false,
			ColoredBuffer(
				pPrf,
				partColor[ ErrorLevel ][ 0 ][ MessagePart ],
				partColor[ ErrorLevel ][ 1 ][ MessagePart ] )
			+ ColoredBuffer( "\n" )
			+ ColoredBuffer(
				pExc.what(),
				partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
				partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );
	}

}

// ============================================================================

void DObject::msgDbg(
	const	Buffer &	pMsg ) const {

	msg( pMsg, DebugLevel );

}

void DObject::msgInf(
	const	Buffer &	pMsg ) const {

	msg( pMsg, InfoLevel );

}

void DObject::msgWrn(
	const	Buffer &	pMsg ) const {

	msg( pMsg, WarningLevel );

}

void DObject::msgErr(
	const	Buffer &	pMsg ) const {

	msg( pMsg, ErrorLevel );

}

void DObject::msg(
	const	Buffer &	pMsg,
	const	LogLevel	pLvl ) const {

	if ( glbLevel >= pLvl
	  && objLevel >= pLvl
	  && pLvl >= ErrorLevel
	  && pLvl <= DebugLevel ) {
		sendMsg(
			getMessagePrefix( pLvl ),
			true,
			ColoredBuffer(
				pMsg,
				partColor[ pLvl ][ 0 ][ MessagePart ],
				partColor[ pLvl ][ 1 ][ MessagePart ] ) );
	}

}

void DObject::msgExc(
		Exception &	pExc ) const {

	if ( glbLevel >= ErrorLevel
	  && objLevel >= ErrorLevel ) {
		sendMsg(
			getMessagePrefix( ErrorLevel ),
			true,
			ColoredBuffer(
				pExc.what(),
				partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
				partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );
	}

}

void DObject::msgExc(
		Exception &	pExc,
	const	Buffer &	pPrf ) const {

	if ( glbLevel >= ErrorLevel
	  && objLevel >= ErrorLevel ) {
		sendMsg(
			getMessagePrefix( ErrorLevel ),
			true,
			ColoredBuffer(
				pPrf,
				partColor[ ErrorLevel ][ 0 ][ MessagePart ],
				partColor[ ErrorLevel ][ 1 ][ MessagePart ] )
			+ ColoredBuffer( "\n" )
			+ ColoredBuffer(
				pExc.what(),
				partColor[ ErrorLevel ][ 0 ][ ExceptionPart ],
				partColor[ ErrorLevel ][ 1 ][ ExceptionPart ] ) );
	}

}

// ============================================================================

ColoredBuffer DObject::getDateTimePart(
	const	LogLevel	pLvl ) {

	return ColoredBuffer(
			longFormat
				? Time::now().dateTimeToBuffer( true, true )
				: Time::now().timeToBuffer(),
			partColor[ pLvl ][ 0 ][ HeaderPart ],
			partColor[ pLvl ][ 1 ][ HeaderPart ] )
		+ ColoredBuffer( " " );

}

ColoredBuffer DObject::getThreadPart(
	const	LogLevel	pLvl ) {

	Buffer b = getThreadFinder()->getThreadName();

	if ( ! b.isEmpty() ) {
		b += " ";
	}

	return ColoredBuffer(
			b,
			partColor[ pLvl ][ 0 ][ ThreadPart ],
			partColor[ pLvl ][ 1 ][ ThreadPart ] );

}

ColoredBuffer DObject::getObjectPart(
	const	LogLevel	pLvl ) const {

	return ColoredBuffer(
			getObjectName(),
			partColor[ pLvl ][ 0 ][ ObjectPart ],
			partColor[ pLvl ][ 1 ][ ObjectPart ] );

}

ColoredBuffer DObject::getMessagePrefix(
	const	LogLevel	pLvl ) const {

	return getDateTimePart( pLvl )
		+ getThreadPart( pLvl )
		+ getObjectPart( pLvl );

}

// ============================================================================

ColoredBuffer DObject::prefixedBy(
	const	ColoredBuffer &	prefix,
	const	Bool		inter,
	const	ColoredBuffer &	multiLine ) {

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
	const	ColoredBuffer &	prefix,
	const	Bool		inter,
	const	ColoredBuffer &	multiline ) {

	if ( debugger ) {
		debugger->consumeMessage(
			prefixedBy(
				prefix,
				inter,
				multiline ) );
	}

}

// ============================================================================

DebuggerPtr DObject::debugger = StderrDebugger::instance();

Bool DObject::longFormat = true;

DObject::LogLevel DObject::glbLevel = DebugLevel;

ConsoleColor DObject::partColor[ 5 ][ 2 ][ 5 ] = {

	/* level, fore/back, part */

	// Quiet colors
{
	{ Black,	Black,		Black,		Black,		Black		},
	{ Black,	Black,		Black,		Black,		Black		}
},
	// Error colors
{
	{ Yellow,	LightRed,	LightGreen,	Yellow,		White		},
	{ Black,	Black,		Black,		LightRed,	LightCyan	}
},
	// Warning colors
{
	{ Yellow,	LightRed,	LightGreen,	Yellow,		White		},
	{ Black,	Black,		Black,		Black,		LightCyan	}
},
	// Info colors
{
	{ Yellow,	LightRed,	LightGreen,	White,		White		},
	{ Black,	Black,		Black,		Black,		LightCyan	}
},
	// Debug colors
{
	{ Yellow,	LightRed,	LightGreen,	White,		White		},
	{ Black,	Black,		Black,		Black,		LightCyan	}
}

};

// ============================================================================

