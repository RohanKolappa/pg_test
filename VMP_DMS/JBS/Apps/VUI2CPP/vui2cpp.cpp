// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	...
// 
// Description:
//	...
// 
// Author(s):
//	...
//
// ============================================================================

#include <iostream>

#include "Array.h"
#include "Buffer.h"
#include "BufferParser.h"
#include "File.h"

// ============================================================================

using namespace std;

// ============================================================================

static const char *progname = 0;

static void usage() {

	cerr << "Usage:" << endl;
	cerr << "    " << progname << " -i <.vui filename> [-f] [-d] [-c <.cpp filename>] [-h <.h filename>]" << endl;
	cerr << "    " << progname << " -v" << endl;
	cerr << "with:" << endl;
	cerr << "-f: force overwriting of output files" << endl;
	cerr << "-v: print version and exit" << endl;

}

// ============================================================================

static Buffer stringify(
	const	Buffer&		iContent ) {

	Buffer	res;
	bool nl = true;

	for ( int i = 0 ; i < iContent.length() ; i++ ) {
		if ( nl ) {
			res += '"';
			nl = false;
		}
		char c = iContent[i];
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

static Buffer unpath(
	const	Buffer&		filename ) {

	if ( filename.isEmpty() ) {
		return filename;
	}

	for ( int pos = filename.length() - 1 ; ; pos-- ) {
		if ( filename[pos] == '/'
		  || filename[pos] == '\\' ) {
			return filename(pos+1);
		}
		if ( pos == 0 ) {
			return filename;
		}
	}

}

// ============================================================================

int main(int argc, char *argv[]) {

	progname = argv[0];

	Buffer	filename;
	Buffer	hFilename;
	Buffer	cFilename;
	bool	doHeader = false;
	bool	doImpl = false;
	bool	doDebug = false;
	bool	doForce = false;

	argc--; argv++;

	while ( argc ) {

		Buffer v( *argv );
		argc--; argv++;

		if ( v == "-i" ) {
			if ( ! argc ) {
				cerr << "Missing input filename!" << endl;
				return -1;
			}
			if ( ! filename.isEmpty() ) {
				cerr << "Duplicate input filename!" << endl;
				return -1;
			}
			Buffer t( *argv );
			argc--; argv++;
			if ( t.isEmpty() ) {
				cerr << "Empty input filename!" << endl;
				return -1;
			}
			filename = t;
			continue;
		}

		if ( v == "-c" ) {
			if ( ! argc ) {
				cerr << "Missing implementation filename!" << endl;
				return -1;
			}
			if ( ! cFilename.isEmpty() ) {
				cerr << "Duplicate implementation filename!" << endl;
				return -1;
			}
			Buffer t( *argv );
			argc--; argv++;
			if ( t.isEmpty() ) {
				cerr << "Empty implementation filename!" << endl;
				return -1;
			}
			cFilename = t;
			doImpl = true;
			continue;
		}

		if ( v == "-h" ) {
			if ( ! argc ) {
				cerr << "Missing header filename!" << endl;
				return -1;
			}
			if ( ! hFilename.isEmpty() ) {
				cerr << "Duplicate header filename!" << endl;
				return -1;
			}
			Buffer t( *argv );
			argc--; argv++;
			if ( t.isEmpty() ) {
				cerr << "Empty headerfilename!" << endl;
				return -1;
			}
			hFilename = t;
			doHeader = true;
			continue;
		}

		if ( v == "-d" ) {
			doDebug = true;
			continue;
		}

		if ( v == "-f" ) {
			doForce = true;
			continue;
		}

		if ( v == "-v" ) {
			cout	<< TARGET_VERSION_MAJ << "."
				<< TARGET_VERSION_MED << "."
				<< TARGET_VERSION_MIN << endl;
			return 0;
		}

		cerr << "Unsupported arg \"" << v << "\"!" << endl;
		usage();
		return -1;
	}

	if ( filename.isEmpty() ) {
		cerr << "No filename!" << endl;
		usage();
		return -1;
	}

	BufferArray pieces = BufferParser::tokenize( unpath( filename ), "." );

	if ( pieces.size() < 2 ) {
		cerr << "Not enough pieces in filename!" << endl;
		return -1;
	}

	Buffer extension = pieces[ pieces.size() - 1 ];

	if ( extension != "vui" ) {
		cerr << "Invalid filename extension!" << endl;
		return -1;
	}

	Buffer varname = pieces[ pieces.size() - 2 ];

	if ( varname[0] >= 'A' && varname[0] <= 'Z' ) {
		varname.set( 0, varname[0] - 'A' + 'a' );
	}

	Buffer nsprefix;
	int i;

	for ( i = 0 ; i < pieces.size() - 2 ; i++ ) {
		nsprefix += pieces[i];
		nsprefix += "::";
	}

	if ( doHeader && ! doForce && File::exists( hFilename ) ) {
		cerr << "Header file already exists!" << endl;
		cerr << "Move it first!" << endl;
		return -1;
	}

	if ( doImpl && ! doForce && File::exists( cFilename ) ) {
		cerr << "Implementation file already exists!" << endl;
		cerr << "Move it first!" << endl;
		return -1;
	}

	// Read input file...

	File iFile;

	if ( ! iFile.open( filename, File::Read ) ) {
		cerr << "Can't open input file!" << endl;
		return -1;
	}

	// Convert input content

	Buffer iContent = iFile.dumpToBuffer();
	Buffer oContent = stringify( iContent );

	if ( doDebug ) {
		cout << oContent;
	}

	if ( doHeader ) {

		File hFile;

		if ( ! hFile.open( hFilename, File::Write ) ) {
			cerr << "Can't open header file!" << endl;
			return -1;
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

	if ( doImpl ) {

		Buffer incFilename;

		if ( doHeader ) {
			incFilename = unpath( hFilename );
		}
		else {
			// We have to figure out the header file ourself!
			BufferArray cpieces = BufferParser::tokenize( unpath( cFilename ), "." );
			for ( i = 0 ; i < cpieces.size() - 1 ; i++ ) {
				incFilename += cpieces[ i ];
				incFilename += ".";
			}
			incFilename += "h";
		}

		File cFile;

		if ( ! cFile.open( cFilename, File::Write ) ) {
			cerr << "Can't open implementation file!" << endl;
			return -1;
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

	return 0;

}

// ============================================================================



