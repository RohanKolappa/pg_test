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
//	MkDep.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_MkDep_H_
#define _JBS_Tools_MkDep_H_

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.TL.List.h"
#include "BFC.TL.Map.h"
#include "BFC.Base.SObject.h"
#include "BFC.TL.Stack.h"

#include "JBS.Tools.MkDep.Expr.h"
#include "JBS.Tools.MkDep.PPFile.h"
#include "JBS.Tools.MkDep.Path.h"

// ============================================================================

namespace JBS {
namespace Tools {
namespace MkDep {

// ============================================================================

class JBS_DLL_NONE MkDep : virtual public BFC::SObject {

public :

	MkDep(
	);

	virtual ~MkDep(
	);

	void getDepsList(
		const	BFC::Buffer&		filename,
		const	BFC::BufferArray&	defs,
		const	BFC::BufferArray&	paths,
			BFC::BufferArray&	deps
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
		const	BFC::Buffer&		def
	) const;

	void addPath(
			PathsList&		paths,
		const	BFC::Buffer&		path
	) const;

	void generateDeps(
		const	BFC::Buffer&		filename,
		const	DefsList&		defs,
		const	PathsList&		paths
	);

	void parseFile(
		const	BFC::Buffer&		filename,
			DefsList&		defs,
		const	PathsList&		paths,
		const	BFC::Uint32		recLevel
	);

	typedef BFC::Stack< BFC::Uint32 >	IfsList;

	void processDirective(
			DirectivePtr		directive,
			DefsList&		defs,
		const	PathsList&		paths,
			IfsList&		ifs,
		const	BFC::Uint32		recLevel
	);

	BFC::Buffer convertExtension(
		const	BFC::Buffer&		filename
	) const;

	BFC::Buffer extractToken(
		const	BFC::Buffer&		line,
			BFC::Uint32&		pos
	) const;

	BFC::Buffer extractRestOfLine(
		const	BFC::Buffer&		line,
		const	BFC::Uint32		pos
	) const;

	ExprPtr buildExpr(
		const	BFC::Buffer&		line
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

} // namespace MkDep
} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_MkDep_H_

// ============================================================================

