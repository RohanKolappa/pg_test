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
//	JBS.Tools.Compiler.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include <stdlib.h>

#include "BFC.Base.Version.h"

#include "JBS.Atoms.CppAtom.h"
#include "JBS.Atoms.SbrAtom.h"
#include "JBS.Context.h"
#include "JBS.CppProject.h"
#include "JBS.Tools.Compiler.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Tools, Compiler )

// ============================================================================

Tools::Compiler::Compiler() {

	setObjectName( "JBS.Tools.Compiler" );
	setDebug( true );

#if defined( PLATFORM_WIN32 )

	JBS_COMPILER	=	"cl.exe";

	flgs_glb	+=	"/nologo";	// suppress copyright message
	flgs_glb	+=	"/W3";		// set warning level 3 (default n=1)
	flgs_glb	+=	"/WX";		// treat warnings as errors
	flgs_glb	+=	"/FD";		// ?
	flgs_glb	+=	"/c";		// compile only, no link
	flgs_glb	+=	"/GX";		// enable C++ EH (same as /EHsc)
	flgs_glb	+=	"/GR";		// enable C++ RTTI
	flgs_glb	+=	"/Gd";		// __cdecl calling convention
	flgs_glb	+=	"/EHa";		// enable asynchronous C++ EH
	flgs_glb	+=	"/Oy-";		// disable frame pointer omission
	flgs_glb	+=	"/Od";		// disable optimizations (default)
	flgs_glb	+=	"/Zm200";	// max memory alloc == 200% of default

	defs_glb	+=	"WIN32";
	defs_glb	+=	"_WIN32";
	defs_glb	+=	"WINDOWS";
	defs_glb	+=	"_WINDOWS";
	defs_glb	+=	"PLATFORM_WIN32";

	flgs_dbg	+=	"/MDd";	// link with MSVCRTD.LIB debug lib
	flgs_dbg	+=	"/ZI";	// enable Edit and Continue debug info
	flgs_dbg	+=	"/GZ";	// enable runtime debug checks
	flgs_dbg	+=	"/Yd";	// put debug info in every .OBJ
	flgs_dbg	+=	"/Zi";
	flgs_dbg	+=	"/Gi";

	defs_dbg	+=	"_DEBUG";

	flgs_rel	+=	"/MD";	// link with MSVCRT.LIB
	flgs_rel	+=	"/Ob2";	// inline expansion (default n=0)

	defs_rel	+=	"NDEBUG";

#else

	JBS_COMPILER	=	"g++";

	flgs_glb	+=	"-c";
	flgs_glb	+=	"-W";
	flgs_glb	+=	"-Wall";
	flgs_glb	+=	"-Werror";
	flgs_glb	+=	"-Wno-deprecated";
	flgs_glb	+=	"-fno-strength-reduce";
	flgs_glb	+=	"-fno-omit-frame-pointer";
	flgs_glb	+=	"-fno-defer-pop";
	flgs_glb	+=	"-fno-default-inline";
	flgs_glb	+=	"-fPIC";
	flgs_glb	+=	"-pipe";

	defs_glb	+=	"_REENTRANT";
	defs_glb	+=	"_XOPEN_SOURCE=500";
	defs_glb	+=	"_GNU_SOURCE";
	defs_glb	+=	"PLATFORM_LINUX";
	defs_glb	+=	"LINUX";

	flgs_dbg	+=	"-g";

	flgs_rel	+=	"-O3";

#endif

	defs_dbg	+=	"JBS_TARGET_DEBUG";

	defs_rel	+=	"JBS_TARGET_RELEASE";

}

Tools::Compiler::~Compiler() {



}

// ============================================================================

void Tools::Compiler::compile(
		Atoms::ObjAtomPtr	objAtom ) const {

	CppProjectPtr		project = objAtom->getProject();
	Atoms::CppAtomPtr	cppAtom = objAtom->getCppAtom();
#if defined( PLATFORM_WIN32 )
	Atoms::SbrAtomPtr	sbrAtom = objAtom->getSbrAtom();
#endif
	Bool			debugMode = objAtom->getDebugMode();

	// Definitions, flags, include paths, ...

	BufferArray	final_flgs;
	BufferArray	final_defs;
	FSPathList	final_dirs;

	final_flgs += flgs_glb;
	final_defs += defs_glb;

	final_flgs += ( debugMode ? flgs_dbg : flgs_rel );
	final_defs += ( debugMode ? defs_dbg : defs_rel );

#if defined( PLATFORM_WIN32 )
	if ( project->getProjectType() == CppProject::Library ) {
	if ( objAtom->getStaticMode() ) {
		final_defs += project->getProjectName() + "_DLL=JBS_DLL_NONE";
	}
	else {
		final_defs += project->getProjectName() + "_DLL=JBS_DLL_EXPORT";
	}
	}
#else
	if ( project->getProjectType() == CppProject::Library ) {
		final_defs += project->getProjectName() + "_DLL=JBS_DLL_NONE";
	}
#endif

	Version ver = project->getVersion();

	final_defs += "JBS_TARGET_VERSION_MAJ=" + Buffer( ver.getMaj() );
	final_defs += "JBS_TARGET_VERSION_MED=" + Buffer( ver.getMed() );
	final_defs += "JBS_TARGET_VERSION_MIN=" + Buffer( ver.getMin() );

	final_defs += "TARGET_VERSION_MAJ=" + Buffer( ver.getMaj() );
	final_defs += "TARGET_VERSION_MED=" + Buffer( ver.getMed() );
	final_defs += "TARGET_VERSION_MIN=" + Buffer( ver.getMin() );

	final_defs += "PLUGIN_VERSION_MAJ=" + Buffer( ver.getMaj() );
	final_defs += "PLUGIN_VERSION_MED=" + Buffer( ver.getMed() );
	final_defs += "PLUGIN_VERSION_MIN=" + Buffer( ver.getMin() );

	final_flgs += project->getCxxFlags();
	final_defs += project->getCxxDefs();

	final_dirs += project->getLocalInclDirs();
	final_dirs += project->getAddInclDirs();

	// Integrate dependencies...

	for ( CppProjectList::CstIterator it = project->getDepsList().firstElt() ; it ; it++ ) {
		final_defs += (*it)->getProjectName() + "_DLL=JBS_DLL_IMPORT";
		final_defs += (*it)->getCxxDefs();
		final_flgs += (*it)->getCxxFlags();
		final_dirs += (*it)->getLocalInclDirs();
		final_dirs += (*it)->getAddInclDirs();
	}

	// Put everything in a big long string...

	Buffer	string = JBS_COMPILER;
	Uint32	i;

	for ( i = 0 ; i < final_flgs.size() ; i++ ) {
		string += " ";
		string += final_flgs[ i ];
	}

	for ( i = 0 ; i < final_defs.size() ; i++ ) {
#if defined( PLATFORM_WIN32 )
		string += " /D\"";
#else
		string += " -D";
#endif
		string += final_defs[ i ];
#if defined( PLATFORM_WIN32 )
		string += "\"";
#endif
	}

	for ( FSPathList::Iterator pit = final_dirs.firstElt() ; pit ; pit++ ) {
#if defined( PLATFORM_WIN32 )
		string += " /I\"";
#else
		string += " -I";
#endif
		string += pit->toBuffer();
#if defined( PLATFORM_WIN32 )
		string += "\"";
#endif
	}

#if defined( PLATFORM_WIN32 )
	string += " /Fo\"" + objAtom->getAtomPath().toBuffer() + "\"";
	if ( objAtom->getDebugMode() ) {
		string += " /Fr\"" + sbrAtom->getAtomPath().toBuffer() + "\"";
	}
#else
	string += " -o " + objAtom->getAtomPath().toBuffer();
#endif
	string += " " + cppAtom->getAtomPath().toBuffer();

	Message( Message::MsgDebug, string );

	string += '\0';

	if ( ::system( (const char *)string.getBufferAddr() ) ) {
		throw Failed();
	}

}

// ============================================================================

