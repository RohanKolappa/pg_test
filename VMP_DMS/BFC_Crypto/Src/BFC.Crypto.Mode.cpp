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
//	BFC.Crypto.Mode.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.UUId.h"

#include "BFC.Crypto.Mode.h"

#include "BFC.DL.Type.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, Mode )

// ============================================================================

const DL::TypeCPtr & Crypto::Mode::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "5b33c6de-3347-41c6-b2b8-6248126c4faa" ),
		"BFC.Crypto.Mode",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

void Crypto::Mode::test() {

	msgWrn( "No test() available!" );

}

// ============================================================================

