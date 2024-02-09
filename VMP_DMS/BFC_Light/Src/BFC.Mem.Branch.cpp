// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Mem.Branch.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.Mem.Branch.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::Branch::Branch() :

	currUsers( 1 ),
	currRders( 0 ),
	currWrtrs( 0 ) {

}

Mem::Branch::Branch(
		Data *		pData ) :

	DataSharer	( pData ),

	currUsers	( 1 ),
	currRders	( 0 ),
	currWrtrs	( 0 ) {

}

Mem::Branch::Branch(
	const	Buffer &	pBuffer ) :

	DataSharer	( pBuffer ),

	currUsers	( 1 ),
	currRders	( 0 ),
	currWrtrs	( 0 ) {

}

Mem::Branch::Branch(
	const	Branch &	pOther ) :

	DataSharer	( pOther ),

	currUsers	( 1 ),
	currRders	( 0 ),
	currWrtrs	( 0 ) {

	if ( pOther.currWrtrs ) {
		// The user was alone. Let's dissociate!
		getLogVarPtr();
	}

}

Mem::Branch::Branch(
	const	Branch &	pOther,
	const	Uint32		pBeg,
	const	Uint32		pLen ) :

	DataSharer	( pOther, pBeg, pLen ),

	currUsers	( 1 ),
	currRders	( 0 ),
	currWrtrs	( 0 ) {

	if ( pOther.currWrtrs ) {
		// The user was alone. Let's dissociate!
		getLogVarPtr();
	}

}

// ============================================================================

Uint32 Mem::Branch::delCreator() {

	FastCSLocker locker( criticalS );

	return --currUsers;

}

// ============================================================================

void Mem::Branch::addWriter() {

	FastCSLocker locker( criticalS );

	if ( ! currWrtrs ) {
		if ( currRders ) {
			throw Busy();
		}
		getLogVarPtr(); // ensure single user ;-)
	}

	++currWrtrs;
	++currUsers;

}

Uint32 Mem::Branch::delWriter() {

	FastCSLocker	locker( criticalS );

	if ( ! currWrtrs ) {
		throw InternalError( "Mem::Branch::delWriter!" );
	}

	--currWrtrs;
	return --currUsers;

}

// ============================================================================

void Mem::Branch::addReader() {

	FastCSLocker locker( criticalS );

	++currRders;
	++currUsers;

}

Uint32 Mem::Branch::delReader() {

	FastCSLocker	locker( criticalS );

	if ( ! currRders ) {
		throw InternalError( "Mem::Branch::delReader!" );
	}

	--currRders;
	return --currUsers;

}

// ============================================================================

Buffer Mem::Branch::toBuffer() const {

	FastCSLocker	locker( criticalS );

	Buffer res( * this );

	if ( currWrtrs ) {
		res.ensureSingleUser();
	}

	return res;

}

// ============================================================================

