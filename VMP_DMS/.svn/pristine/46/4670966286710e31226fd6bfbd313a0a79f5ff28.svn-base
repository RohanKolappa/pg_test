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

#include "BFC.Base.Buffer.h"
#include "BFC.Base.BufferIO.h"
#include "BFC.Base.Exception.h"

#include "BFC.IO.File.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

static Buffer	exeName;
static Buffer	clsName;

// ============================================================================

static void doIt() {

	Buffer		fleName = clsName + ".java";
	IO::FilePtr	clsFile = new IO::File( fleName, IO::File::New );

	clsFile->open();

	clsFile->putBytes(
		"// ============================================================================\n"
		"//\n"
		"// Copyright (c) 2003-2014 Barco N.V.\n"
		"//\n"
		"// ============================================================================\n"
		"//\n"
		"// Filename:\n"
		"//	" + fleName +"\n"
		"//\n"
		"// Description:\n"
		"//	...\n"
		"//\n"
		"// Author(s):\n"
		"//	Jean-Francois GOBBERS\n"
		"//\n"
		"// ============================================================================\n"
		"\n"
		"// package com.barco.???;\n"
		"\n"
		"// ============================================================================\n"
		"\n"
		"// import com.barco.???;\n"
		"\n"
		"// ============================================================================\n"
		"\n"
		"public class " + clsName + " {\n"
		"\n"
		"\t/// \\brief Creates a new " + clsName + ".\n"
		"\n"
		"\tpublic " + clsName + "(\n"
		"\t) {\n"
		"\t}\n"
		"\n"
		"\t// --------------------------------------------------------------------\n"
		"\n"
		"//\tpublic static void main(\n"
		"//\t	String[]	pArgs\n"
		"//\t) {\n"
		"//\n"
		"//\t}\n"
		"\n"
		"}\n"
		"\n"
		"// ============================================================================\n"
		"\n" );

	clsFile->close();

}

// ============================================================================

static void usage() {

	cerr << "Usage: " << exeName << " [option]... [clsName]..." << endl;
	cerr << endl;
	cerr << "  -h      display this help and exit" << endl;

}

int main( int argc, char *argv[] ) {

	BufferArray	args( argc, argv );

	exeName = args.pop( 0 );

	while ( ! args.isEmpty() && args[ 0 ].beginsWith( "-" ) ) {
		Buffer cmd = args.pop( 0 );
		if ( cmd == "-h" ) {
			usage();
			return 0;
		}
		else {
			usage();
			return -1;
		}
	}

	try {
		while  ( ! args.isEmpty() ) {
			clsName = args.pop( 0 );
			doIt();
		}
	}
	catch ( Exception & e ) {
		cerr << e.what() << endl;
		return -2;
	}

	return 0;

}

// ============================================================================

