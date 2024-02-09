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
//	JBS.GenDep.PPFileParser.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _PPFileParser_H_
#define _PPFileParser_H_

// ============================================================================

#include "JBS.GenDep.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace GenDep {

TBFC_PTR_DECL(JBS_GenDep_DLL,PPFileParser)

} // namespace GenDep
} // namespace JBS

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"

#include "JBS.GenDep.Directive.h"
#include "JBS.GenDep.Expr.h"
#include "JBS.GenDep.ExprBuilder.h"
#include "JBS.GenDep.PPFile.h"
#include "JBS.GenDep.Path.h"
#include "JBS.GenDep.Token.h"
#include "JBS.GenDep.TokenFinder.h"

// ============================================================================

namespace JBS {
namespace GenDep {

// ============================================================================

class JBS_GenDep_DLL PPFileParser : virtual public TBFC::SObject {

public :

	PPFileParser(
	);

	virtual ~PPFileParser(
	);

	PPFilePtr parse(
		const	TBFC::Buffer&		filename
	);

	PPFilePtr parse(
		const	TBFC::Buffer&		filename,
		const	PathsList&		paths
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("PPFileParser");

	CLASS_EXCEPTION(InvalidArgument, "Invalid argument");

protected :

	void readLine(
		const	TBFC::Buffer&		content,
			TBFC::Uint32&		pos
	);

	DirectivePtr processLine(
	) const;

	TBFC::Buffer extractToken(
			TBFC::Uint32&		pos
	) const;

	TBFC::Buffer extractRestOfLine(
		const	TBFC::Uint32		pos
	) const;

	ExprPtr buildExpr(
		const	TBFC::Buffer&		line
	) const;

private :

	// Internal state...

	TBFC::Uchar		*outbuf;
	TBFC::Uint32		outlen;

	TokenFinder		lexer;
	ExprBuilder		builder;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	PPFileParser(
		const	PPFileParser&
	);

	PPFileParser& operator = (
		const	PPFileParser&
	);

};

// ============================================================================

} // namespace GenDep
} // namespace JBS

// ============================================================================

#endif // _PPFileParser_H_

// ============================================================================

