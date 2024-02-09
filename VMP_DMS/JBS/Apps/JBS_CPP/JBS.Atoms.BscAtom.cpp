// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "JBS".
// 
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	JBS.Atoms.BscAtom.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )

// ============================================================================

#include "JBS.Atoms.BscAtom.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Atoms, BscAtom )

// ============================================================================

Atoms::BscAtom::BscAtom(
		ProjectPtr	project )
	: TempAtom( project, fileExt ) {



}

Atoms::BscAtom::~BscAtom() {



}

// ============================================================================

const Buffer Atoms::BscAtom::fileExt = ".bsc";

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

