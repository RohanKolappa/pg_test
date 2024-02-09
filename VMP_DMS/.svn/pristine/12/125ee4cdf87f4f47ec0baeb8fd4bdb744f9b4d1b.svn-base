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
//	JBS.Tools.Compiler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_Compiler_H_
#define _JBS_Tools_Compiler_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Tools {

BFC_PTR_DECL( JBS_DLL_NONE, Compiler )

} // namespace Tools
} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"

#include "JBS.Atoms.ObjAtom.h"

// ============================================================================

namespace JBS {
namespace Tools {

// ============================================================================

class JBS_DLL_NONE Compiler : virtual public BFC::DObject {

public :

	Compiler(
	);

	virtual ~Compiler(
	);

	void compile(
			Atoms::ObjAtomPtr	objAtom
	) const;

	const BFC::BufferArray& getGlobalDefs(
	) const {
		return defs_glb;
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.Tools.Compiler" );

	CLASS_EXCEPTION( Failed, "Failed" );

private :

	BFC::Buffer		JBS_COMPILER;

	BFC::BufferArray	flgs_glb;
	BFC::BufferArray	flgs_rel;
	BFC::BufferArray	flgs_dbg;

	BFC::BufferArray	defs_glb;
	BFC::BufferArray	defs_rel;
	BFC::BufferArray	defs_dbg;

};

// ============================================================================

} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_Compiler_H_

// ============================================================================

