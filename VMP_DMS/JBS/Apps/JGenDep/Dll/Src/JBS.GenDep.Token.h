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
//	JBS.GenDep.Token.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_GenDep_Token_H_
#define _JBS_GenDep_Token_H_

// ============================================================================

#include "JBS.GenDep.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace GenDep {

TBFC_PTR_DECL(JBS_GenDep_DLL,Token)

} // namespace GenDep
} // namespace JBS

// ============================================================================

#include <iostream>

#include "TBFC.TL.Array.h"
#include "TBFC.TL.List.h"
#include "TBFC.Base.SObject.h"

// ============================================================================

namespace JBS {
namespace GenDep {

// ============================================================================

class JBS_GenDep_DLL Token : virtual public TBFC::SObject {

public :

	enum Type {

		None = 0,

		CstString,	// __cpluscplus
		CstInteger,	// 12347

		ParLeft,	// (
		ParRight,	// )

		Comma,		// ,

		Plus,		// +
		Minus,		// -
		Mult,		// *
		Div,		// /
		Mod,		// %

		LShift,		// <<
		RShift,		// >>

		BitAnd,		// &
		BitOr,		// |
		BitCompl,	// ~
		BitXor,		// ^

		Not,		// !
		And,		// &&
		Or,		// ||

		Less,		// <
		LessEqual,	// <=
		Equal,		// ==
		Diff,		// !=
		GreaterEqual,	// >=
		Greater		// >

	};

	Token(
		const	Type		t = None
	) : type( t ) {
	}

	Token(
		const	TBFC::Buffer&	s
	) : type( CstString ), string( s ) {
	}

	Token(
		const	TBFC::Int64	s
	) : type( CstInteger ), integer( s ) {
	}

	Token(
		const	Token&		t
	) : SObject(), type( t.type ), integer( t.integer ), string( t.string ) {
	}

	virtual ~Token(
	) {
	}

	Token& operator = (
		const	Token&		t
	) {
		if ( this != &t ) {
			type = t.type;
			integer = t.integer;
			string = t.string;
		}
		return *this;
	}

	TBFC::Buffer toBuffer(
	) const;

	Type		type;
	TBFC::Int64	integer;
	TBFC::Buffer	string;

};

// ============================================================================

typedef TBFC::List< Token > TokenList;
typedef TBFC::Array< Token > TokenArray;

// ============================================================================

} // namespace GenDep
} // namespace JBS

// ============================================================================

#endif // _JBS_GenDep_Token_H_

// ============================================================================

