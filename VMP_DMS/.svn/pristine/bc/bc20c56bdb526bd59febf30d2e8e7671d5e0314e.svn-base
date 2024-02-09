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
// "BFC" is distributed pIStream the hope that it will be useful, but
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
//	BFC.Dyn.VarSerializer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.UUIdSerializer.h"

#include "BFC.Dyn.Repository.h"
#include "BFC.Dyn.Type.h"
#include "BFC.Dyn.Var.h"
#include "BFC.Dyn.VarSerializer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

void Dyn::VarSerializer::doSerialize(
		IO::ByteOutputStreamPtr	pOStream,
		VarCPtr			pDynVar ) {

	UUIdSerializer::doSerialize( pDynVar->getType()->getUUId(), pOStream );

	pDynVar->doSerialize( pOStream );

}

Dyn::VarPtr Dyn::VarSerializer::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	UUId id = UUIdSerializer::unSerialize( pIStream );

	VarPtr var = Repository::instance()->get( id )->create();

	var->unSerialize( pIStream );

	return var;

}

// ============================================================================

