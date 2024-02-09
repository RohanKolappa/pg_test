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
//	ExprBuilder.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_MkDep_ExprBuilder_H_
#define _JBS_Tools_MkDep_ExprBuilder_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Tools {
namespace MkDep {

// not using any namespace {

BFC_PTR_DECL(JBS_DLL_NONE,ExprBuilder)

// } // not using any namespace
} // namespace MkDep
} // namespace Tools
} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "JBS.Tools.MkDep.Expr.h"
#include "JBS.Tools.MkDep.Token.h"

// ============================================================================

namespace JBS {
namespace Tools {
namespace MkDep {

// not using any namespace {

// ============================================================================

class JBS_DLL_NONE ExprBuilder : virtual public BFC::SObject {

public :

	ExprBuilder(
	);

	virtual ~ExprBuilder(
	);

	ExprPtr process(
		const	TokenArray&		tokens
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("ExprBuilder");

	CLASS_EXCEPTION(InternalError, "Internal error");
	CLASS_EXCEPTION(JunkAtEnd, "Junk at end");
	CLASS_EXCEPTION(SyntaxError, "Syntax error");

protected :

	ExprPtr process(
		const	TokenArray&		tokens,
			BFC::Uint32&		startPos
	) const;

	static BFC::Uint32 prio[];

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	ExprBuilder(
		const	ExprBuilder&
	);

	ExprBuilder& operator = (
		const	ExprBuilder&
	);

};

// ============================================================================

// } // not using any namespace
} // namespace MkDep
} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_MkDep_ExprBuilder_H_

// ============================================================================

