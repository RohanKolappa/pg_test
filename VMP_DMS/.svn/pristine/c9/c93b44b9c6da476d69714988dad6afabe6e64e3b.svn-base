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
//	JBS.Context.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _JBS_Context_H_
#define _JBS_Context_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, Context )

} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSPath.h"

#include "JBS.Command.h"
#include "JBS.DataBase.h"
#include "JBS.Environment.h"
#include "JBS.FancyOutput.h"
#include "JBS.Message.h"
#include "JBS.Tools.Compiler.h"
#include "JBS.Tools.Linker.h"
#include "JBS.Tools.Mocifier.h"
#include "JBS.Tools.Qui2Vui.h"
#include "JBS.Tools.Uicifier.h"
#include "JBS.Tools.Vui2Cpp.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE Context : virtual public BFC::DObject {

public :

	Context(
	);

	virtual ~Context(
	);

	void init(
	);

	void dump(
	);

	// Settings from the command line...

	BFC::Bool	dryRun;
	BFC::Bool	noRecurse;
	BFC::Bool	noColor;
//	BFC::Bool	verbose;
//	BFC::Bool	quiet;
	Message::Type	verbosity;
	BFC::Bool	contOnErr;
	BFC::Bool	forceSta;
	BFC::Bool	forceDbg;

	// First target...

	BFC::FSPath	targetPath;		// where to start ?
	BFC::Buffer	targetName;
	Command		targetType;		// what to do ?

	// Immutable constants...

	static const BFC::Buffer	JBS_TOP_DIR_MARK;
	static const BFC::Buffer	JBS_NOENTER_FILE;
	static const BFC::Buffer	JBS_NOBUILD_FILE;
	static const BFC::Buffer	JBS_TARGET_FILE;
	static const BFC::Buffer	JBS_BUILD_SUBDIR;

	// Per JBS run constants...

	BFC::FSPath		JBS_TOP_DIR;

	// Tools: compiler, linker, ...

	BFC::Uint32		maxDepLevel;

	// Sub objects...

	FancyOutputPtr		output;
	EnvironmentPtr		environment;
	DataBasePtr		database;

	Tools::CompilerPtr	compiler;
	Tools::LinkerPtr	linker;
	Tools::MocifierPtr	mocifier;
	Tools::UicifierPtr	uicifier;
	Tools::Vui2CppPtr	vui2cpp;
	Tools::Qui2VuiPtr	qui2vui;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Context(
		const	Context&
	);

	Context& operator = (
		const	Context&
	);

};

// ============================================================================

extern ContextPtr context;

// ============================================================================

// Some global exceptions...

GLOBAL_CLASS_EXCEPTION( JBS_DLL_NONE, InternalError, "Internal error!" );

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_Context_H_

// ============================================================================

