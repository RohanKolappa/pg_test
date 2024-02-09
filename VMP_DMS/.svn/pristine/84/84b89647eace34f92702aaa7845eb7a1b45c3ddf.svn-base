// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	BufferParser.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BufferParser.h"

// ============================================================================

BufferParser::BufferParser() {



}

BufferParser::~BufferParser() {



}

// ============================================================================

BufferArray BufferParser::tokenize(
	const	Buffer&		source,
	const	Buffer&		sep) {

	BufferArray tokens;

	if ( source.isEmpty() ) {
		return tokens;
	}

	int	i;
	int	beg;

	i = 0;

	int	lsource = source.length();

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
				char c = source[i];
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
					return tokens;
//					throw InvalidArgument("Terminating '\\'");
				}
				token += source[i];
				i++;
			}
			if (i == lsource) {
				return tokens;
//				throw InvalidArgument();
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

