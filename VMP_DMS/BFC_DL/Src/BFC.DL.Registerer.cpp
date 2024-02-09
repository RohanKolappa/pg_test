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
//	BFC.DL.Registerer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_LINUX )
#if ! defined( _GNU_SOURCE )
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#endif

#include <iostream>

#include "BFC.DL.Registerer.h"
#include "BFC.DL.Repository.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

DL::Registerer::Registerer(
		RepositoryPtr	pRepository,
		FactoryPtr	pFactory ) :

	repo	( pRepository ),
	fact	( pFactory ) {

#if defined( PLATFORM_LINUX )

	Dl_info	i;

	if ( ! ::dladdr( ( void * )this, &i ) ) {
		throw InternalError( "Can't find this' address..." );
	}

	name = Buffer( i.dli_fname, true );

#else

//	throw NotImplemented();

	name = "<global>";

#endif

	try {
		if ( repo && fact ) {
			repo->addFactory( fact, name );
		}
	}
	catch ( Exception & ) {
		repo.kill();
		fact.kill();
	}

}

DL::Registerer::~Registerer() {

	try {
		if ( repo && fact ) {
			repo->delFactory( fact, name );
		}
	}
	catch ( Exception & ) {
	}

}

// ============================================================================

