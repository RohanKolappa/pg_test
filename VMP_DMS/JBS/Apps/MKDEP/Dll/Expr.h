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
//	Expr.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _Expr_H_
#define _Expr_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

// not using any namespace {

BFC_PTR_DECL(JBS_DLL_NONE,Expr)

// } // not using any namespace

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Map.h"
#include "BFC.Base.SObject.h"

// ============================================================================

// not using any namespace {

// ============================================================================

class JBS_DLL_NONE Expr : virtual public BFC::SObject {

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

	BFC::Bool isUnaryOp(
	) const {
		return ( type >= OpUnaryPlus
		      && type <= OpLogicalNot );
	}

	Expr(
	) : type( None ) {
	}

	Expr(
		const	BFC::Buffer&		s
	) : type( CstString ), string( s ) {
	}

	Expr(
		const	BFC::Int32		s
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

	typedef BFC::Map< BFC::Buffer, BFC::Buffer >	DefsList;

	BFC::Int32 eval(
		const	DefsList&		defs
	) const;

	BFC::Buffer toBuffer(
	) const;

	Type		type;
	BFC::Buffer	string;
	BFC::Int32	integer;
	ExprPtr		left;
	ExprPtr		right;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("Expr");

	CLASS_EXCEPTION(NotInitialized, "Not initialized");

protected :

	BFC::Bool isFirstTokenChar(
		const	BFC::Uchar		c
	) const {
		return ( c >= 'A' && c <= 'Z' )
		    || ( c >= 'a' && c <= 'z' )
		    || ( c == '_' );
	}

	BFC::Int32 interpret(
		const	BFC::Buffer&		token,
		const	DefsList&		defs
	) const;

private :

};

// ============================================================================

// } // not using any namespace

// ============================================================================

#endif // _Expr_H_

// ============================================================================

