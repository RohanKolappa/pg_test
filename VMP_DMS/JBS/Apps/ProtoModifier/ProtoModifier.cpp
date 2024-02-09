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
//	ProtoModifier.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#include "BFC.TL.Array.h"
#include "BFC.Base.Buffer.h"
#include "BFC.Base.BufferIO.h"
#include "BFC.IO.File.h"

// ============================================================================

using namespace std;
using namespace BFC;
using namespace BFC::IO;

// ============================================================================

Bool isSpace(const Uchar c) {

	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');

}

Bool isTokenChar(const Uchar c) {

	return (c == '_'
	     || (c >= 'A' && c <= 'Z')
	     || (c >= 'a' && c <= 'z')
	     || (c >= '0' && c <= '9'));

}

Buffer getArg(
	const	Buffer&	in,
		Uint32&	i) {

	Bool	in_string = false;
	Bool	dblQuote = false;
	Uint32	par_level = 0;
	Uint32	arg_beg = i;
	Uchar	c;

	Buffer	res;
	Bool	done = false;

	while (i < in.length() && !done) {

		switch (c = in[i++]) {
		case '(' :
			if (!in_string)
				par_level++;
			break;
		case ',' :
			if (in_string || par_level)
				break;
			i--;
			res = in( arg_beg, i - arg_beg );
			done = true;
			break;
		case ')' :
			if (in_string)
				break;
			if (par_level) {
				par_level--;
				break;
			}
			i--;
			res = in( arg_beg, i - arg_beg );
			done = true;
			break;
		case '\'' :
			if ( ! in_string ) {
				in_string = true;
				dblQuote = false;
			}
			else if ( ! dblQuote ) {
				in_string = false;
			}
			break; // normal character if between dbl quote
		case '"' :
			if ( ! in_string ) {
				in_string = true;
				dblQuote = true;
			}
			else if ( dblQuote ) {
				in_string = false;
			}
			break; // normal character if between single quote
		case '\\' :
			if ( in_string ) {
				i++;
			}
			break;
		default :
			break;
		}

	}

	while (res.length() >= 2 && isSpace(res[res.length()-1])) {
		res = res( 0, res.length() - 1);
	}

	while (res.length() >= 2 && isSpace(res[0])) {
		res = res( 1, res.length() - 1);
	}

	return res;

}

Buffer instanciate(
	const	Buffer&		proto,
	const	BufferArray&	args) {

//	cerr << "DEBUG: instanciating in '" << proto << "'" << endl;
//	cerr << "DEBUG: with args:" << endl;
//	for ( Uint32 ii = 0 ; ii < args.size() ; ii++ ) {
//		cerr << "[" << ii << "]: " << args[ii] << endl;
//	}

	Buffer res;

	Uint32 i = 0;

	while (i < proto.length()) {
		Uchar c = proto[i++];
		if (c == '@') {
			Uint32 idx = (Uint32)(proto[i++] - '1');
			if (idx >= args.size()) {
				cerr << "OVERFLOW while instanciating!" << endl;
				break;
			}
			else {
//				cerr << "DEBUG: instanciate arg " << idx << " ---> '" << args[idx] << "'" << endl;
			}
			res += args[idx];
		}
		else {
			res += c;
		}
	}

//	cerr << "DEBUG: instanciate ---> " << res << endl;

	return res;

}

Buffer transform(
	const	Buffer&	in,
	const	Buffer&	func_name,
	const	Uint32	nbr_args,
	const	Buffer&	new_proto) {

	Buffer	res;
	Uint32	i = 0;

	for (;;) {

		// Find occurrence of <func_name>

		Uint32 j = i;

		if (!in.findBufferAt(j, func_name)) {
			res += in( i, in.length() - i );
			return res;
		}

		if (j > i) {
			res += in( i, j - i );
			i = j;
		}

		// maybe we found the right part of a larger string...

		if (i && isTokenChar(in[i-1])) {
			res += func_name;
			i += func_name.length();
			continue;
		}

		Uint32 func_beg = i; // where the func_name begins

		i += func_name.length();

		// Skip blank chars...

		while ( i < in.length()
		     && ( in[i] == ' '
		       || in[i] == '\t'
		       || in[i] == '\n'
		       || in[i] == '\r'))
			i++;

		if (i == in.length()) {
			// no opening par... terminate
			res += in( func_beg, in.length() - func_beg );
			return res;
		}

		if (in[i] != '(') {
			// not an opening par...
			res += in( func_beg, i - func_beg );
			continue;
		}

		// Got an opening par!

		i++;
		BufferArray args_list;

		for (;;) {

			// Skip blank chars...

			while ( i < in.length()
			     && ( in[i] == ' '
			       || in[i] == '\t'
			       || in[i] == '\n'
			       || in[i] == '\r'))
				i++;

			if ( in[i] == ')' ) {
				i++;
				break;
			}

			args_list += getArg(in, i);
// cerr << "DEBUG: new arg: " << args_list[args_list.size()-1] << endl;
			if (i >= in.length()) {
				res += in( func_beg, in.length() - func_beg);
				return res;
			}
			if (in[i] == ',') {
				i++;
				while ( i < in.length()
				     && ( in[i] == ' '
				       || in[i] == '\t'
				       || in[i] == '\n'
				       || in[i] == '\r'))
					i++;
				continue;
			}
			if (in[i] != ')') {
				i = func_beg;
				res += in( func_beg, in.length() - func_beg );
				return res;
			}
			i++;
			break;
		}

		if (args_list.size() != nbr_args) {
			res += in( func_beg, i - func_beg );
			continue;
		}

		res += instanciate(new_proto, args_list);

	}

}

// ============================================================================

int main(int argc, char *argv[]) {

	if (argc != 4) {
		cerr << "Usage: "
			<< argv[0]
			<< " <func_name> <#args> <output>" << endl;
		return -1;
	}

	Uint32	i = 1;

	Buffer	func_name = Buffer(argv[i++]);
	Uint32	nbr_args = Buffer(argv[i++]).toUint32();
	Buffer	new_proto = Buffer(argv[i++]);

	File	input( 0, File::Read );
	File	output( 1, File::Write );

	input.open();
	output.open();

	Buffer	file;

	// Input...

	file = input.dumpToBuffer();

	// Processing...

	Buffer res = transform(file, func_name, nbr_args, new_proto);

	// Output...

	output.putBytes(res);

	return 0;

}

// ============================================================================



