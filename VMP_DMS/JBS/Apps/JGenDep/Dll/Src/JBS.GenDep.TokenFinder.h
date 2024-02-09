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
//	JBS.GenDep.TokenFinder.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_GenDep_TokenFinder_H_
#define _JBS_GenDep_TokenFinder_H_

// ============================================================================

#include "JBS.GenDep.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace GenDep {

TBFC_PTR_DECL( JBS_GenDep_DLL, TokenFinder )

} // namespace GenDep
} // namespace JBS

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"

#include "JBS.GenDep.Token.h"

// ============================================================================

namespace JBS {
namespace GenDep {

// ============================================================================

class JBS_GenDep_DLL TokenFinder : virtual public TBFC::SObject {

public :

	TokenFinder(
	);

	virtual ~TokenFinder(
	);

	void process(
		const	TBFC::Buffer &		line,
			TokenArray &		res
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.GenDep.TokenFinder" );

	CLASS_EXCEPTION( IncompleteToken, "Incomplete token" );
	CLASS_EXCEPTION( InternalError, "Internal error" );
	CLASS_EXCEPTION( InvalidCharacter, "Invalid character" );

protected :

//	enum Context {
//
//		NOC	= 0,		// No context
//
//		STR	= 1,		// String
//		INT	= 2,		// Integer
//
//		LES	= 3,		// <, <<, <=
//		GRE	= 4,		// >, >>, >=
//		EQU	= 5,		// =, ==
//		DIF	= 6,		// !, !=
//		AMP	= 7,		// &, &&
//		BAR	= 8,		// |, ||
//
//		PLU,			// '+'
//		MIN,			// '-'
//		MUL,			// '*'
//		DIV,			// '/'
//		MOD,			// '%'
//		LPA,			// '('
//		RPA,			// ')'
//		COM,			// ','
//		TIL,			// '~'
//		HAT,			// '^'
//
//		SPA,			// Space
//
//		END,			// End of token
//
//		INV			// Invalid!
//	};
//
//	static const Context conv[9][128];
//
//	Token createToken(
//		const	TBFC::Buffer &		buffer,
//		const	TBFC::Uint32		start,
//		const	TBFC::Uint32		stop,
//		const	Context			ctx
//	) const;

	static TBFC::Bool isAlpha(
		const	TBFC::Uchar		c
	) {
		return ( ( c >= 'a' && c <= 'z' )
		      || ( c >= 'A' && c <= 'Z' )
		      || c == '_' );
	}

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

} // namespace GenDep
} // namespace JBS

// ============================================================================

#endif // _JBS_GenDep_TokenFinder_H_

// ============================================================================

