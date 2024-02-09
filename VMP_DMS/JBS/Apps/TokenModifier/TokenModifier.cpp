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
//	TokenModifier.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.Buffer.h"
#include "BFC.Base.BufferIO.h"
#include "BFC.IO.File.h"

// ============================================================================

using namespace std;
using namespace BFC;
using namespace BFC::IO;

// ============================================================================

Bool isTokenChar(const Uchar c) {

	return (c == '_'
	     || (c >= 'A' && c <= 'Z')
	     || (c >= 'a' && c <= 'z')
	     || (c >= '0' && c <= '9'));

}

static Buffer transform(
	const	Buffer &	in,
	const	Buffer &	oldName,
	const	Buffer &	newName) {

	Buffer	res;
	Uint32	i = 0;

	for (;;) {

		// Find occurrence of <oldName>

		Uint32 j = i;

		if ( ! in.findBufferAt( j, oldName ) ) {
			res += in( i );
			return res;
		}

		if ( j > i ) {
			res += in( i, j - i );
			i = j;
		}

		i += oldName.getLength();

		if ( ( j > 0 && isTokenChar( in[ j - 1 ] ) )
		  || ( i < in.getLength() && isTokenChar( in[ i ] ) ) ) {
			res += oldName;
		}
		else {
			res += newName;
		}

	}

}

// ============================================================================

int main(int argc, char *argv[]) {

	if ( argc != 3 ) {
		cerr << "Usage: "
			<< argv[ 0 ]
			<< " <old_name> <new_name>" << endl;
		return -1;
	}

	Uint32	i = 1;

	Buffer	oldName = Buffer( argv[ i++ ] );
	Buffer	newName = Buffer( argv[ i++ ] );

	File	input( 0, File::Read );
	File	output( 1, File::Write );

	input.open();
	output.open();

	Buffer	file;

	// Input...

	file = input.dumpToBuffer();

	// Processing...

	Buffer res = transform( file, oldName, newName );

	// Output...

	output.putBytes(res);

	return 0;

}

// ============================================================================



