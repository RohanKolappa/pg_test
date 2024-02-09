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
//	BFC.Light.LibConfig.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_LibConfig_H_
#define _BFC_Light_LibConfig_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Light {

BFC_PTR_DECL( BFC_Light_DLL, LibConfig )

} // namespace Light
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Light {

// ============================================================================

class BFC_Light_DLL LibConfig : virtual public SObject {

protected :

	LibConfig(
	);

public :

	static LibConfigPtr instance(
	);

	void reload(
	);

	Bool getTraceExceptions(
	) const {
		return traceExceptions;
	}

	void setTraceExceptions(
		const	Bool		pYesNo
	) {
		traceExceptions = pYesNo;
	}

	Bool getTraceDeadLocks(
	) const {
		return traceDeadLocks;
	}

	void setTraceDeadLocks(
		const	Bool		pYesNo
	) {
		traceDeadLocks = pYesNo;
	}

protected :

private :

	Bool	traceExceptions;
	Bool	traceDeadLocks;

};

// ============================================================================

} // namespace Light
} // namespace BFC

// ============================================================================

#endif // _BFC_Light_LibConfig_H_

// ============================================================================

