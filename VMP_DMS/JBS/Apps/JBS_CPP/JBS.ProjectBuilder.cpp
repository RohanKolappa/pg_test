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
//	JBS.ProjectBuilder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.VFS.FSNode.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.File.h"
#include "BFC.IO.MemoryInputStream.h"

#include "JBS.Context.h"
#include "JBS.ProjectBuilder.h"
#include "JBS.UserDefsMap.h"

#include "JBS.CppProject.h"
#include "JBS.FilterProject.h"
#include "JBS.NullProject.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( ProjectBuilder )

// ============================================================================

ProjectBuilder::ProjectBuilder() {

	setObjectName( "JBS.ProjectBuilder" );
	setDebug( true );

}

ProjectBuilder::~ProjectBuilder() {



}

// ============================================================================

ProjectPtr ProjectBuilder::buildFromDir(
	const	FSPath&		dirpath ) {

	FSNode	noBuildNode( dirpath + context->JBS_NOBUILD_FILE );
	Bool	noBuildFlag = noBuildNode.exists();
	FSNode	targetNode( dirpath + context->JBS_TARGET_FILE );

	if ( targetNode.exists() ) {
		if ( ! targetNode.isFile()
		  || ! targetNode.isReadable() ) {
			throw InternalError( "In dir ["
				+ dirpath.toBuffer()
				+ "], invalid target file!" );
		}
		return buildFromV5File( dirpath, targetNode, noBuildFlag );
	}

	// No v.5 target file ? We must have a v.4 then!

	FSNode oldTargetNode( dirpath + "Make.target" );

	if ( oldTargetNode.exists() ) {
		if ( ! oldTargetNode.isFile()
		  || ! oldTargetNode.isReadable() ) {
			throw InternalError( "In dir ["
				+ dirpath.toBuffer()
				+ "], invalid Make.target!" );
		}
		return buildFromV4File( dirpath, oldTargetNode, noBuildFlag );
	}

	Message( Message::MsgError,
		"In dir ["
		+ dirpath.toBuffer()
		+ "], no target file (v.4 or v.5)!" );

	return ProjectPtr();

}

// ============================================================================

ProjectPtr ProjectBuilder::buildFromV4File(
	const	FSPath&		projectPath,
	const	FSNode&		targetFile,
	const	Bool		noBuildFlag ) {

	Buffer content = IO::File::dump( targetFile.getPath() );

//	msgDbg( content );

	IO::MemoryInputStreamPtr mis = new IO::MemoryInputStream( content );
	IO::ByteInputStreamPtr bis = new IO::ByteInputStream( mis );

	bis->open();

	Bool	condUsed = false;
	Bool	condFound = false;

	Buffer	tagInclude = "include";
	Buffer	tagIfeq = "ifeq";
	Buffer	tagIfneq = "ifneq";
	Buffer	tagElse = "else";
	Buffer	tagEndif = "endif";
	Buffer	tagPlatform = "$(JBS-PLATFORM)";
	Buffer	tagWin32 = "WIN32";
	Buffer	tagLinux = "LINUX";
#if defined( PLATFORM_WIN32 )
	Buffer	tagCurrent = "WIN32";
#else
	Buffer	tagCurrent = "LINUX";
#endif
	Buffer	tagAssign = ":=";
	Buffer	tagAppend = "+=";

	UserDefsMapPtr	defs = new UserDefsMap;

	for (;;) {
		Buffer line;
		try {
			line = bis->getLine( true );
		}
		catch ( IO::EndOfStream& ) {
			break;
		}
//		msgDbg( line );
		// Interpret line now!
		BufferArray tokens = BufferParser::tokenize( line, " \t" );
		if ( tokens.size() < 1 ) {
			throw InternalError( "No tokens in Make.target!" );
		}
		if ( tokens.size() >= 2 && tokens[ 1 ] == tagAssign ) {
			if ( ! condUsed || condFound ) {
				Buffer var = tokens[ 0 ];
				defs->reset( var );
				for ( Uint32 i = 2 ; i < tokens.size() ; i++ ) {
					defs->append( var, tokens[ i ] );
				}
			}
		}
		else if ( tokens.size() >= 2 && tokens[ 1 ] == tagAppend ) {
			if ( ! condUsed || condFound ) {
				Buffer var = tokens[ 0 ];
				for ( Uint32 i = 2 ; i < tokens.size() ; i++ ) {
					defs->append( var, tokens[ i ] );
				}
			}
		}
		else if ( tokens[ 0 ] == tagIfeq || tokens[ 0 ] == tagIfneq ) {
			if ( condUsed ) {
				throw InternalError( "Recursive if[n]eq!" );
			}
			if ( tokens.size() != 3 ) {
				throw InternalError( "Invalid if[n]eq line!" );
			}
			if ( tokens[ 1 ] != tagPlatform ) {
				throw InternalError( "Invalid if[n]eq variable (\""
					+ tokens[ 1 ]
					+ "\")!" );
			}
			if ( tokens[ 2 ] != tagWin32
			  && tokens[ 2 ] != tagLinux ) {
				throw InternalError( "Invalid if[n]eq platform (\""
					+ tokens[ 2 ]
					+ "\")!" );
			}
			condUsed = true;
			condFound = ( tokens[ 2 ] == tagCurrent );
			if ( tokens[ 0 ] == tagIfneq ) {
				condFound = ! condFound;
			}
		}
		else if ( tokens[ 0 ] == tagElse ) {
			if ( ! condUsed ) {
				throw InternalError( "else without if!" );
			}
			condFound = ! condFound;
		}
		else if ( tokens[ 0 ] == tagEndif ) {
			if ( ! condUsed ) {
				throw InternalError( "endif without if!" );
			}
			condUsed = false;
		}
		else if ( tokens[ 0 ] == tagInclude ) {
		}
		else {
			throw InternalError( "Invalid line! (" + line + ")" );
		}

	}

	// Now, try to figure out which kind of project this is... Then,
	// transmit our holy power to the appropriate constructor!

	UserDefinitionPtr typeDef = defs->find( "target-type" );

	if ( ! typeDef ) {
		throw InternalError( "No Project type!" );
	}

	if ( ! typeDef->isSingleton() ) {
		throw InternalError( "Invalid Project type!" );
	}

	Buffer typeVal = typeDef->getSingleton();

	ProjectPtr res;

	if ( typeVal == "none" ) {
		res = new NullProject;
	}
	else if ( typeVal == "exe"
	       || typeVal == "lib"
	       || typeVal == "dll"
	       || typeVal == "plugin" ) {
		res = new CppProject;
	}
	else if ( typeVal == "filter" ) {
		res = new FilterProject;
	}

	if ( ! res ) {
		throw InternalError( "Invalid project type! (\""
			+ typeVal
			+ "\")" );
	}

	res->setNoBuildFlag( noBuildFlag );
	res->setProjectPath( projectPath );
	res->buildFromDefs( defs );

	return res;

}

ProjectPtr ProjectBuilder::buildFromV5File(
	const	FSPath&		projectPath,
	const	FSNode&		targetFile,
	const	Bool		noBuildFlag ) {

	// Work-around: new file name, but old content...

	return buildFromV4File( projectPath, targetFile, noBuildFlag );

	throw InternalError( "buildFromV5File() not implemented!" );

}

// ============================================================================

