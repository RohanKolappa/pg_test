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
//	JBS.GenDep.Expr.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_GenDep_Expr_H_
#define _JBS_GenDep_Expr_H_

// ============================================================================

#include "JBS.GenDep.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace GenDep {

TBFC_PTR_DECL(JBS_GenDep_DLL,Expr)

} // namespace GenDep
} // namespace JBS

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"
#include "TBFC.TL.Array.h"
#include "TBFC.TL.Map.h"

// ============================================================================

namespace JBS {
namespace GenDep {

// ============================================================================

class JBS_GenDep_DLL Expr : virtual public TBFC::SObject {

public :

	enum Type {

		None = 0,

		CstString,
		CstInteger,

		OpBinaryPlus,
		OpBinaryMinus,
		OpBinaryMult,
		OpBinaryDiv,
		OpBinaryMod,

		OpUnaryPlus,
		OpUnaryMinus,

		OpFnDefined,

		OpLogicalNot,
		OpLogicalAnd,
		OpLogicalOr,

		OpLogicalLess,
		OpLogicalLessEqual,
		OpLogicalEqual,
		OpLogicalDiff,
		OpLogicalGreaterEqual,
		OpLogicalGreater

	};

	TBFC::Bool isUnaryOp(
	) const {
		return ( type >= OpUnaryPlus
		      && type <= OpLogicalNot );
	}

	Expr(
	) : type( None ) {
	}

	Expr(
		const	TBFC::Buffer&		s
	) : type( CstString ), string( s ) {
	}

	Expr(
		const	TBFC::Int32		s
	) : type( CstInteger ), integer( s ) {
	}

	Expr(
		const	Type			t
	) : type( t ) {
	}

	Expr(
		const	Expr&
	);

	virtual ~Expr(
	) {
	}
	
	Expr& operator = (
		const	Expr&
	);

	typedef TBFC::Map< TBFC::Buffer, TBFC::Buffer >	DefsList;

	TBFC::Int32 eval(
		const	DefsList&		defs
	) const;

	TBFC::Buffer toBuffer(
	) const;

	Type		type;
	TBFC::Buffer	string;
	TBFC::Int32	integer;
	ExprPtr		left;
	ExprPtr		right;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("JBS.GenDep.Expr");

	CLASS_EXCEPTION(NotInitialized, "Not initialized");

protected :

	static TBFC::Bool isFirstTokenChar(
		const	TBFC::Uchar		c
	) {
		return ( c >= 'A' && c <= 'Z' )
		    || ( c >= 'a' && c <= 'z' )
		    || ( c == '_' );
	}

	TBFC::Int32 interpret(
		const	TBFC::Buffer&		token,
		const	DefsList&		defs
	) const;

private :

};

// ============================================================================

typedef TBFC::Array< ExprPtr >	ExprArray;

// ============================================================================

} // namespace GenDep
} // namespace JBS

// ============================================================================

#endif // _JBS_GenDep_Expr_H_

// ============================================================================

