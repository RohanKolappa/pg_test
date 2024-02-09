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
#include "BFC.Base.BufferParser.h"
#include "BFC.Base.Exception.h"
#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.File.h"
#include "BFC.TL.BufferArray.h"
#include "BFC.VFS.FSDir.h"
#include "BFC.VFS.FSPath.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

static Buffer readProjectName() {

	FSPath		ipath = FSDir::getCurrentDir();
	Buffer		iname;
	IO::FilePtr	ifile;

	for (;;) {
		if ( ipath.size() == 0 ) {
			msgWrn( "Can't find Make.target!" );
			return Buffer();
		}
		iname = ( ipath + "Make.target" ).toBuffer();
		if ( IO::File::exists( iname ) ) {
			msgDbg( "Defs from " + iname );
			ifile = new IO::File( iname, IO::File::Read );
			break;
		}
		ipath.append( FSPath::dot2 );
	}

	IO::ByteInputStreamPtr istream = new IO::ByteInputStream( ifile );

	try {
		istream->open();
	}
	catch ( Exception & ) {
		return Buffer();
	}

	for (;;) {
		Buffer line;
		try {
			line = istream->getLine();
		}
		catch ( Exception & ) {
			return Buffer();
		}
		if ( line.beginsWith( "target-type" ) ) {
			if ( ! line.endsWith( "dll" ) ) {
				return Buffer();
			}
		}
		else if ( line.beginsWith( "target-name" ) ) {
			BufferArray tokens = BufferParser::tokenize( line, " \t:=" );
			if ( tokens.getSize() == 2 ) {
				return tokens[ 1 ];
			}
		}

	}

	return Buffer();

}

// ============================================================================

static Buffer mergeTokens(
	const	BufferArray &	a,
	const	Buffer &	l,
	const	Bool		addLastToo,
	const	Uint32		skipLast = 0 ) {

	Buffer	res;
	Uint32	cnt = a.getSize() - skipLast;

	for ( Uint32 i = 0 ; i < cnt ; i++ ) {
		res += a[ i ];
		if ( addLastToo || i + 1 < cnt ) {
			res += l;
		}
	}

	return res;

}

// ============================================================================

static Buffer	class_name;
static Buffer	copyright_owner;
static Buffer	license_type;
static Buffer	license_vers;
static Buffer	license_pack;
static Buffer	jbs_project_name;
static Buffer	dll_declaration;
static Buffer	bfc_prefix;

// ============================================================================

static void doIt() {

	Buffer	answer;

	// Copyright owner...

	copyright_owner = "Copyright (c) 2003-2014 Barco N.V.";

	// License type...

	license_type = "Lesser General Public License";
	license_vers = "2.1";
	license_pack = "BFC";

	// DLL export stuff...

	jbs_project_name = readProjectName();

	if ( jbs_project_name.hasContent() ) {
		dll_declaration = jbs_project_name + "_DLL";
	}
	else {
		dll_declaration = "JBS_DLL_NONE";
	}

	// Doxygen group name...

	Buffer	doxy_module_name;

	if ( jbs_project_name.hasContent() ) {
		doxy_module_name = jbs_project_name;
	}

	BufferArray	namespaces;

	if ( jbs_project_name.hasContent() ) {
		namespaces = BufferParser::tokenize( jbs_project_name, '_' );
	}

	if ( ! namespaces.isEmpty() && namespaces[ 0 ] != "BFC" ) {
		bfc_prefix = "BFC::";
	}

	// --------------------------------------------------------------------

	Buffer	doxy_module_decl;

	doxy_module_decl =
		"/// \\brief [no brief description]\n";

	if ( doxy_module_name.hasContent() ) {
		doxy_module_decl +=
			"///\n"
			"/// \\ingroup " + doxy_module_name + "\n";
	}

	Buffer	namespace_prefix_dot;
	Buffer	namespace_prefix_und;
	Buffer	namespace_prefix_col;

	namespace_prefix_dot = mergeTokens( namespaces, ".", true );
	namespace_prefix_und = mergeTokens( namespaces, "_", true );
	namespace_prefix_col = mergeTokens( namespaces, "::", false );

	Buffer	source_filename = namespace_prefix_dot + class_name + ".cpp";
	Buffer	header_filename = namespace_prefix_dot + class_name + ".h";
	Buffer	source_ptr_filename = namespace_prefix_dot + class_name + "Ptr.cpp";
	Buffer	header_ptr_filename = namespace_prefix_dot + class_name + "Ptr.h";

	Buffer	copyright_text;

	if ( copyright_owner != "" ) {
		copyright_text =
			"// ============================================================================\n"
			"// \n"
			"// " + copyright_owner + "\n"
			"// \n";
	}

	Buffer	license_text;

	if ( license_type != "" ) {
		license_text =
			"// ============================================================================\n"
			"// \n"
			"// This file is part of \"" + license_pack + "\".\n"
			"// \n"
			"// \"" + license_pack + "\" is free software; you can redistribute it and/or modify it\n"
			"// under the terms of the GNU " + license_type + " as published\n"
			"// by the Free Software Foundation; either version " + license_vers + " of the License, or\n"
			"// (at your option) any later version.\n"
			"// \n"
			"// \"" + license_pack + "\" is distributed in the hope that it will be useful, but\n"
			"// WITHOUT ANY WARRANTY; without even the implied warranty of\n"
			"// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
			"// " + license_type + " for more details.\n"
			"// \n"
			"// You should have received a copy of the GNU " + license_type + "\n"
			"// along with \"" + license_pack + "\"; if not, write to:\n"
			"//                          Free Software Foundation\n"
			"//                          59 Temple Place, Suite 330\n"
			"//                          Boston, MA 02111-1307 USA\n"
			"// \n";
	}

	// --------------------------------------------------------------------

	IO::FilePtr	srcFile = new IO::File( source_filename, IO::File::New );

	srcFile->open();

	srcFile->putBytes(

		copyright_text +

		license_text +

		"// ============================================================================\n"
		"// \n"
		"// Filename:\n"
		"//	" + source_filename + "\n"
		"// \n"
		"// Description:\n"
		"//	...\n"
		"// \n"
		"// Author(s):\n"
		"//	Jean-Francois GOBBERS\n"
		"// \n"
		"// ============================================================================\n"
		"\n"
		"#include \"" + header_filename + "\"\n"
		"\n"
		"// ============================================================================\n"
		"\n" );

	srcFile->close();

	// --------------------------------------------------------------------

	IO::FilePtr	hdrFile = new IO::File( header_filename, IO::File::New );

	hdrFile->open();

	hdrFile->putBytes(
		copyright_text +
		license_text +
		"// ============================================================================\n"
		"// \n"
		"// Filename:\n"
		"//	" + header_filename +"\n"
		"// \n"
		"// Description:\n"
		"//	...\n"
		"// \n"
		"// Author(s):\n"
		"//	Jean-Francois GOBBERS\n"
		"// \n"
		"// ============================================================================\n"
		"\n"
		"#ifndef _" + namespace_prefix_und + class_name + "_H_\n"
		"#define _" + namespace_prefix_und + class_name + "_H_\n"
		"\n" );

	if ( jbs_project_name.hasContent()
	  && namespace_prefix_dot.hasContent() ) {
		hdrFile->putBytes(
			"// ============================================================================\n"
			"\n"
			"#include \"" + namespace_prefix_dot + "DLL.h\"\n"
			"\n" );
	}

	hdrFile->putBytes(
		"// ============================================================================\n"
		"\n"
		"#include \"BFC.Base.Exception.h\"\n"
		"\n" );

	if ( ! namespaces.isEmpty() ) {
		hdrFile->putBytes(
			"// ============================================================================\n"
			"\n" );
		for ( Uint32 i = 0 ; i < namespaces.getSize() ; i++ ) {
			hdrFile->putBytes(
				"namespace " + namespaces[ i ] + " {\n" );
		}
		hdrFile->putBytes(
			"\n" );
	}

	hdrFile->putBytes(
		"// ============================================================================\n"
		"\n" +
		doxy_module_decl + "\n"
		"GLOBAL_CLASS_EXCEPTION( " + dll_declaration + ", " + class_name + " );\n"
		"\n" );

	if ( ! namespaces.isEmpty() ) {
		hdrFile->putBytes(
			"// ============================================================================\n"
			"\n" );
		for ( Uint32 i = 0 ; i < namespaces.getSize() ; i++ ) {
			hdrFile->putBytes(
				"} // namespace " + namespaces[ namespaces.getSize() - i - 1 ] + "\n" );
		}
		hdrFile->putBytes(
			"\n" );
	}

	hdrFile->putBytes(
		"// ============================================================================\n"
		"\n"
		"#endif // _" + namespace_prefix_und + class_name + "_H_\n"
		"\n"
		"// ============================================================================\n"
		"\n" );

	hdrFile->close();

}

// ============================================================================

static Buffer exeName;

static void usage() {

	cerr << "Usage: " << exeName << " [option]... [exception_name]..." << endl;
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

	if ( args.isEmpty() ) {
		cerr << "Missing argument!" << endl;
		usage();
		return -1;
	}

	try {
		while ( ! args.isEmpty() ) {
			class_name = args.pop( 0 );
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

