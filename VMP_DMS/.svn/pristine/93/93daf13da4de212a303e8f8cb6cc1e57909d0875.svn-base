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
//	JBS.Tools.Linker.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "JBS.Context.h"
#include "JBS.CppProject.h"
#include "JBS.Tools.Linker.h"

// ============================================================================

using namespace BFC;
using namespace JBS;
using namespace JBS::Atoms;

// ============================================================================

BFC_PTR_IMPL_NS( Tools, Linker )

// ============================================================================

Tools::Linker::Linker() {

	setObjectName( "JBS.Tools.Linker" );
	setDebug( true );

#if defined( PLATFORM_WIN32 )

	JBS_LINKER	=	"link.exe";

	flgs_glb	+=	"/nologo";
	flgs_glb	+=	"/STACK:10000000";

	flgs_dbg	+=	"/debug";
	flgs_dbg	+=	"/INCREMENTAL:YES";

	flgs_sta	+=	"-lib";

	libs_glb	+=	"kernel32";
	libs_glb	+=	"user32";
	libs_glb	+=	"gdi32";
	libs_glb	+=	"winspool";
	libs_glb	+=	"comdlg32";
	libs_glb	+=	"advapi32";
	libs_glb	+=	"shell32";
	libs_glb	+=	"ole32";
	libs_glb	+=	"oleaut32";
	libs_glb	+=	"uuid";
	libs_glb	+=	"odbc32";
	libs_glb	+=	"odbccp32";

#else

	JBS_LINKER	=	"g++";

	flgs_sha	+=	"-W,--export-dynamic";

#endif

}

Tools::Linker::~Linker() {



}

// ============================================================================

void Tools::Linker::link(
		LibAtomPtr		libAtom ) const {

	CppProjectPtr	project = libAtom->getProject();
	Bool		debugMode = libAtom->getDbgMode();
	Bool		staticMode = libAtom->getStaMode();

	Buffer string;

	if ( staticMode ) {
#if defined( PLATFORM_WIN32 )
		string = "lib.exe /nologo";
		for ( AtomList::CstIterator it = libAtom->getObjList().firstElt() ; it ; it++ ) {
			string += " " + (*it)->getAtomPath().toBuffer();
		}
		string += " /out:\"" + libAtom->getAtomPath().toBuffer() + "\"";
		execute( string );
#else
		string = "ar crv " + libAtom->getAtomPath().toBuffer();
		for ( AtomList::CstIterator it = libAtom->getObjList().firstElt() ; it ; it++ ) {
			string += " " + (*it)->getAtomPath().toBuffer();
		}
		execute( string );
		string = "ranlib " + libAtom->getAtomPath().toBuffer();
		execute( string );
#endif
		return;
	}

	// Flags, libs, paths, ...

	BufferArray	final_flgs;
	BufferArray	final_libs;
	FSPathList	final_dirs;

	// ... hard-coded defaults

	final_flgs += flgs_glb;
	final_libs += libs_glb;
	final_flgs += ( debugMode ? flgs_dbg : flgs_rel );
	final_flgs += flgs_sha;

#if defined( PLATFORM_WIN32 )
	if ( ! staticMode ) {
		final_flgs += "/dll";
		final_flgs += "/incremental:no";
		final_flgs += "/machine:I386";
	}
#else
	if ( ! staticMode ) {
		final_flgs += "-shared";
//		final_flgs += "-Wl,-soname," + libAtom->getAtomPath().getLastComponent();
	}
#endif

	// ... project dependencies

	for ( AtomList::CstIterator it = libAtom->getDepList().lastElt() ; it ; it-- ) {
		LibAtomPtr	libDep = *it;
		CppProjectPtr	prjDep = libDep->getProject();
		final_libs += libDep->getShortName();
		final_dirs += prjDep->getProjectPath();
#if defined( PLATFORM_WIN32 )
		final_libs += prjDep->getAddLibNames();
		final_dirs += prjDep->getAddLibDirs();
#else
		if ( prjDep->getLibraryType() == CppProject::Shared ) {
			final_flgs += "-Wl,-rpath,"
				+ prjDep->getProjectPath().toBuffer();
		}
#endif
	}

	// ... per project settings

	final_libs += project->getAddLibNames();
	final_dirs += project->getAddLibDirs();

	// Put everything in a big long string...

#if defined( PLATFORM_WIN32 )

	Buffer	tempfile;

	if ( ! staticMode ) {
		tempfile = libAtom->getAtomPath().toBuffer();
		tempfile = tempfile( 0, tempfile.length() - LibAtom::sharedFileExt.length() );
		tempfile += LibAtom::staticFileExt;
	}

	string = buildString(
		final_flgs,
		libAtom->getObjList(),
		final_dirs,
		final_libs,
		libAtom->getAtomPath().toBuffer(),
		tempfile );

#else

	string = buildString(
		final_flgs,
		libAtom->getObjList(),
		final_dirs,
		final_libs,
		libAtom->getAtomPath().toBuffer() );

#endif

	execute( string );

}

// ============================================================================

void Tools::Linker::link(
		ExeAtomPtr		exeAtom ) const {

#if defined( PLATFORM_WIN32 )

	CppProjectPtr	project = exeAtom->getProject();
	Bool		debugMode = exeAtom->getDbgMode();
	Bool		staticMode = exeAtom->getStaMode();

	// Flags, libs, paths, ...

	BufferArray	final_flgs;
	BufferArray	final_libs;
	FSPathList	final_dirs;

	// ... hard-coded defaults

	final_flgs += flgs_glb;
	final_libs += libs_glb;
	final_flgs += ( debugMode ? flgs_dbg : flgs_rel );
	final_flgs += ( staticMode ? flgs_sta : flgs_sha );

	final_flgs += "/subsystem:console";
	final_flgs += "/machine:I386";
	if ( debugMode ) {
		Buffer pdbname = exeAtom->getBaseName() + ".pdb";
		final_flgs += "/pdb:\"" + pdbname + "\"";
	}
	else {
		final_flgs += "/pdb:none";
	}

	// ... project dependencies

	for ( AtomList::CstIterator it = exeAtom->getDepList().lastElt() ; it ; it-- ) {
		LibAtomPtr	libDep = *it;
		CppProjectPtr	prjDep = libDep->getProject();
		final_libs += libDep->getShortName();
		final_libs += prjDep->getAddLibNames();
		final_dirs += prjDep->getProjectPath();
		final_dirs += prjDep->getAddLibDirs();
	}

	// ... per project settings

	final_libs += project->getAddLibNames();
	final_dirs += project->getAddLibDirs();

	// Put everything in a big long string...

	Buffer	string;
	Buffer	tempfile;

	string = buildString(
		final_flgs,
		exeAtom->getObjList(),
		final_dirs,
		final_libs,
		exeAtom->getAtomPath().toBuffer(),
		tempfile );

	execute( string );

#else

	CppProjectPtr	project = exeAtom->getProject();
	Bool		debugMode = exeAtom->getDbgMode();
	Bool		staticMode = exeAtom->getStaMode();

	// Flags, libs, paths, ...

	BufferArray	final_flgs;
	BufferArray	final_libs;
	FSPathList	final_dirs;

	// ... hard-coded defaults

	final_flgs += flgs_glb;
	final_libs += libs_glb;
	final_flgs += ( debugMode ? flgs_dbg : flgs_rel );
	final_flgs += ( staticMode ? flgs_sta : flgs_sha );

	// ... project dependencies

	for ( AtomList::CstIterator it = exeAtom->getDepList().lastElt() ; it ; it-- ) {
		LibAtomPtr	libDep = *it;
		CppProjectPtr	prjDep = libDep->getProject();
		final_libs += libDep->getShortName();
		final_libs += prjDep->getAddLibNames();
		final_dirs += prjDep->getProjectPath();
		final_dirs += prjDep->getAddLibDirs();
		if ( ! staticMode
		  && prjDep->getLibraryType() == CppProject::Shared ) {
			final_flgs += "-Wl,-rpath,"
				+ prjDep->getProjectPath().toBuffer();
		}
	}

	// ... per project settings

	final_libs += project->getAddLibNames();
	final_dirs += project->getAddLibDirs();

	if ( staticMode ) {

		BufferArray flags;

		for ( Uint32 strat = 0 ; strat < 2 ; strat++ ) {

			flags = final_flgs;

			if ( strat == 0 ) {
				flags += "-static";
			}

			try {
				execute( buildString(
					flags,
					exeAtom->getObjList(),
					final_dirs,
					final_libs,
					exeAtom->getAtomPath().toBuffer() ) );
				break;
			}
			catch ( Failed& ) {
				Message( Message::MsgWarning,
					"Failed to link statically using strategy "
					+ Buffer( strat ) + "!" );
			}

		}

	}
	else { // ---> shared mode!

		execute( buildString(
			final_flgs,
			exeAtom->getObjList(),
			final_dirs,
			final_libs,
			exeAtom->getAtomPath().toBuffer() ) );

	}

	if ( ! debugMode ) {
		execute( "strip " + exeAtom->getAtomPath().toBuffer() );
	}

#endif

}

// ============================================================================

Buffer Tools::Linker::buildString(
	const	BufferArray&		tmp_flags,
	const	AtomList&		tmp_objects,
	const	FSPathList&		tmp_dirs,
	const	BufferArray&		tmp_libs,
	const	Buffer&			tmp_output
#if defined( PLATFORM_WIN32 )
	,const	Buffer&			tmp_tempfile
#endif
	) const {

	Buffer string;

	// ... the linker

	string = JBS_LINKER;

	// ... the flags

	Uint32 i;

	for ( i = 0 ; i < tmp_flags.size() ; i++ ) {
		string += " ";
		string += tmp_flags[ i ];
	}

	// ... the objects

	for ( AtomList::CstIterator it = tmp_objects.firstElt() ; it ; it++ ) {
		string += " " + (*it)->getAtomPath().toBuffer();
	}

	// ... the link paths

	for ( FSPathList::CstIterator pit = tmp_dirs.firstElt() ; pit ; pit++ ) {
#if defined( PLATFORM_WIN32 )
		string += " /libpath:\"" + pit->toBuffer() + "\"";
#else
		string += " -L" + pit->toBuffer();
#endif
	}

	// ... the libraries

	for ( i = 0 ; i < tmp_libs.size() ; i++ ) {
#if defined( PLATFORM_WIN32 )
		string += " " + tmp_libs[ i ] + LibAtom::staticFileExt;
#else
		string += " -l" + tmp_libs[ i ];
#endif
	}

	// ... the output file

#if defined( PLATFORM_WIN32 )
	string += " /out:\"" + tmp_output + "\"";
#else
	string += " -o " + tmp_output;
#endif

	// ... the temp .lib file for DLL under WIN32

#if defined( PLATFORM_WIN32 )
	if ( ! tmp_tempfile.isEmpty() ) {
		string += " /implib:\"" + tmp_tempfile + "\"";
	}
#endif

	return string;

}

// ============================================================================

void Tools::Linker::execute(
	const	Buffer&		string ) const {

	Message( Message::MsgDebug, string );

	Buffer tmp = string + '\0';

	if ( ::system( (const char *)tmp.getBufferAddr() ) ) {
		throw Failed();
	}

}

// ============================================================================

