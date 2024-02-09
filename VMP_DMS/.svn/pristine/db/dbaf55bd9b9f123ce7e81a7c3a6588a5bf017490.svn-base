// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Base.BufferParser.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.BufferParser.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

BufferParser::BufferParser() {



}

BufferParser::~BufferParser() {



}

// ============================================================================

BufferArray BufferParser::scan(
	const	Buffer		& source,
	const	Buffer		& format ) {

	BufferArray	results;

	Uint32		isource = 0;
	Uint32		iformat = 0;
	Uint32		beg;
	Uchar		c;

	while ( iformat < format.length() ) {

		if ( isource >= source.length() ) {
			throw Underflow("Format too long!");
		}

		c = format[iformat];

		if ( c != '%' ) {
			// A char we have to find 'verbatim' in the source!
			if ( source[isource] != c ) {
				throw WrongMatch("Char '"
					+ Buffer(c)
					+ "' from format["
					+ Buffer(iformat)
					+ "] not found at source["
					+ Buffer(isource)
					+ "]!");
			}
			iformat++;
			isource++;
			continue;
		}

		// We need at least one more char in the format string!

		if ( iformat + 1 == format.length() ) {
			throw Underflow("Format truncated!");
		}

		c = format[iformat + 1];

		if ( c == '%' ) {
			// Got to find a single '%'!
			if ( source[isource] != '%' ) {
				throw WrongMatch("Char '%' from format("
					+ Buffer(iformat)
					+ ",2) not found at source["
					+ Buffer(isource)
					+ "]!");
			}
			iformat += 2;
			isource++;
			continue;
		}

		if (c == '?') {
			// Any char should match!
			isource++;
			iformat += 2;
			continue;
		}
		// Last choice: got to find an integer probably!
		if (c != 'd') {
			throw InvalidFormat("Flag '"
				+ Buffer(c)
				+ "' from format["
				+ Buffer(iformat)
				+ "]!");
		}
		beg = isource;
		while ( isource < source.length()
		     && source[isource] >= '0'
		     && source[isource] <= '9' ) {
			isource++;
		}

		if ( isource == beg ) {
			throw WrongMatch();
		}
		results += source(beg, isource - beg);
		iformat += 2;

	}

	if ( isource != source.length() ) {
		throw Overflow();
	}

	return results;

}

// ============================================================================

BufferArray BufferParser::tokenize(
	const	Buffer		& source,
	const	Buffer		& sep) {

	BufferArray tokens;

	if ( sep.isEmpty() ) {
		throw InvalidArgument();
	}

	if ( source.isEmpty() ) {
		return tokens;
	}

	Uint32	i;
	Uint32	beg;

	i = 0;

	Uint32	lsource = source.length();

	for (;;) {

		// Let's skip all separators.

		while ( i < lsource && sep.contains(source[i]) ) {
			i++;
		}

		if (i == lsource) {
			break;
		}

		// We are on a non-separator char.

		if ( source[i] == '"' ) {
			// Start string handling...
			i++;
			Buffer token;
			beg = i;
			while ( i < lsource ) {
				Uchar c = source[i];
				if (c == '"') {
					break;
				}
				if (c != '\\') {
					token += c;
					i++;
					continue;
				}
				i++;
				if (i == lsource) {
					throw InvalidArgument("Terminating '\\'");
				}
				token += source[i];
				i++;
			}
			if (i == lsource) {
				throw InvalidArgument( "Missing '\"'" );
			}
			tokens += token;
			i++;
			continue;
		}

		beg = i;
		while ( i < lsource && ! sep.contains(source[i]) ) {
			i++;
		}
		tokens += source(beg, i - beg);
		if (i == lsource) {
			break;
		}
	}

	return tokens;

}

// ============================================================================

