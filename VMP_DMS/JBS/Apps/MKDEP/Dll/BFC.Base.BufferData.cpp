// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.BufferData.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.BufferData.h"
#include "BFC.Base.System.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

BufferData::BufferData()
	: user( 0 ), refs( 1 ) {



}

BufferData::BufferData(
	const	Uchar		*__ptr,
	const	Uint32		__len )
	: user( 0 ), refs( 1 ) {

	if ( __ptr && __len ) {
		block.alloc( __len );
		user = __len;
		System::memcpy( block.getAddr(), __ptr, __len );
	}

}

BufferData::~BufferData() {

	// Auto-delete activated!

	if ( block.getSize() ) {
		block.free();
	}

}

// ============================================================================

void BufferData::setUser(
	const	Uint32		newUser
) {

	if ( ! newUser ) {
		if ( block.getSize() ) {
			block.free();
		}
		user = 0;
		return;
	}

	if ( newUser <= block.getSize() ) {
		user = newUser;
		return;
	}

	if ( block.getSize() ) {
		block.free();
	}

	user = newUser;
	block.alloc( user );

}

void BufferData::setPhys(
	const	Uint32		newPhys
) {

	if ( newPhys <= block.getSize() ) {
		return;
	}

	if ( block.getSize() ) {
		block.free();
	}

	user = 0;
	block.alloc( newPhys );

}

// ============================================================================

Uchar BufferData::getData(
	const	Uint32		i
) const {

	return *( block.getAddr() + i );

}

void BufferData::setData(
	const	Uint32		i,
	const	Uchar		c
) {

	*( block.getAddr() + i ) = c;

}

void BufferData::setData(
	const	Uint32		i,
	const	Uchar		*p,
	const	Uint32		l
) {

	if ( ! block.getAddr() || ! p ) {
		cerr << "PROBLEMO!!!" << endl;
		cerr << "Block.addr: " << (UintPtr)block.getAddr() << endl;
		cerr << "Block.size: " << block.getSize() << endl;
		cerr << "arg.i: " << i << endl;
		cerr << "arg.p: " << (UintPtr)p << endl;
		cerr << "arg.l: " << l << endl;
		return;
	}

//	cerr << "setData(): copying '";
//	cerr.write((const char *)p, l);
//	cerr << "' to " << hex << (Uint32)block.getAddr() << dec
//		<< " with offset " << i << endl;

	System::memcpy( block.getAddr() + i, p, l );

}

// ============================================================================

void BufferData::reuse(
		Uchar		*ptr,
	const	Uint32		len,
	const	Uint32		phys
) {

	if ( block.getSize() ) {
		block.free();
	}

	block.use( ptr, phys );
	user = len;

}

// ============================================================================

