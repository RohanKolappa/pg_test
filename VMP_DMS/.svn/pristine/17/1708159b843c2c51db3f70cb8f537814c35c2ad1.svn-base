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
//	JBS.Tools.Mocifier.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _JBS_Tools_Mocifier_H_
#define _JBS_Tools_Mocifier_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Tools {

BFC_PTR_DECL( JBS_DLL_NONE, Mocifier )

} // namespace Tools
} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSPath.h"

#include "JBS.Atoms.CppAtom.h"

// ============================================================================

namespace JBS {
namespace Tools {

// ============================================================================

class JBS_DLL_NONE Mocifier : virtual public BFC::DObject {

public :

	Mocifier(
	);

	virtual ~Mocifier(
	);

	void init(
	);

	void mocify(
			Atoms::MocCppAtomPtr	cppAtom
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.Tools.Mocifier" );

	CLASS_EXCEPTION( Failed, "Failed" );

private :

	BFC::FSPath		JBS_MOCIFIER;

};

// ============================================================================

} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_Mocifier_H_

// ============================================================================

