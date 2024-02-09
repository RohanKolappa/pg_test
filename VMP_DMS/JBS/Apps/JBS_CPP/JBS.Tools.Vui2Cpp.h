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
//	JBS.Tools.Vui2Cpp.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_Vui2Cpp_H_
#define _JBS_Tools_Vui2Cpp_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Tools {

BFC_PTR_DECL( JBS_DLL_NONE, Vui2Cpp )

} // namespace Tools
} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSPath.h"
#include "BFC.Base.SObject.h"

#include "JBS.Atoms.CppAtom.h"
#include "JBS.Atoms.HdrAtom.h"

// ============================================================================

namespace JBS {
namespace Tools {

// ============================================================================

class JBS_DLL_NONE Vui2Cpp : virtual public BFC::SObject {

public :

	Vui2Cpp(
	);

	virtual ~Vui2Cpp(
	);

	void generateDecl(
			Atoms::VuiDeclHdrAtomPtr	hdrAtom
	) const;

	void generateImpl(
			Atoms::VuiImplCppAtomPtr	cppAtom
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.Tools.Vui2Cpp" );

	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );

protected :

	BFC::Buffer stringify(
		const	BFC::Buffer&		content
	) const;

private :

};

// ============================================================================

} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_Vui2Cpp_H_

// ============================================================================

