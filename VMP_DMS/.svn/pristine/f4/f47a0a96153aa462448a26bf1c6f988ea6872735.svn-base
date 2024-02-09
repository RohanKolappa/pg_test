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
//	BFC.MemAllocator.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <iostream>

#include "BFC.MemAllocator.h"
#include "BFC.MemBlock.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

BFC_PTR_IMPL(MemAllocator)

// ============================================================================

MemAllocator::MemAllocator() {



}

MemAllocator::~MemAllocator() {



}

// ============================================================================

MemAllocatorPtr MemAllocator::instance() {

	static MemAllocatorPtr i = new MemAllocator;

	return i;

}

// ============================================================================

void MemAllocator::alloc(
		MemBlock	*block) {

	block->addr = new Uchar[ block->size ];

}

void MemAllocator::free(
		MemBlock	*block) {

	delete[] block->addr;

	block->addr = 0;
	block->size =
	block->expo =
	block->sidx = 0;

}

// ============================================================================

