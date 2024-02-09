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
//	BFC.Types.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================


#include "BFC.Base.Types.h"


// ============================================================================

using namespace BFC;

// ============================================================================

#if 0

class Allocated {
public :
	~Allocated() {}
	static Allocated& instance(
	) {
		static Allocated i;
		return i;
	}
	void operator += (const Uint32 i) {allocated += i;}
	void operator -= (const Uint32 i) {allocated -= i;}
	Uint64 val() const {return allocated;}
protected :
	Allocated() {}
private :
	Uint64	allocated;
};

// ============================================================================

void *operator new(size_t s) {

	Uchar *ptr = (Uchar *)malloc(s + 4);

	*(Uint32 *)ptr = (Uint32)s;

	Allocated::instance() += (Uint32)s;

	return (void *)(ptr + 4);

}

void *operator new[](size_t s) {

	Uchar *ptr = (Uchar *)malloc(s + 4);

	*(Uint32 *)ptr = (Uint32)s;

	Allocated::instance() += (Uint32)(s);

	return (void *)(ptr + 4);

}

void operator delete(void *p) {

	Uchar *ptr = (Uchar *)p - 4;

	Allocated::instance() -= *(Uint32 *)ptr;

	free((void *)ptr);

}

void operator delete[](void *p) {

	Uchar *ptr = (Uchar *)p - 4;

	Allocated::instance() -= *(Uint32 *)ptr;

	free((void *)ptr);

}

// ============================================================================

Uint64 getAllocatedMemory() {

	return Allocated::instance().val();

}

#endif

// ============================================================================





