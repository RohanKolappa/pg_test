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
//	JBS.DirTraverser.cpp
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

#include "JBS.Context.h"
#include "JBS.DirTraverser.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( DirTraverser )

// ============================================================================

DirTraverser::DirTraverser(
	const	FSPath&		startPath ) {

	setObjectName( "JBS.DirTraverser" );
	setDebug( true );

	// Push all subdirs on our list...

	Buffer	startName = startPath.toBuffer();
	FSDir	dir;

	dir.open( startName );

	for ( Uint32 i = 0 ; i < dir.size() ; i++ ) {
		Buffer entry = dir[ dir.size() - i - 1 ];
		if ( entry == "."
		  || entry == ".."
		  || entry == ".svn"
		  || entry == ".CVS" ) {
			continue;
		}
		FSPath subpath( startPath + entry );
		FSNode subnode( subpath );
		if ( subnode.isSymLink()
		  || ! subnode.isDir() ) {
			continue;
		}
//		msgDbg( "Found subdir: " + subpath.toBuffer() );
		FSNode noenternode( subpath + context->JBS_NOENTER_FILE );
		if ( noenternode.exists() ) {
//			msgWrn( "May not enter in: " + subpath.toBuffer() );
			continue;
		}
		paths.insertFirst( subpath );
	}

}

DirTraverser::~DirTraverser() {



}

// ============================================================================

FSPath DirTraverser::getNext() {

	if ( paths.isEmpty() ) {
		return FSPath();
	}

	FSPath res = paths.getFirst();

	paths.deleteFirst();

	return res;

}

// ============================================================================

