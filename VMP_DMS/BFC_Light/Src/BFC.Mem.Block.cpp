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
//	BFC.Mem.Block.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.Mem.Block.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::Block::~Block() {

	if ( ! branchPtr->delCreator() ) {
		delete branchPtr;
	}

}

// ============================================================================

void Mem::Block::kill() {

	Branch	* oldBranchPtr;

	{
		FastCSLocker	locker( criticalS );
		oldBranchPtr = branchPtr;
		branchPtr = new Branch( new Data );
	}

	if ( ! oldBranchPtr->delCreator() ) {
		delete oldBranchPtr;
	}

}

// ============================================================================

Mem::Block & Mem::Block::operator = (
	const	Block		& __other ) {

	if ( this == & __other ) {
		return * this;
	}

	Branch	* newBranchPtr;

	{
		FastCSLocker	locker( __other.criticalS );
		newBranchPtr = new Branch( * __other.branchPtr );
	}

	Branch	* oldBranchPtr;

	{
		FastCSLocker	locker( criticalS );
		oldBranchPtr = branchPtr;
		branchPtr = newBranchPtr;
	}

	if ( ! oldBranchPtr->delCreator() ) {
		delete oldBranchPtr;
	}

	return * this;

}

// ============================================================================

Bool Mem::Block::operator == (
	const	Block &		other ) const {

	return ( toBuffer() == other.toBuffer() );

}

Bool Mem::Block::operator != (
	const	Block &		other ) const {

	return ( toBuffer() != other.toBuffer() );

}

// ============================================================================

Uint32 Mem::Block::getLength() const {

	return ( branchPtr->getLogLength() );

}

// ============================================================================

Buffer Mem::Block::toBuffer() const {

	return ( branchPtr->toBuffer() );

}

// ============================================================================

