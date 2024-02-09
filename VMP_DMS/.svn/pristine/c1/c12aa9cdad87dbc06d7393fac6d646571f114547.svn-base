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
//	JBS.Atoms.Atom.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include <stdlib.h>

#include "BFC.VFS.FSDir.h"

#include "JBS.Atoms.Atom.h"

#include "JBS.Context.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, Atom )

// ============================================================================

Atoms::Atom::Atom() {

	setObjectName( "Atom" );
	setDebug( true );

	done = DontKnow;

}

Atoms::Atom::~Atom() {



}

// ============================================================================

void Atoms::Atom::addInternalDep(
		AtomPtr		dep ) {

	intDeps.insertLast( dep );

}

void Atoms::Atom::addExternalDep(
		AtomPtr		dep ) {

	extDeps.insertLast( dep );

}

void Atoms::Atom::addPreBuildDep(
		AtomPtr		dep ) {

	preDeps.insertLast( dep );

}

// ============================================================================

void Atoms::Atom::delInternalDeps() {

	intDeps.kill();

}

void Atoms::Atom::delExternalDeps() {

	extDeps.kill();

}

// ============================================================================

Time::Clock Atoms::Atom::getBuildTime() const {

	return node.getLastModifTime();

}

// ============================================================================

void Atoms::Atom::doUpdate() {

	// If we already built this target, we may safely return!

	if ( done == Yes ) {
		return;
	}

	// Bring all external dependencies up to date...

	AtomList::Iterator it;

	for ( it = extDeps.firstElt() ; it ; it++ ) {
		(*it)->doUpdate();
	}

	// Bring all internal dependencies up to date...

	for ( it = intDeps.firstElt() ; it ; it++ ) {
		(*it)->doUpdate();
	}

	// Bring all pre-build dependencies up to date...

	for ( it = preDeps.firstElt() ; it ; it++ ) {
		(*it)->doUpdate();
	}

	// Build local target, if needed!

//	Message( Message::MsgDebug, "Checking rebuildability of "
//		+ getAtomPath().toBuffer() );

	Bool buildNeeded = false;

	if ( ! node.exists() ) {

		// The node doesn't already exist! Let's force a rebuild!

//		Message( Message::MsgDebug,
//			"File \""
//			+ getAtomPath().toBuffer()
//			+ "\" doesn't exist! Rebuild needed!" );

		buildNeeded = true;

	}
	else {

		// The node already exists! Must be a previous call to 'jbs'!
		// Or a precious file...

		if ( ! intDeps.isEmpty() ) {
		Time::Clock ref = node.getLastModifTime();
		for ( it = intDeps.firstElt() ; it ; it++ ) {
			if ( ref < (*it)->getBuildTime() ) {
//				Message( Message::MsgDebug,
//					getAtomPath().toBuffer()
//					+ " older than internal dep "
//					+ (*it)->getObjectName() );
				buildNeeded = true;
				break;
			}
		}
		}

		if ( ! buildNeeded && ! extDeps.isEmpty() ) {
		Time::Clock ref = node.getLastModifTime();
		for ( it = extDeps.firstElt() ; it ; it++ ) {
			if ( ref < (*it)->getBuildTime() ) {
//				Message( Message::MsgDebug,
//					getAtomPath().toBuffer()
//					+ " older than external dep "
//					+ (*it)->getObjectName() );
				buildNeeded = true;
				break;
			}
		}
		}

//		if ( buildNeeded ) {
//			Message( Message::MsgDebug,
//				getAtomPath().toBuffer()
//				+ " is obsolete! Rebuild needed!" );
//		}
//		else {
//			Message( Message::MsgDebug,
//				getAtomPath().toBuffer()
//				+ " is up to date!" );
//		}

	}

	if ( buildNeeded
	  && ! context->dryRun ) {
		FSDir::setCurrentDir( getProject()->getProjectPath() );
		doBuildLocalTarget();
		node.refresh();
	}

	done = Yes;

}

void Atoms::Atom::doClean(
	const	Bool	distClean ) {

	if ( done == No ) {
		return;
	}

	// Clean all internal dependencies...

	for ( AtomList::Iterator it = intDeps.firstElt() ; it ; it++ ) {
		(*it)->doClean( distClean );
	}

	// Now, clean local target!

	if ( node.exists() ) {
		FSDir::setCurrentDir( getProject()->getProjectPath() );
		doCleanLocalTarget( distClean );
	}

	done = No;

}

// ============================================================================

void Atoms::Atom::doBuildLocalTarget() {

	// 'touch' the node...

	Buffer pathbuf = path.toBuffer();

	Message( Message::Touching, path );

	Buffer cmd = "touch " + pathbuf + '\0';

	if ( ::system( (const char *)cmd.getBufferAddr() ) ) {
		throw Failed( "Can't touch: " + pathbuf );
	}

}

void Atoms::Atom::doCleanLocalTarget(
	const	Bool		/* distClean */ ) {



}

// ============================================================================

void Atoms::Atom::setAtomPath(
	const	FSPath&		buildDir,
	const	Buffer&		baseName,
	const	Buffer&		extension ) {

	Buffer fileName = baseName + extension;

	path = buildDir + fileName;
	base = baseName;
	fext = extension;

	node.setPath( path );

	setObjectName( "Atom[ " + fileName + " ]" );

}

void Atoms::Atom::setAtomPath(
	const	FSPath&		nodepath ) {

	path = nodepath;
	base = path.getLastComponent();

	node.setPath( path );

	setObjectName( "Atom[ " + base + " ]" );

}

// ============================================================================

Atoms::UserAtom::UserAtom(
		ProjectPtr	project,
	const	FSPath&		fullPath,
	const	Buffer&		extension ) {

	setProject(
		project );

	setAtomPath(
		fullPath.getDirPath(),
		fullPath.getBaseName( extension ),
		extension );

}

Atoms::UserAtom::~UserAtom() {



}

// ============================================================================

void Atoms::UserAtom::doBuildLocalTarget() {

	// Never rebuild precious files!

}

void Atoms::UserAtom::doCleanLocalTarget(
	const	BFC::Bool	/* distClean */ ) {

	// Never clean precious files!

}

// ============================================================================

Atoms::TempAtom::TempAtom(
		AtomPtr		dependency,
	const	Buffer&		newExtension,
	const	Bool		precious )
	: cleanOnDistCleanOnly( precious ) {

	setProject(
		dependency->getProject() );

	setAtomPath(
		getProject()->getTempDir(),
		dependency->getBaseName(),
		newExtension );

	addInternalDep(
		dependency );

}

Atoms::TempAtom::TempAtom(
		AtomPtr		dependency,
	const	Buffer&		newBaseName,
	const	Buffer&		newExtension,
	const	Bool		precious )
	: cleanOnDistCleanOnly( precious ) {

	setProject(
		dependency->getProject() );

	setAtomPath(
		getProject()->getTempDir(),
		newBaseName,
		newExtension );

	addInternalDep(
		dependency );

}

Atoms::TempAtom::TempAtom(
		ProjectPtr	project,
	const	FSPath&		nodePath,
	const	Bool		precious )
	: cleanOnDistCleanOnly( precious ) {

	setProject(
		project );

	setAtomPath(
		nodePath );

}

Atoms::TempAtom::TempAtom(
		ProjectPtr	project,
	const	Buffer&		newExtension,
	const	Bool		precious )
	: cleanOnDistCleanOnly( precious ) {

	setProject(
		project );

	setAtomPath(
		project->getProjectPath(),
		project->getProjectName(),
		newExtension );

}

Atoms::TempAtom::TempAtom(
		ProjectPtr	project,
	const	Buffer&		newBaseName,
	const	Buffer&		newExtension,
	const	Bool		precious )
	: cleanOnDistCleanOnly( precious ) {

	setProject(
		project );

	setAtomPath(
		FSPath::dot1,
		newBaseName,
		newExtension );

}

Atoms::TempAtom::~TempAtom() {



}

// ============================================================================

void Atoms::TempAtom::doCleanLocalTarget(
	const	Bool	distClean ) {

	if ( ! distClean && cleanOnDistCleanOnly ) {
		return;
	}

	Message( Message::Removing, getAtomPath() );

	if ( context->dryRun ) {
		return;
	}

	getAtomNode().remove();

}

// ============================================================================

