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
//	BFC.Base.BufferData.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_BufferData_H_
#define _BFC_BufferData_H_

// ============================================================================

#include "BFC.MemBlock.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// Internal class, used by Buffer.

/// This structure is shared across multiple Buffer objects
/// in order to improve efficiency while copying, for example.
///
/// \ingroup Common_Base

class BufferData {

public :

	BufferData(
	);

	BufferData(
		const	Uchar		*ptr,
		const	Uint32		len
	);

	~BufferData(
	);

	/// Resizes this object.

	void setUser(
		const	Uint32		newUser
	);

	void setPhys(
		const	Uint32		newPhys
	);

	Uchar *getAddr() const { return block.getAddr(); }
	Uint32 getUser() const { return user; }
	Uint32 getPhys() const { return block.getSize(); }

	Uchar getData(
		const	Uint32		i
	) const;

	void setData(
		const	Uint32		i,
		const	Uchar		c
	);

	void setData(
		const	Uint32		i,
		const	Uchar		*p,
		const	Uint32		l
	);

	void reuse(
			Uchar		*ptr,
		const	Uint32		len,
		const	Uint32		phys
	);

	//

	void incRefs(
	) {
		refs++;
	}

	Uint32 decRefs(
	) {
		Uint32 res = --refs;
		return res;
	}

	Uint32 getRefs(
	) {
		Uint32 res = refs;
		return res;
	}

private :

	MemBlock	block;	// physical memory
	Uint32		user;	// requested by the user
	Uint32		refs;	// counter

	BufferData(
		const	BufferData&
	);

	BufferData& operator = (
		const	BufferData&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_BufferData_H_

// ============================================================================

