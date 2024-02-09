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
//	BFC.MemAllocator.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_MemAllocator_H_
#define _BFC_MemAllocator_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {

BFC_PTR_DECL(JBS_DLL_NONE,MemAllocator)

} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

class MemBlock;

// ============================================================================

class MemAllocator : virtual public BFC::SObject {

protected :

	MemAllocator(
		const	Uint32		M,
		const	Uint32		N,
		const	Uint32		P
	);

public :

	virtual ~MemAllocator(
	);

	static MemAllocatorPtr instance(
	);

private :

	// interface for the MemBlock!

	friend class MemBlock;

	void alloc(
			MemBlock	*block
	);

	void free(
			MemBlock	*block
	);

private :

	// others...

	MemAllocator(
	);

	MemAllocator(
		const	MemAllocator&
	);

	MemAllocator& operator = (
		const	MemAllocator&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_MemAllocator_H_

// ============================================================================

