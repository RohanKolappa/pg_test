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
//	JBS.GenDep.MkDep.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_GenDep_MkDep_H_
#define _JBS_GenDep_MkDep_H_

// ============================================================================

#include "JBS.GenDep.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"
#include "TBFC.TL.BufferArray.h"
#include "TBFC.TL.List.h"
#include "TBFC.TL.Map.h"
#include "TBFC.TL.Stack.h"

#include "JBS.GenDep.Expr.h"
#include "JBS.GenDep.PPFile.h"
#include "JBS.GenDep.Path.h"

// ============================================================================

namespace JBS {
namespace GenDep {

// ============================================================================

class JBS_GenDep_DLL MkDep : virtual public TBFC::SObject {

public :

	MkDep(
	);

	virtual ~MkDep(
	);

	void getDepsList(
		const	TBFC::Buffer&		filename,
		const	TBFC::BufferArray&	defs,
		const	TBFC::BufferArray&	paths,
			TBFC::BufferArray&	deps
	);

	void main(
			int	argc,
			char	*argv[]
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION("MkDep");

	CLASS_EXCEPTION(InvalidArgument, "Invalid argument");
	CLASS_EXCEPTION(NotImplemented, "Not implemented");
	CLASS_EXCEPTION(SyntaxError, "Syntax error");

protected :

	typedef Expr::DefsList				DefsList;

	void initWithStdDefs(
			DefsList&		defs
	);

	void addDef(
			DefsList&		defs,
		const	TBFC::Buffer&		def
	) const;

	void addPath(
			PathsList&		paths,
		const	TBFC::Buffer&		path
	) const;

	void generateDeps(
		const	TBFC::Buffer&		filename,
		const	DefsList&		defs,
		const	PathsList&		paths
	);

	void parseFile(
		const	TBFC::Buffer&		filename,
			DefsList&		defs,
		const	PathsList&		paths,
		const	TBFC::Uint32		recLevel
	);

	typedef TBFC::Stack< TBFC::Uint32 >	IfsList;

	void processDirective(
			DirectivePtr		directive,
			DefsList&		defs,
		const	PathsList&		paths,
			IfsList&		ifs,
		const	TBFC::Uint32		recLevel
	);

	TBFC::Buffer convertExtension(
		const	TBFC::Buffer&		filename
	) const;

	TBFC::Buffer extractToken(
		const	TBFC::Buffer&		line,
			TBFC::Uint32&		pos
	) const;

	TBFC::Buffer extractRestOfLine(
		const	TBFC::Buffer&		line,
		const	TBFC::Uint32		pos
	) const;

	ExprPtr buildExpr(
		const	TBFC::Buffer&		line
	) const;

private :

	// Internal state...

	PathsList	deps;
	PPFileList	ppfiles;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	MkDep(
		const	MkDep&
	);

	MkDep& operator = (
		const	MkDep&
	);

};

// ============================================================================

} // namespace GenDep
} // namespace JBS

// ============================================================================

#endif // _JBS_GenDep_MkDep_H_

// ============================================================================

