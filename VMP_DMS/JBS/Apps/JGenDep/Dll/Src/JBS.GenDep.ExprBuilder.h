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
//	JBS.GenDep.ExprBuilder.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_GenDep_ExprBuilder_H_
#define _JBS_GenDep_ExprBuilder_H_

// ============================================================================

#include "JBS.GenDep.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace GenDep {

TBFC_PTR_DECL(JBS_GenDep_DLL,ExprBuilder)

} // namespace GenDep
} // namespace JBS

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"

#include "JBS.GenDep.Expr.h"
#include "JBS.GenDep.Token.h"

// ============================================================================

namespace JBS {
namespace GenDep {

// ============================================================================

class JBS_GenDep_DLL ExprBuilder : virtual public TBFC::SObject {

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
			TBFC::Uint32&		startPos
	) const;

	static TBFC::Uint32 prio[];

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

} // namespace GenDep
} // namespace JBS

// ============================================================================

#endif // _JBS_GenDep_ExprBuilder_H_

// ============================================================================

