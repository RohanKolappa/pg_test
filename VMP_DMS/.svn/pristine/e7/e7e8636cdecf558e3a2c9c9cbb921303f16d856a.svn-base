// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Mem.Branch.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.FastCSLocker.h"

#include "TBFC.Mem.Branch.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

Mem::Branch::Branch() :

	currUsers( 1 ),
	currRders( 0 ),
	currWrtrs( 0 ) {

}

Mem::Branch::Branch(
		Data		* __data ) :

	DataSharer( __data ),
	currUsers( 1 ),
	currRders( 0 ),
	currWrtrs( 0 ) {

}

Mem::Branch::Branch(
	const	Buffer		& __buffer ) :

	DataSharer( __buffer ),
	currUsers( 1 ),
	currRders( 0 ),
	currWrtrs( 0 ) {

}

Mem::Branch::Branch(
	const	Branch		& __other ) :

	DataSharer( __other ),
	currUsers( 1 ),
	currRders( 0 ),
	currWrtrs( 0 ) {

	if ( __other.currWrtrs ) {
		// The user was alone. Let's dissociate!
		getLogVarPtr();
	}

}

Mem::Branch::Branch(
	const	Branch		& __other,
	const	Uint32		__beg,
	const	Uint32		__len ) :

	DataSharer( __other, __beg, __len ),
	currUsers( 1 ),
	currRders( 0 ),
	currWrtrs( 0 ) {

	if ( __other.currWrtrs ) {
		// The user was alone. Let's dissociate!
		getLogVarPtr();
	}

}

Mem::Branch::~Branch() {



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

