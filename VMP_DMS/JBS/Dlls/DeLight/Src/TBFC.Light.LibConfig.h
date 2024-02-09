// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC".
// 
// "TBFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Light.LibConfig.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_LibConfig_H_
#define _TBFC_Light_LibConfig_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {
namespace Light {

TBFC_PTR_DECL( TBFC_Light_DLL, LibConfig )

} // namespace Light
} // namespace TBFC

// ============================================================================

#include "TBFC.Base.SObject.h"

// ============================================================================

namespace TBFC {
namespace Light {

// ============================================================================

class TBFC_Light_DLL LibConfig : virtual public SObject {

protected :

	LibConfig(
	);

public :

	virtual ~LibConfig(
	);

	static LibConfigPtr instance(
	);

	void reload(
	);

	Bool doTraceExceptions(
	) const {
		return traceExceptions;
	}

protected :

private :

	Bool	traceExceptions;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	LibConfig(
		const	LibConfig&
	);

	LibConfig& operator = (
		const	LibConfig&
	);

};

// ============================================================================

} // namespace Light
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_LibConfig_H_

// ============================================================================

