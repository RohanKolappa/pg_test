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
//	JBS.Atoms.DirAtom.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.VFS.FSDir.h"

#include "JBS.Atoms.DirAtom.h"

#include "JBS.Context.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, DirAtom )

// ============================================================================

Atoms::DirAtom::DirAtom(
		ProjectPtr	proj )
	: TempAtom( proj, proj->getTempDir(), true ) {

	tempDirName = proj->getTempDir().toBuffer();
	noEnterName = context->JBS_NOENTER_FILE;


}

Atoms::DirAtom::~DirAtom() {



}

// ============================================================================

void Atoms::DirAtom::doBuildLocalTarget() {

	Message( Message::Mkdir, getAtomPath() );

	if ( context->dryRun ) {
		return;
	}

	FSDir d;

	d.open( getProject()->getProjectPath() );
	d.mkdir( tempDirName );
	d.cd( tempDirName );
	d.create( noEnterName );
	d.close();

}

void Atoms::DirAtom::doCleanLocalTarget(
	const	BFC::Bool	distClean ) {

	if ( ! distClean ) {
		return;
	}

	Message( Message::Rmdir, getAtomPath() );

	if ( context->dryRun ) {
		return;
	}

	FSDir d;

	d.open( getProject()->getProjectPath() );
	d.cd( tempDirName );
	d.rm( noEnterName );
	d.cd( FSPath::dot2 );
	d.rmdir( tempDirName );

}

// ============================================================================
#if 0
void Atoms::DirAtom::checkDone() {

	FSDir d;

	d.open( getProject()->getProjectPath() );

	if ( ! d.getAllEntries().contains( tempDirName ) ) {
		done = No;
		return;
	}

	d.cd( tempDirName );

	if ( ! d.getAllEntries().contains( noEnterName ) ) {
		throw InternalError( "Temp folder without noenter marker!" );
	}

	done = Yes;

}
#endif
// ============================================================================

Time::Clock Atoms::DirAtom::getBuildTime() const {

	return Time::Clock();

}

// ============================================================================

