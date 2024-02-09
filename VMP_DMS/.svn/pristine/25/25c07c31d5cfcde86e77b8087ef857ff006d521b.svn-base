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
//	JBS.Main.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include <iostream>

#include "BFC.VFS.FSDir.h"

#include "JBS.Context.h"
#include "JBS.Main.h"

// ============================================================================

using namespace JBS;
using namespace BFC;

// ============================================================================

BFC_PTR_IMPL( Main )

// ============================================================================

Main::Main() {

	setObjectName( "JBS.Main" );
	setDebug( true );

}

Main::~Main() {



}

// ============================================================================

int Main::doit(
		Uint32		argc,
	const	Char		*argv[] ) {

	context = new Context;

	try {
		if ( ! parseCmdLine( argc, argv ) ) {
			return 0;
		}
	}
	catch ( Exception& e ) {
		if ( context && context->verbosity >= Message::Normal ) {
			msgExc( e, "Can't parse command line!" );
			usage();
		}
		else {
			throw e;
		}
		return -1;
	}

	Message( Message::MsgInfo, "" );
	Message( Message::MsgInfo, "*******************************************" );
	Message( Message::MsgInfo, "***                                     ***" );
	Message( Message::MsgInfo, "***        XXX   XXXXXXXX     XXXXXXX   ***" );
	Message( Message::MsgInfo, "***        XXX   XXX   XXX   XXX        ***" );
	Message( Message::MsgInfo, "***        XXX   XXXXXXXX     XXXXXXX   ***" );
	Message( Message::MsgInfo, "***  XXX   XXX   XXX   XXX         XXX  ***" );
	Message( Message::MsgInfo, "***  XXX   XXX   XXX   XXX         XXX  ***" );
	Message( Message::MsgInfo, "***   XXXXXXX    XXXXXXXX    XXXXXXXX   ***" );
	Message( Message::MsgInfo, "***                                     ***" );
	Message( Message::MsgInfo, "*** (c) 2003-2014 Barco N.V. ***" );
	Message( Message::MsgInfo, "***                                     ***" );
	Message( Message::MsgInfo, "*******************************************" );
	Message( Message::MsgInfo, "" );

	Message( Message::MsgDebug, "Start folder: " + context->targetPath.toBuffer() );
	Message( Message::MsgDebug, "Start target: " + context->targetName );

	// Make sure we initialize the Context, starting from the folder of
	// our first project...

	FSDir::setCurrentDir( context->targetPath.toBuffer() );

	try {
		context->init();
	}
	catch ( Exception& e ) {
		Message( Message::MsgError, "Can't initializa context!" );
		Message( Message::MsgError, e.what() );
		return -1;
	}

	// Find project corresponding to 'targetPath'...

	ProjectPtr proj;

	try {
		proj = context->database->getProjectByPath(
				context->targetPath );
	}
	catch ( Exception& e ) {
		Message( Message::MsgError, e.what() );
		return -1;
	}

	try {
		proj->doTarget( context->targetType );
	}
	catch ( Exception& e ) {
		Message( Message::MsgError, "Failed to reach target:" );
		Message( Message::MsgError, e.what() );
		return -1;
	}

	return 0;

}

// ============================================================================

Bool Main::parseCmdLine(
		Uint32		argc,
	const	Char		*argv[] ) {

	try {
		context->targetPath.set( FSDir::getCurrentDir() );
	}
	catch ( Exception& e ) {
		throw InternalError( e, "Can't find current dir!" );
	}

	if ( ! context->targetPath.isValid() ) {
		throw InternalError( "Invalid current dir!" );
	}

	Buffer defaultTarget = "build";

	argc--;
	argv++;

	while ( argc ) {

		argc--;

		Buffer item( *argv++ );

		if ( item.isEmpty() ) {
			throw InternalError( "Empty flag!" );
		}

		else if ( item == "-C" ) {
			if ( ! argc-- ) {
				throw InternalError( "Missing directory name after '-C' flag!" );
			}
			Buffer path( *argv++ );
			try {
				context->targetPath.set( path );
			}
			catch ( Exception& e ) {
				throw InternalError( e,
					"Invalid directory! [\""
					+ path
					+ "\"]" );
			}
			if ( ! context->targetPath.isValid() ) {
				throw InternalError( "Invalid directory!" );
			}
			try {
				context->targetPath.forceAbsolute();
			}
			catch ( Exception& e ) {
				throw InternalError( e,
					"Can't convert to absolute path! [\""
					+ path
					+ "\"]" );
			}
		}

		else if ( item == "-n" || item == "--no-recurse" ) {
			context->noRecurse = true;
		}

		else if ( item == "-r" || item == "--dry-run" ) {
			context->dryRun = true;
		}

		else if ( item == "-q" || item == "--quiet" ) {
			context->verbosity = Message::Warning;
		}

		else if ( item == "-k" ) {
			context->contOnErr = true;
		}

		else if ( item == "-i" || item == "--info" ) {
			context->verbosity = Message::Debug;
		}

		else if ( item == "-t" || item == "--no-color" ) {
			context->noColor = true;
		}

		else if ( item == "-h" || item == "--help" ) {
			usage();
			return false;
		}

		else if ( item == "-v" || item == "--version" ) {
			version();
			return false;
		}

		else if ( item == "-s" || item == "--static" ) {
			context->forceSta = true;
		}

		else if ( item == "-d" || item == "--debug" ) {
			context->forceDbg = true;
		}

		else if ( item[ 0 ] != '-' ) {
			context->targetName = item;
			break;
		}

		else {
			throw InternalError( "Unsupported flag! [" + item + "]" );
		}

	}

	if ( context->targetName.isEmpty() ) {
		context->targetName = defaultTarget;
	}

	if ( argc ) {
		throw InternalError( "Too much cmdline args!" );
	}

	try {
		context->targetType.convert( context->targetName );
	}
	catch ( Exception& ) {
		throw InternalError( "Invalid target type! ["
			+ context->targetName + "]" );
	}

	return true;

}

// ============================================================================

void Main::usage() const {

	std::cout <<
"Usage: jbs [options] [target]\n"
"\n"
"Target:\n"
"    build             Rebuilds everything.\n"
"    clean             Deletes all temporary files.\n"
"    distclean         Mr.Proper-like clean-up.\n"
"    doc               Generates doxygen documentation for libraries.\n"
"    packs             Generates JPF packages, for online serving.\n"
"\n"
"Options for all targets:\n"
"    -C <dir>          Change to <dir> before doing anything.\n"
"    -n, --no-recurse  No recursive directories traversal. This doesn't\n"
"                      prevent JBS from rebuilding dependencies when\n"
"                      needed, of course!\n"
"    -r, --dry-run     Performs a dry run, i.e. nothing is built. Only\n"
"                      the actions that would be performed are shown.\n"
"    -q, --quiet       Quiet mode.\n"
"    -k                Continue as much as possible after an error.\n"
"    -i, --info        Verbose mode.\n"
"    -t, --no-color    No colored output.\n"
"    -h, --help        Shows this help and exits.\n"
"    -v, --version     Shows the version of the program and exits.\n"
"\n"
"Options for the \"build\" target:\n"
"    -s, --static      Builds static executables or libraries (the\n"
"                      default is to build shared libraries, and\n"
"                      executables using shared libraries, unless\n"
"                      instructed otherwise by a <target-file> flag).\n"
"    -d, --debug       Builds the targets in debug mode (the default\n"
"                      is to build everything is release mode, unless\n"
"                      instructed otherwise by a <target-file> flag).\n"
;

}

#define VER(x) #x

void Main::version() const {

	std::cout
		<< JBS_TARGET_VERSION_MAJ
		<< "."
		<< JBS_TARGET_VERSION_MED
		<< "."
		<< JBS_TARGET_VERSION_MIN << std::endl;

}

// ============================================================================

