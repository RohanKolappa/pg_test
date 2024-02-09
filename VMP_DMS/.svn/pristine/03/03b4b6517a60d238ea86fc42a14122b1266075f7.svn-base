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
//	JBS.Atoms.Atom.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _JBS_Atoms_Atom_H_
#define _JBS_Atoms_Atom_H_

// ============================================================================

#if defined( PLATFORM_WIN32 )

// Disable warning C4250: inheritance via dominance

#pragma warning ( disable : 4250 )

#endif

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, Atom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSNode.h"
#include "BFC.VFS.FSPath.h"
#include "BFC.TL.List.h"
#include "BFC.Debug.DObject.h"

#include "JBS.BuildConfig.h"
#include "JBS.Project.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

typedef BFC::List< AtomPtr >	AtomList;

// ============================================================================

class JBS_DLL_NONE Atom : virtual public BFC::DObject {

public :

	Atom(
	);

	virtual ~Atom(
	);

	void addInternalDep(
			AtomPtr		dependency
	);

	void addExternalDep(
			AtomPtr		dependency
	);

	void addPreBuildDep(
			AtomPtr		dependency
	);

	void delInternalDeps(
	);

	void delExternalDeps(
	);

	ProjectPtr getProject(
	) const {
		return proj;
	}

	const BFC::FSPath& getAtomPath(
	) const {
		return path;
	}

	const BFC::FSNode& getAtomNode(
	) const {
		return node;
	}

	const BFC::Buffer& getBaseName(
	) const {
		return base;
	}

	const BFC::Buffer& getExtension(
	) const {
		return fext;
	}

	const AtomList& getIntDeps(
	) const {
		return intDeps;
	}

	const AtomList& getExtDeps(
	) const {
		return extDeps;
	}

	virtual BFC::Time::Clock getBuildTime(
	) const;

	virtual UserFlag isUpToDate(
	) const {
		return ( done );
	}

	// Recursive command handlers...

	virtual void doUpdate(
	);

	virtual void doClean(
		const	BFC::Bool	distClean
	);

	BASE_CLASS_GEN_EXCEPTION( "JBS.Atoms.UserAtom" );

	CLASS_EXCEPTION( Failed, "Failed" );
	CLASS_EXCEPTION( InternalError, "Internal error" );

protected :

	virtual void doBuildLocalTarget(
	);
//	) {}

	virtual void doCleanLocalTarget(
		const	BFC::Bool	distClean
	);

	void setProject(
			ProjectPtr	proj
	) {
		this->proj = proj;
	}

	void setAtomPath(
		const	BFC::FSPath&	buildDir,
		const	BFC::Buffer&	baseName,
		const	BFC::Buffer&	extension
	);

	void setAtomPath(
		const	BFC::FSPath&	buildDir
	);

	BFC::FSNode& getAtomNode(
	) {
		return node;
	}

private :

	ProjectPtr	proj;		// parent project
	BFC::FSPath	path;		// relative path of file
	BFC::Buffer	base;		// basename of file
	BFC::Buffer	fext;		// file extension
	BFC::FSNode	node;		// the user or temp file
	UserFlag	done;		// up to date!
	AtomList	intDeps;	// auto-clean!
	AtomList	extDeps;	// no auto-clean!
	AtomList	preDeps;	// no auto-clean, no time dependency!

};

// ============================================================================

class JBS_DLL_NONE UserAtom : virtual public Atom {

public :

	UserAtom(
			ProjectPtr	project,
		const	BFC::FSPath&	fullPath,	// given in Make.target
		const	BFC::Buffer&	extension	// expected extension
	);

	virtual ~UserAtom(
	);

	virtual void doBuildLocalTarget(
	);

	virtual void doCleanLocalTarget(
		const	BFC::Bool	distClean
	);

};

// ============================================================================

class JBS_DLL_NONE TempAtom : virtual public Atom {

public :

	TempAtom(
			AtomPtr		dependency,
		const	BFC::Buffer&	newExtension,
		const	BFC::Bool	cleanOnDistCleanOnly = false
	);

	TempAtom(
			AtomPtr		dependency,
		const	BFC::Buffer&	newBaseName,
		const	BFC::Buffer&	newExtension,
		const	BFC::Bool	cleanOnDistCleanOnly = false
	);

	TempAtom(
			ProjectPtr	project,
		const	BFC::FSPath&	nodePath,
		const	BFC::Bool	cleanOnDistCleanOnly = false
	);

	TempAtom(
			ProjectPtr	project,
		const	BFC::Buffer&	newExtension,
		const	BFC::Bool	cleanOnDistCleanOnly = false
	);

	TempAtom(
			ProjectPtr	project,
		const	BFC::Buffer&	newBaseName,
		const	BFC::Buffer&	newExtension,
		const	BFC::Bool	cleanOnDistCleanOnly = false
	);

	virtual ~TempAtom(
	);

	virtual void doCleanLocalTarget(
		const	BFC::Bool	distClean
	);

protected :

private :

	BFC::Bool	cleanOnDistCleanOnly;

};

// ============================================================================

} // namespace Atoms
} // namespace JBS

// ============================================================================

#endif // _JBS_Atoms_Atom_H_

// ============================================================================

