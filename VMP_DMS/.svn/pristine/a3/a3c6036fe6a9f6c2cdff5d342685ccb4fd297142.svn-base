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
//	JBS.Project.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.VFS.FSDir.h"

#include "JBS.Context.h"
#include "JBS.Project.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( Project )

// ============================================================================

Project::Project() {

	tempDir = context->JBS_BUILD_SUBDIR;

}

Project::~Project() {



}

// ============================================================================

void Project::doTarget(
	const	Command&		what ) {

	switch ( what.getEnum() ) {

	case Command::DoBuild :
		doBuild();
		break;

	case Command::DoClean :
		doClean( false );
		break;

	case Command::DoDistClean :
		doClean( true );
		break;

	case Command::GenPacks :
		doGenPacks();
		break;

	case Command::GenDoc :
		doGenDoc();
		break;

	case Command::DumpVars :
		context->verbosity = Message::Debug;
		context->noRecurse = true;
		break;

	default :

		throw InternalError( "Invalid target type!" );

	}

	if ( context->noRecurse ) {
		return;
	}

	for ( ChildIterator it = getFirstChild() ; it ; it++ ) {
		if ( ! (*it)->getNoBuildFlag() ) {
			(*it)->doTarget( what );
		}
	}

}

// ============================================================================

void Project::addChild(
		ProjectPtr		project ) {

	children.insertLast( project );

}

Project::ChildIterator Project::getFirstChild() {

	return children.firstElt();

}

// ============================================================================

void Project::doBuild() {

	// No-op!

}

void Project::doClean(
	const	Bool		/* distClean */ ) {

	// No-op!

}

void Project::doGenPacks() {

	// No-op!

}

void Project::doGenDoc() {

	// No-op!

}

// ============================================================================

