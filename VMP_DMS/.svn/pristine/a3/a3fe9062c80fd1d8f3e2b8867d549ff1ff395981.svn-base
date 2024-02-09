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
//	TokenFinder.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _TokenFinder_H_
#define _TokenFinder_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

// not using any namespace {

BFC_PTR_DECL(JBS_DLL_NONE,TokenFinder)

// } // not using any namespace

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "Token.h"

// ============================================================================

// not using any namespace {

// ============================================================================

class JBS_DLL_NONE TokenFinder : virtual public BFC::SObject {

public :

	TokenFinder(
	);

	virtual ~TokenFinder(
	);

	void process(
		const	BFC::Buffer&		line,
			TokenArray&		tokens
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("TokenFinder");

	CLASS_EXCEPTION(IncompleteToken, "Incomplete token");
	CLASS_EXCEPTION(InternalError, "Internal error");
	CLASS_EXCEPTION(InvalidCharacter, "Invalid character");

protected :

	enum Context {
		// Contexts with an associated table...
		NOC	= 0,		// No context
		STR	= 1,		// String
		INT	= 2,		// Integer
		LES	= 3,		// <
		GRE	= 4,		// >
		EQU	= 5,		// =
		DIF	= 6,		// !
		AMP	= 7,		// &
		BAR	= 8,		// |
		// Contexts without an associated table...
		PLU,			// +
		MIN,			// -
		MUL,			// *
		DIV,			// /
		MOD,			// %
		LPA,			// Left par
		RPA,			// Right par
		COM,			// Comma
		SPA,			// Space
		END,			// End of token
		INV			// Invalid!
	};

	static const Context conv[9][128];

	Token createToken(
		const	BFC::Uchar		*ptr,
		const	BFC::Uint32		start,
		const	BFC::Uint32		stop,
		const	Context			ctx
	) const;

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	TokenFinder(
		const	TokenFinder&
	);

	TokenFinder& operator = (
		const	TokenFinder&
	);

};

// ============================================================================

// } // not using any namespace

// ============================================================================

#endif // _TokenFinder_H_

// ============================================================================

