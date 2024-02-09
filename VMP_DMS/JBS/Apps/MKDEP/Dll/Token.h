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
//	Token.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _Token_H_
#define _Token_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

// not using any namespace {

BFC_PTR_DECL(JBS_DLL_NONE,Token)

// } // not using any namespace

// ============================================================================

#include <iostream>

#include "BFC.TL.Array.h"
#include "BFC.TL.List.h"
#include "BFC.Base.SObject.h"

// ============================================================================

// not using any namespace {

// ============================================================================

class JBS_DLL_NONE Token : virtual public BFC::SObject {

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
		const	BFC::Buffer&	s
	) : type( CstString ), string( s ) {
	}

	Token(
		const	BFC::Int32	s
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

	BFC::Buffer toBuffer(
	) const;

	Type		type;
	BFC::Int32	integer;
	BFC::Buffer	string;

};

// ============================================================================

typedef BFC::List< Token > TokenList;
typedef BFC::Array< Token > TokenArray;

// ============================================================================

// } // not using any namespace

// ============================================================================

#endif // _Token_H_

// ============================================================================

