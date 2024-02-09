// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "JBS".
//
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	JBS.Tools.Vui2Cpp.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "BFC.Base.BufferParser.h"
#include "BFC.IO.File.h"

#include "JBS.Tools.Vui2Cpp.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Tools, Vui2Cpp )

// ============================================================================

Tools::Vui2Cpp::Vui2Cpp() {



}

Tools::Vui2Cpp::~Vui2Cpp() {



}

// ============================================================================

Buffer Tools::Vui2Cpp::stringify(
	const	Buffer&		iContent ) const {

	Buffer	res;
	Bool	nl = true;

	for ( Uint32 i = 0 ; i < iContent.length() ; i++ ) {
		if ( nl ) {
			res += '"';
			nl = false;
		}
		Uchar c = iContent[i];
		if ( c == '"' ) {
			res += '\\';
			res += '"';
		}
		else if ( c == '\\' ) {
			res += '\\';
			res += '\\';
		}
		else if ( c == '\n' ) {
			res += '"';
			res += c;
			nl = true;
		}
		else {
			res += c;
		}
	}

	return res;

}

// ============================================================================

void Tools::Vui2Cpp::generateDecl(
		Atoms::VuiDeclHdrAtomPtr	atom ) const {

	FSPath	iPath = atom->getVuiAtom()->getAtomPath();
	FSPath	hPath = atom->getAtomPath();

	Buffer	filename = iPath.getLastComponent();

	BufferArray pieces = BufferParser::tokenize( filename, "." );

	if ( pieces.size() < 2 ) {
		throw InvalidArgument( "Not enough pieces in filename!" );
	}

	Buffer extension = pieces[ pieces.size() - 1 ];

	if ( extension != "vui" ) {
		throw InvalidArgument( "Invalid filename extension!" );
	}

	Buffer varname = pieces[ pieces.size() - 2 ];

	if ( varname[0] >= 'A' && varname[0] <= 'Z' ) {
		varname[ 0 ] = varname[ 0 ] - 'A' + 'a';
	}

	Buffer	nsprefix;
	Uint32	i;

	for ( i = 0 ; i < pieces.size() - 2 ; i++ ) {
		nsprefix += pieces[i];
		nsprefix += "::";
	}

	// Read input file...

	IO::File iFile(
		iPath.toBuffer(),
		IO::File::Read );

	try {
		iFile.open();
	}
	catch ( Exception& e ) {
		throw InvalidArgument( e, "Can't open input file!" );
	}

	// Convert input content

	Buffer iContent = iFile.dumpToBuffer();
	Buffer oContent = stringify( iContent );

	IO::File hFile(
		hPath.toBuffer(),
		IO::File::Write | IO::File::Create | IO::File::Truncate );

	try {
		hFile.open();
	}
	catch ( Exception& e ) {
		throw InvalidArgument( e, "Can't open header file!" );
	}

	hFile.putBytes(
		"// ============================================================================\n"
		"// ===                     AUTOMATICALLY GENERATED FILE                     ===\n"
		"// ===                              DO NOT EDIT                             ===\n"
		"// ============================================================================\n"
		"\n"
	);

	for ( i = 0 ; i < pieces.size() - 2 ; i++ ) {
		hFile.putBytes(
			Buffer( "namespace " ) + pieces[ i ] + " {\n"
		);
	}

	hFile.putBytes(
		Buffer( "\n"
		"// ============================================================================\n"
		"\n"
		"extern const char *" ) + varname + ";\n"
		"\n"
		"// ============================================================================\n"
		"\n"
	);

	for ( i = 0 ; i < pieces.size() - 2 ; i++ ) {
		hFile.putBytes(
			"}\n"
		);
	}

	hFile.putBytes(
		"\n"
		"// ============================================================================\n"
		"\n"
	);

}

// ============================================================================

void Tools::Vui2Cpp::generateImpl(
		Atoms::VuiImplCppAtomPtr	atom ) const {

	FSPath iPath = atom->getVuiAtom()->getAtomPath();
	FSPath hPath = atom->getHdrAtom()->getAtomPath();
	FSPath cPath = atom->getAtomPath();

	Buffer	filename = iPath.getLastComponent();

	BufferArray pieces = BufferParser::tokenize( filename, "." );

	if ( pieces.size() < 2 ) {
		throw InvalidArgument( "Not enough pieces in filename!" );
	}

	Buffer extension = pieces[ pieces.size() - 1 ];

	if ( extension != "vui" ) {
		throw InvalidArgument( "Invalid filename extension!" );
	}

	Buffer varname = pieces[ pieces.size() - 2 ];

	if ( varname[0] >= 'A' && varname[0] <= 'Z' ) {
		varname[ 0 ] = varname[ 0 ] - 'A' + 'a';
	}

	Buffer	nsprefix;
	Uint32	i;

	for ( i = 0 ; i < pieces.size() - 2 ; i++ ) {
		nsprefix += pieces[i];
		nsprefix += "::";
	}

	// Read input file...

	IO::File iFile(
		iPath.toBuffer(),
		IO::File::Read );

	try {
		iFile.open();
	}
	catch ( Exception& e ) {
		throw InvalidArgument( e, "Can't open input file!" );
	}

	// Convert input content

	Buffer iContent = iFile.dumpToBuffer();
	Buffer oContent = stringify( iContent );

	Buffer incFilename = hPath.getLastComponent();

	IO::File cFile(
		cPath.toBuffer(),
		IO::File::Write | IO::File::Truncate | IO::File::Create );

	try {
		cFile.open();
	}
	catch ( Exception& /* e */ ) {
		throw InvalidArgument( "Can't open implementation file!" );
	}

	cFile.putBytes(
		Buffer( "// ============================================================================\n"
		"// ===                     AUTOMATICALLY GENERATED FILE                     ===\n"
		"// ===                              DO NOT EDIT                             ===\n"
		"// ============================================================================\n"
		"\n"
		"#include \"" ) + incFilename + "\"\n"
		"\n"
		"// ============================================================================\n"
		"\n"
		"const char * " + nsprefix + varname + " =\n"
	);

	cFile.putBytes(
		oContent
	);

	cFile.putBytes(
		"\n"
		";\n"
		"\n"
		"// ============================================================================\n"
		"\n"
	);

}

// ============================================================================

