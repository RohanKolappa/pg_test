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

#include "Buffer.h"
#include "File.h"

#include "QtUIParser.h"

// ============================================================================

using namespace std;

// ============================================================================

static const char *progname = 0;

static void usage() {

	cerr << "Usage:" << endl;
	cerr << "    " << progname << " [-d] [-q] [-f] -i <.ui filename> [-o <.vui filename>]" << endl;
	cerr << "    " << progname << " -v" << endl;
	cerr << "with:" << endl;
	cerr << "-d: print resulting VMP tree" << endl;
	cerr << "-q: suppress warnings" << endl;
	cerr << "-f: force overwriting of output files" << endl;
	cerr << "-v: print version and exit" << endl;

}

// ============================================================================

int main(int argc, char *argv[]) {

	progname = argv[0];

	Buffer	iFilename;
	Buffer	oFilename;
	bool	doDebug = false;
	bool	beQuiet = false;
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
			if ( ! iFilename.isEmpty() ) {
				cerr << "Duplicate input filename!" << endl;
				return -1;
			}
			Buffer t( *argv );
			argc--; argv++;
			if ( t.isEmpty() ) {
				cerr << "Empty input filename!" << endl;
				return -1;
			}
			iFilename = t;
			continue;
		}

		if ( v == "-o" ) {
			if ( ! argc ) {
				cerr << "Missing output filename!" << endl;
				return -1;
			}
			if ( ! oFilename.isEmpty() ) {
				cerr << "Duplicate output filename!" << endl;
				return -1;
			}
			Buffer t( *argv );
			argc--; argv++;
			if ( t.isEmpty() ) {
				cerr << "Empty output filename!" << endl;
				return -1;
			}
			oFilename = t;
			continue;
		}

		if ( v == "-d" ) {
			doDebug = true;
			continue;
		}

		if ( v == "-q" ) {
			beQuiet = true;
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

	if ( iFilename.isEmpty() ) {
		cerr << "No input filename!" << endl;
		usage();
		return -1;
	}

	File iFile;

	if ( ! iFile.open( iFilename, File::Read ) ) {
		cerr << "Can't open input file!" << endl;
		return -1;
	}

	Buffer iContent;

	iContent = iFile.dumpToBuffer();

	QtUIParser qtparser;

	if ( beQuiet ) {
		qtparser.setDebug( false );
	}

	Buffer oContent;

	oContent = qtparser.parse( iContent );

	if ( oContent.isEmpty() ) {
		cerr << "Can't parse input file!" << endl;
		return -1;
	}

	if ( doDebug ) {
		cout << oContent;
	}

	if ( ! oFilename.isEmpty() ) {

		File oFile;

		if ( ! doForce && File::exists( oFilename ) ) {
			cerr << "Output file exists!" << endl;
			return -1;
		}

		if ( ! oFile.open( oFilename, File::Write ) ) {
			cerr << "Can't open output file!" << endl;
			return -1;
		}

		oFile.putBytes( oContent );

	}

	return 0;

}

// ============================================================================

