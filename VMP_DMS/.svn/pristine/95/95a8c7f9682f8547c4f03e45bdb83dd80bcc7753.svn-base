// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
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
//	TBFC.TL.Uint32Array.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.System.h"

#include "TBFC.TL.Uint32Array.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL( Uint32Array )

// ============================================================================

Uint32Array::Uint32Array(const Uint32 e) {

	data = 0;
	phys = 0;
	user = 0;

	resize(e);

}

Uint32Array::Uint32Array(const Uint32Array& src)
	: SObject() {

	data = 0;
	phys = 0;
	user = 0;

	if (src.user) {

		resize(user);
		System::memcpy(
			(Uchar *)data,
			(const Uchar *)src.data,
			src.user * sizeof(Uint32));

	}

}

Uint32Array::~Uint32Array() {

	kill();

}

Uint32Array& Uint32Array::operator = (const Uint32Array& s) {

	if (this != &s) {

		resize(s.user);

		if (s.user) {
			System::memcpy(
				(Uchar *)data,
				(const Uchar *)s.data,
				s.user * sizeof(Uint32));
		}

	}

	return *this;

}

void Uint32Array::pop(const Uint32 index) {

	if (index >= user) {
		throw IndexOutOfRange();
	}

	for (Uint32 i = index ; i + 1 < user ; i++) {
		data[i] = data[i + 1];
	}

	user--;

}

void Uint32Array::insert(
	const	Uint32		index,
	const	Uint32		value) {

	if (index > user) {
		throw IndexOutOfRange();
	}

	resize(user + 1);

	for (Uint32 i = user - 1 ; i > index ; i--) {
		data[i] = data[i - 1];
	}

	data[index] = value;

}

void Uint32Array::kill() {

	if (phys) {

//		delete[] data;
		System::free( (Uchar *)data );

		data = 0;
		phys = 0;
		user = 0;

	}

}

void Uint32Array::resize(const Uint32 newUser) {

	if (newUser <= phys) {
		user = newUser;
		return;
	}

	if (newUser & 0x80000000) {
		throw Overflow();
	}

	Uint32 newPhys = (phys ? phys : 0x0400);

	while (newPhys < newUser) {
		newPhys <<= 1;
	}

	data = (Uint32 *)System::realloc((Uchar *)data, newPhys * sizeof(Uint32));
	phys = newPhys;
	user = newUser;

}

// ============================================================================

