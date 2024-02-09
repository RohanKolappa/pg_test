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
//	BFC.Light.LibConfig.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdlib.h>

#include "BFC.Light.LibConfig.h"

#include "BFC.Base.Buffer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Light, LibConfig )

// ============================================================================

Light::LibConfig::LibConfig() {

	reload();

}

// ============================================================================

Light::LibConfigPtr Light::LibConfig::instance() {

	static LibConfigPtr i = new LibConfig;

	return i;

}

// ============================================================================

void Light::LibConfig::reload() {

	traceExceptions	= Buffer( ::getenv( "BFC_TRACE_EXCEPTIONS" ) ).toBool();
	traceDeadLocks	= Buffer( ::getenv( "BFC_TRACE_DEADLOCKS" ) ).toBool();

}

// ============================================================================

