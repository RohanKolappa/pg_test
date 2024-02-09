// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.Debug.Console.cpp
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

#include "BFC.Debug.Console.h"
#include "BFC.Debug.ConsoleColor.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::Debug::Console::Data
// ============================================================================

#if defined( PLATFORM_WIN32 )

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

class BFC_Light_DLL Console::Data {

public :

	Data(
	);

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

Console::Data::Data() {

	out = GetStdHandle(STD_ERROR_HANDLE);

}

void Console::Data::setColor(
	const	ConsoleColor	fg,
	const	ConsoleColor	bg) {

	if (GetFileType(out) == FILE_TYPE_CHAR) {
		SetConsoleTextAttribute(
			out,
			fgConv[fg] | bgConv[bg]);
	}

}

void Console::Data::write(
	const	Uchar	*ptr,
	const	Uint32	len) {

	if (GetFileType(out) == FILE_TYPE_CHAR) {
		WriteConsoleA(
			out,
			ptr,
			len,
			NULL,
			NULL);
	} else {
		DWORD dummyWordBecauseWindowsWantsToPutNumberOfBytesWrittenInSomeVariableEvenWhenIDontGiveASh_t;
		WriteFile(
			out,
			ptr,
			len,
			&dummyWordBecauseWindowsWantsToPutNumberOfBytesWrittenInSomeVariableEvenWhenIDontGiveASh_t,
			NULL);
	}

}

void Console::Data::reset() {

}

// ============================================================================

#else

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

	"\033[90m",
	"\033[91m",
	"\033[92m",
	"\033[93m",
	"\033[94m",
	"\033[95m",
	"\033[96m",
	"\033[97m"

//	"\033[1;30m",
//	"\033[1;31m",
//	"\033[1;32m",
//	"\033[1;33m",
//	"\033[1;34m",
//	"\033[1;35m",
//	"\033[1;36m",
//	"\033[1;37m"

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

	"\033[100m",
	"\033[101m",
	"\033[102m",
	"\033[103m",
	"\033[104m",
	"\033[105m",
	"\033[106m",
	"\033[107m"

//	"\033[1;40m",
//	"\033[1;41m",
//	"\033[1;42m",
//	"\033[1;43m",
//	"\033[1;44m",
//	"\033[1;45m",
//	"\033[1;46m",
//	"\033[1;47m"

};

// ============================================================================

class BFC_Light_DLL BFC::Console::Data {

public :

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

};

// ============================================================================

void Console::Data::setColor(
	const	ConsoleColor	fg,
	const	ConsoleColor	bg) {

//	try {
	if ( isatty( 2 ) ) {
		std::cerr << fgConv[fg];
		std::cerr << bgConv[bg];
	}
//	}
//	catch ( __cxxabiv1::__forced_unwind& ) {
//	}

}

void Console::Data::write(
	const	Uchar	*ptr,
	const	Uint32	len) {

	std::cerr.write((const char *)ptr, len);

}

void Console::Data::reset() {

	if ( isatty( 2 ) ) {
		std::cerr << "\033[0m";
	}

}

// ============================================================================

#endif

// ============================================================================
// BFC::Debug::Console
// ============================================================================

BFC_PTR_IMPL( Console )

// ============================================================================

Console & BFC::nl(Console& c) {

	c << ColoredBuffer( "\n" );

	return c;

}

// ============================================================================

Console::Console() {

	data = new Data;

}

Console::~Console() {

	delete data;

}

// ============================================================================

ConsolePtr Console::instance() {

	static ConsolePtr i = new Console;

	return i;

}

// ============================================================================

Console& Console::operator << (Console& (*pf)(Console&)) {

	pf( *this );

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

// ============================================================================

