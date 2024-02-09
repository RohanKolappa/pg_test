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
//	JBS.Atoms.DepAtom.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "BFC.Base.BufferParser.h"
#include "BFC.IO.File.h"

#include "JBS.Atoms.DepAtom.h"
#include "JBS.Context.h"

#include "JBS.Tools.MkDep.h"

// ============================================================================

using namespace BFC;
using namespace JBS;
using namespace JBS::Tools::MkDep;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, DepAtom )

// ============================================================================

Atoms::DepAtom::DepAtom(
		AtomPtr		__cppAtom )
	: TempAtom(
		__cppAtom,
		fileExt,
		true ) {

	cppAtom = __cppAtom;
	hdrList = new ListAtom;

	addExternalDep( hdrList );

	if ( getAtomNode().exists() ) {
		loadFile();
	}

}

Atoms::DepAtom::~DepAtom() {



}

// ============================================================================

const Buffer Atoms::DepAtom::fileExt = ".dep";

// ============================================================================

Time::Clock Atoms::DepAtom::getBuildTime() const {

	return hdrList->getBuildTime();

}

// ============================================================================

void Atoms::DepAtom::doBuildLocalTarget() {

	Message( Message::Gendepends, getAtomPath() );

	// Inputs to the MKDEP algo:
	// - .cpp file (of course)
	// - <defines>
	// - <include paths>

	// ... input filename

	Buffer filename = cppAtom->getAtomPath().toBuffer();

	// ... defines & include paths

	BufferArray	array_defs = context->compiler->getGlobalDefs();
	BufferArray	array_dirs;

	CppProjectPtr	proj = getProject();

	appendDefsFrom( array_defs, proj );
	appendDirsFrom( array_dirs, proj );

	for ( CppProjectList::CstIterator it = proj->getDepsList().firstElt()
	    ; it ; it++ ) {
		appendDefsFrom( array_defs, *it );
		appendDirsFrom( array_dirs, *it );
	}

	// Let's rumble!

	BufferArray	array_deps;
	MkDep		mkdep;	// our own dependencies generator! :-)

	mkdep.getDepsList(
		filename,
		array_defs,
		array_dirs,
		array_deps );

	Message( Message::MsgDebug,
		"Dependencies for: "
		+ cppAtom->getAtomPath().toBuffer() );

	for ( Uint32 i = 0 ; i < array_deps.size() ; i++ ) {
		Message( Message::MsgDebug, "... " + array_deps[ i ] );
	}

	// Remove all dependencies that do not belong to our devtree!

	populateDepsList( array_deps );

	// Update our .dep file :-)

	saveFile();

}

// ============================================================================

void Atoms::DepAtom::appendDefsFrom(
		BufferArray&	defs,
		CppProjectPtr	project ) const {

	defs += project->getCxxDefs();

}

void Atoms::DepAtom::appendDirsFrom(
		BufferArray&	dirs,
		CppProjectPtr	project ) const {

	FSPathList::CstIterator it;

	for ( it = project->getLocalInclDirs().firstElt() ; it ; it++ ) {
		Buffer name = it->toBuffer();
		Message( Message::MsgDebug, "... adding: " + name );
		dirs += name;
	}

	for ( it = project->getAddInclDirs().firstElt() ; it ; it++ ) {
		Buffer name = it->toBuffer();
		Message( Message::MsgDebug, "... adding: " + name );
		dirs += name;
	}

}

// ============================================================================

void Atoms::DepAtom::saveFile() {

	IO::FilePtr f = new IO::File(
		getAtomPath().toBuffer(),
		IO::File::Write | IO::File::Create | IO::File::Truncate );

	f->open();

	for ( AtomList::CstIterator it = hdrList->getExtDeps().firstElt() ; it ; it++ ) {
		f->putBytes(
			(*it)->getProject()->getProjectName()
			+ ":"
//			+ (*it)->getAtomPath().toBuffer()
			+ (*it)->getAtomPath().getLastComponent()
			+ "\n" );
	}

	f->close();

}

void Atoms::DepAtom::loadFile() {

//	Message( Message::MsgDebug, "Loading .dep file for \""
//		+ getAtomPath().toBuffer()
//		+ "\"..." );

	IO::FilePtr f = new IO::File(
		getAtomPath().toBuffer(),
		IO::File::Read );

	f->open();

	Buffer content = f->dumpToBuffer();

	f->close();

	BufferArray lines = BufferParser::tokenize( content, "\n" );

	for ( Uint32 i = 0 ; i < lines.size() ; i++ ) {
		if ( lines[ i ].isEmpty() ) {
			continue;
		}
//		Message( Message::MsgDebug, "Read dependency: "
//			+ lines[ i ] );
		BufferArray tokens = BufferParser::tokenize( lines[ i ], ":" );
		CppProjectPtr proj = context->database->getProjectByName(
				tokens[ 0 ] );
		HdrAtomPtr hdrAtom;
		for ( AtomList::CstIterator it = proj->getAllHdrAtoms().firstElt() ; it ; it++ ) {
//			if ( (*it)->getAtomPath().toBuffer() == tokens[ 1 ] ) {
			if ( (*it)->getAtomPath().getLastComponent() == tokens[ 1 ] ) {
				hdrAtom = *it;
				break;
			}
		}
		if ( ! hdrAtom ) {
			Message( Message::MsgDebug, "Can't find header anymore!" );
		}
		else {
			hdrList->addExternalDep( hdrAtom );
		}
	}

//	Message( Message::MsgDebug, "Loaded .dep file!" );

}

// ============================================================================

void Atoms::DepAtom::populateDepsList(
	const	BufferArray&		hdrs ) {

	hdrList->delExternalDeps();

	// Try to resolve the names...

	CppProjectList	allProjects;
	CppProjectPtr	thisProject = getProject();

	allProjects += thisProject;
	allProjects += thisProject->getDepsList();

	for ( Uint32 i = 0 ; i < hdrs.size() ; i++ ) {
		HdrAtomPtr atom = resolveHdrAtom( hdrs[ i ], allProjects );
		if ( atom ) {
			Message( Message::MsgDebug,
				"Found dependency \""
				+ hdrs[ i ]
				+ "\" in project \""
				+ atom->getProject()->getProjectName()
				+ "\"!" );
			hdrList->addExternalDep( atom );
		}
		else {
			Message( Message::MsgDebug,
				"Can't find dependency \""
				+ hdrs[ i ]
				+ "\" in any project!" );
		}
	}

}

Atoms::HdrAtomPtr Atoms::DepAtom::resolveHdrAtom(
	const	Buffer&		hdrName,
	const	CppProjectList&	allProjects ) {

	FSPath hdrPath = hdrName;
	Buffer hdrSuff = hdrPath.getLastComponent();

	for ( CppProjectList::CstIterator it = allProjects.firstElt() ; it ; it++ ) {
	for ( AtomList::CstIterator pit = (*it)->getAllHdrAtoms().firstElt() ; pit ; pit++ ) {
		if ( (*pit)->getAtomPath().getLastComponent() == hdrSuff ) {
			// We found a candidate :-)
			// Validate with the full path!
			FSPath p = (*it)->getProjectPath()
				+ (*pit)->getAtomPath();
			if ( p.toBuffer() == hdrName ) {
				return *pit;
			}
		}
	}
	}

	return HdrAtomPtr();

}

// ============================================================================

