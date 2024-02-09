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
//	JBS.Context.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "BFC.VFS.FSDir.h"
#include "BFC.VFS.FSNode.h"
#include "BFC.VFS.FSPath.h"

#include "JBS.Context.h"
#include "JBS.Environment.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( Context )

// ============================================================================

const Buffer Context::JBS_TOP_DIR_MARK	= ".jbs.root";
const Buffer Context::JBS_NOENTER_FILE	= ".jbs.noenter";
const Buffer Context::JBS_NOBUILD_FILE	= ".jbs.nobuild";
const Buffer Context::JBS_TARGET_FILE	= ".jbs.target";
const Buffer Context::JBS_BUILD_SUBDIR	= ".jbs.temp";

// ============================================================================

Context::Context() {

	setObjectName( "JBS.Context" );
	setDebug( true );

	dryRun		= false;
	noRecurse	= false;
	noColor		= false;
//	verbose		= false;
//	quiet		= false;
	verbosity	= Message::Normal;
	contOnErr	= false;
	forceSta	= false;
	forceDbg	= false;

	maxDepLevel	= 10;

	output		= new FancyOutput;
	environment	= new Environment;
	database	= new DataBase;

	compiler	= new Tools::Compiler;
	linker		= new Tools::Linker;
	mocifier	= new Tools::Mocifier;
	uicifier	= new Tools::Uicifier;
	vui2cpp		= new Tools::Vui2Cpp;
	qui2vui		= new Tools::Qui2Vui;

}

Context::~Context() {



}

// ============================================================================

void Context::init() {

	// We need at least to have access to the environment variables!

	environment->init();

	// Find top directory...

	FSPath path;

	path.set( FSDir::getCurrentDir() );

	for (;;) {

		FSNode node( path + JBS_TOP_DIR_MARK );

		if ( node.exists() ) {
			break;
		}

		if ( path.size() == 0 ) {
			throw InternalError( "Can't find \""
				+ JBS_TOP_DIR_MARK
				+ "\" in upper folders!" );
		}

		path.removeLast();

	}

	Message( Message::MsgDebug, "Root dir: " + path.toBuffer() );

	JBS_TOP_DIR = path;

	// Now, perform initialization of remaining sub elements...

	database->init();
	mocifier->init();
	uicifier->init();

}

// ============================================================================

void Context::dump() {



}

// ============================================================================

ContextPtr JBS::context;

// ============================================================================

