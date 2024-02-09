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
//	TBFC.Debug.Console.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#ifdef PLATFORM_WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "TBFC.Debug.Console.h"
#include "TBFC.Debug.ConsoleColor.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL( Console )

// ============================================================================
// ============================================================================

#if defined( PLATFORM_WIN32 )

// ============================================================================
// ============================================================================

static WORD fgConv[] = {

	0,
	FOREGROUND_RED,
	                 FOREGROUND_GREEN,
	FOREGROUND_RED | FOREGROUND_GREEN,			// YELLOW
	                                    FOREGROUND_BLUE,
	FOREGROUND_RED                    | FOREGROUND_BLUE,	// MAGENTA
	                 FOREGROUND_GREEN | FOREGROUND_BLUE,	// CYAN
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,	// WHITE

	                                                      FOREGROUND_INTENSITY,
	FOREGROUND_RED                                      | FOREGROUND_INTENSITY,
	                 FOREGROUND_GREEN                   | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_GREEN                   | FOREGROUND_INTENSITY,
	                                    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_RED                    | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	                 FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

};

static WORD bgConv[] = {

	0                                                                         ,
	BACKGROUND_RED                                                            ,
	                 BACKGROUND_GREEN                                         ,
	BACKGROUND_RED | BACKGROUND_GREEN                                         ,
	                                    BACKGROUND_BLUE                       ,
	BACKGROUND_RED                    | BACKGROUND_BLUE                       ,
	                 BACKGROUND_GREEN | BACKGROUND_BLUE                       ,
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE                       ,

	                                                      BACKGROUND_INTENSITY,
	BACKGROUND_RED                                      | BACKGROUND_INTENSITY,
	                 BACKGROUND_GREEN                   | BACKGROUND_INTENSITY,
	BACKGROUND_RED | BACKGROUND_GREEN                   | BACKGROUND_INTENSITY,
	                                    BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	BACKGROUND_RED                    | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	                 BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY

};

// ============================================================================

class TBFC::ConsoleData {

public :

	ConsoleData();
	~ConsoleData() {}

	void setColor(
		const	ConsoleColor	fg,
		const	ConsoleColor	bg
	);

	void write(
		const	Uchar		*ptr,
		const	Uint32		len
	);

	void reset(
	);

private :

	HANDLE		out;

};

// ============================================================================

ConsoleData::ConsoleData() {

	out = GetStdHandle(STD_OUTPUT_HANDLE);

}

void ConsoleData::reset() {



}

void ConsoleData::setColor(
	const	ConsoleColor	fg,
	const	ConsoleColor	bg) {

	SetConsoleTextAttribute(
		out,
		fgConv[fg] | bgConv[bg]);

}

void ConsoleData::write(
	const	Uchar	*ptr,
	const	Uint32	len) {

	WriteConsole(
		out,
		ptr,
		len,
		NULL,
		NULL);

}

// ============================================================================
// ============================================================================

#else

// ============================================================================
// ============================================================================

static const char * fgConv[] = {

	"\033[30m",
	"\033[31m",
	"\033[32m",
	"\033[33m",
	"\033[34m",
	"\033[35m",
	"\033[36m",
	"\033[37m",

	"\033[1;30m",
	"\033[1;31m",
	"\033[1;32m",
	"\033[1;33m",
	"\033[1;34m",
	"\033[1;35m",
	"\033[1;36m",
	"\033[1;37m"

};

static const char * bgConv[] = {

	"\033[40m",
	"\033[41m",
	"\033[42m",
	"\033[43m",
	"\033[44m",
	"\033[45m",
	"\033[46m",
	"\033[47m",

	"\033[1;40m",
	"\033[1;41m",
	"\033[1;42m",
	"\033[1;43m",
	"\033[1;44m",
	"\033[1;45m",
	"\033[1;46m",
	"\033[1;47m"

};

// ============================================================================

class TBFC::ConsoleData {

public :

	ConsoleData() {}
	~ConsoleData() {}

	void setColor(
		const	ConsoleColor	fg,
		const	ConsoleColor	bg
	);

	void write(
		const	Uchar		*ptr,
		const	Uint32		len
	);

	void reset(
	);

private :

};

// ============================================================================

void ConsoleData::setColor(
	const	ConsoleColor	fg,
	const	ConsoleColor	bg) {

	if ( isatty( 2 ) ) {
		std::cerr << fgConv[fg];
		std::cerr << bgConv[bg];
	}

}

void ConsoleData::write(
	const	Uchar	*ptr,
	const	Uint32	len) {

	std::cerr.write((const char *)ptr, len);

}

void ConsoleData::reset() {

	if ( isatty( 2 ) ) {
		std::cerr << "\033[0m";
	}

}

// ============================================================================
// ============================================================================

#endif

// ============================================================================
// ============================================================================

Console & TBFC::nl(Console& c) {

	c << ColoredBuffer("\n");

	return c;

}

// ============================================================================

Console::Console() {

//	std::cerr << "Console()" << std::endl;

	data = new ConsoleData;

}

Console::~Console() {

	delete data;

//	std::cerr << "~Console()" << std::endl;

}

// ============================================================================

ConsolePtr Console::instance() {

	static ConsolePtr i = new Console;

	return i;

}

// ============================================================================

Console& Console::operator << (Console& (*pf)(Console&)) {
// Console& Console::operator << (std::ostream& (*pf)(std::ostream&)) {

	pf(*this);

	return *this;

}

// ============================================================================

Console& Console::operator << (const Buffer& b) {

	data->write(b.getBufferAddr(), b.length());

	return *this;

}

Console& Console::operator << (const ColoredBuffer& b) {

	Uint32 p = 0;
	Uint32 c = 0;
	Uint32 f;

	while (p < b.getBuffer().length()) {
		if (b.getStartPosition(c) != p) {
			std::cerr << "INTERNAL ERROR" << std::endl;
			return *this;
		}
		data->setColor(b.getForeground(c), b.getBackground(c));
		c++;
		if (c == b.getColorsNumber()) {
			f = b.getBuffer().length();
		}
		else {
			f = b.getStartPosition(c);
		}
		data->write(b.getBuffer().getBufferAddr() + p, f - p);
		p = f;
	}

	data->reset();

	return *this;

}

Console& Console::operator << (const Exception& e) {

	ColoredBuffer	cb;

	cb = ColoredBuffer( "[EXC] ", LightRed )
	   + ColoredBuffer( e.getClassName(), LightGreen )
	   + Buffer( ": " )
	   + ColoredBuffer( e.getErrorName(), Yellow );

	if ( e.getAdMessage() ) {
		cb += ColoredBuffer(
			" (" + Buffer( e.getAdMessage() ) + ")",
			White );
	}

	*this << cb;

	const Exception *p = e.getPrevExcep();
	Uint32 i = 0;

	while ( p ) {

		*this << nl;

		cb = ColoredBuffer(
				"["
				+ Buffer( i + 1, Buffer::Base10, 3 )
				+ "] ",
			LightRed )
		   + ColoredBuffer( p->getClassName(), LightGreen )
		   + Buffer( ": " )
		   + ColoredBuffer( p->getErrorName(), Yellow );

		if ( p->getAdMessage() ) {
			cb += ColoredBuffer(
					" ("
					+ Buffer( p->getAdMessage() )
					+ ")",
				White );
		}

		*this << cb;

		p = p->getPrevExcep();
		i++;

	}

	return *this;

}

// ============================================================================
// ============================================================================

