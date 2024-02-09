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
//	BFC.Crypto.PRNGDescriptor.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.PRNGDescriptor.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, PRNGDescriptor )

// ============================================================================

Crypto::PRNGDescriptor::PRNGDescriptor(
	const	DL::Descr &	pDescr,
	const	Uint32		pExportSize ) :

	DL::Descr	( pDescr ),

	exprtSze	( pExportSize ) {

}

Crypto::PRNGDescriptor::PRNGDescriptor(
	const	PRNGDescriptor &
				pOther ) :

	SObject		(),
	DL::Descr	( pOther ),

	exprtSze	( pOther.exprtSze ) {

}

// ============================================================================

DL::DescrPtr Crypto::PRNGDescriptor::clone() const {

	return new PRNGDescriptor( *this );

}

// ============================================================================

