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
//	TBFC.Mem.Block.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.FastCSLocker.h"

#include "TBFC.Mem.Block.h"

// ============================================================================

using namespace TBFC;

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

Uint32 Mem::Block::getLength() const {

	return ( branchPtr->getLogLength() );

}

// ============================================================================

Buffer Mem::Block::toBuffer() const {

	return ( branchPtr->toBuffer() );

}

// ============================================================================

