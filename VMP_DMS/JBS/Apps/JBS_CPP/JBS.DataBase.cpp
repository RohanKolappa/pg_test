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
//	JBS.DataBase.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.Context.h"
#include "JBS.DataBase.h"
#include "JBS.DirTraverser.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( DataBase )

// ============================================================================

DataBase::DataBase() {

	setObjectName( "JBS.DataBase" );
	setDebug( true );

	builder = new ProjectBuilder;

}

DataBase::~DataBase() {



}

// ============================================================================

void DataBase::init() {

//	msgDbg( "init(): root: " + context->JBS_TOP_DIR.toBuffer() );
//	msgDbg( "init(): file: " + context->JBS_DEFS_FILE );

	rebuildFromScratch();

}

// ============================================================================

void DataBase::rebuildFromScratch() {

	ProjectPtr root = rebuildFrom( context->JBS_TOP_DIR );

	// Convert the tree representation, starting with 'root', into a
	// more suitable flat list...

	populateList( root );

	Message( Message::MsgDebug, "Found "
		+ Buffer( projects.size() )
		+ " projects!" );

	// Now that the database is up-to-date, we can't fix all projects
	// having dependencies...

	for ( ProjectIterator it = projects.firstElt() ; it ; it++ ) {
		(*it)->fixDefs();
	}

}

// ============================================================================

ProjectPtr DataBase::getProjectByName(
	const	Buffer&		projectName ) {

	ProjectIterator it;

	for ( it = mostUsed.firstElt() ; it ; it++ ) {
		if ( (*it)->getProjectName() == projectName ) {
			return *it;
		}
	}

	for ( it = projects.firstElt() ; it ; it++ ) {
		if ( (*it)->getProjectName() == projectName ) {
			mostUsed.insertLast( *it );
			return *it;
		}
	}

	throw NoSuchProject( "Name: \"" + projectName + "\"" );

}

ProjectPtr DataBase::getProjectByPath(
	const	FSPath&		projectPath ) {

	ProjectIterator it;

	for ( it = projects.firstElt() ; it ; it++ ) {
		if ( (*it)->getProjectPath() == projectPath ) {
			return *it;
		}
	}

	throw NoSuchProject( "Path: \"" + projectPath.toBuffer() + "\"" );

}

// ============================================================================

ProjectPtr DataBase::rebuildFrom(
	const	FSPath&		path ) {

	ProjectPtr		root = builder->buildFromDir( path );
	DirTraverserPtr		finder = new DirTraverser( path );

	if ( root ) {
		for ( ;; ) {
			// Find a new subdir to parse...
			FSPath subpath = finder->getNext();
			if ( ! subpath.isValid() ) {
				break;
			}
			// Construct the corresponding Project...
			ProjectPtr sub = rebuildFrom( subpath );
			if ( sub ) {
				root->addChild( sub );
			}
		}
	}

	return root;

}

// ============================================================================

void DataBase::populateList(
		ProjectPtr	project ) {

	projects.insertLast( project );

	for ( Project::ChildIterator it = project->getFirstChild() ; it ; it++ ) {
		populateList( *it );
	}

}

// ============================================================================

