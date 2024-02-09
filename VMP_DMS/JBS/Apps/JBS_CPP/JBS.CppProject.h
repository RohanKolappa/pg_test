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
//	JBS.CppProject.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_CppProject_H_
#define _JBS_CppProject_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, CppProject )

} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.TL.Map.h"
#include "BFC.Base.Version.h"

#include "JBS.Atoms.Atom.h"
#include "JBS.BuildConfig.h"
#include "JBS.Project.h"

// ============================================================================

namespace JBS {

// ============================================================================

typedef BFC::List< CppProjectPtr >	CppProjectList;

// ============================================================================

class JBS_DLL_NONE CppProject : public Project {

public :

	CppProject(
	);

	virtual ~CppProject(
	);

	virtual void buildFromDefs(
			UserDefsMapPtr		defs
	);

	virtual void fixDefs(
	);

	const CppProjectList& getDepsList(
		const	BFC::Uint32		level = 0
							// to trace inf. loops!
	);

	const BFC::Version& getVersion(
	) const {
		return versionLocal;
	}

	BFC::Bool canBuildConfig(
		const	BuildConfig&		config
	) const;

	Atoms::AtomPtr getTargetRoot(
		const	BuildConfig&		config
	) const;

	enum ProjectType {
		NoType,
		Executable,
		Library
	};

	enum LibraryType {
		Static,	// lib
		Shared,	// so, export
		Plugin	// so, no export
	};

	ProjectType getProjectType() const { return projectType; }
	LibraryType getLibraryType() const { return libraryType; }

	const BFC::BufferArray& getCxxDefs() const { return cxxDefs; }
	const BFC::BufferArray& getCxxFlags() const { return cxxFlags; }
	const BFC::FSPathList& getLocalInclDirs() const { return localInclDirs; }
	const BFC::BufferArray& getAddLibNames() const { return addLibNames; }
	const BFC::FSPathList& getAddLibDirs() const { return addLibDirs; }
	const BFC::FSPathList& getAddInclDirs() const { return addInclDirs; }

	const Atoms::AtomList& getAllHdrAtoms(
	) const {
		return hdrList;
	}

	BASE_CLASS_GEN_EXCEPTION( "JBS.CppProject" );

	CLASS_EXCEPTION( Failed, "Failed" );

protected :

	// Command handlers...

	virtual void doBuild(
	);

	void doBuild(
		const	BuildConfig&		config
	);

	virtual void doClean(
		const	BFC::Bool		distClean
	);

	virtual void doGenPacks(
	);

	virtual void doGenDoc(
	);

	// Helpers...

	void createTree(
	);

	void unglob(
		const	BFC::BufferArray&	globbed,
			BFC::FSPathList&	unglobbed
	) const;

	void dumpBufferArray(
		const	BFC::BufferArray&	atomList,
		const	BFC::Buffer&		listName
	) const;

	void dumpAtomList(
		const	Atoms::AtomList&	atomList,
		const	BFC::Buffer&		listName
	) const;

	void dumpPathList(
		const	BFC::FSPathList&	pathList,
		const	BFC::Buffer&		listName
	) const;

	BFC::Bool foundIn(
		const	Atoms::AtomList&	list,
		const	BFC::FSPath&		path
	) const;

private :

	ProjectType		projectType;
	LibraryType		libraryType;	// if projectType == Library
	BuildConfig		buildConfig;	// from Make.target
	BFC::BufferArray	targetDeps;	// from Make.target

	// To fix when all projects are read from cache / rebuilt...

	BFC::Buffer		versionBase;
	BFC::Version		versionLocal;

	CppProjectList		depsList;

	// To interpret when all definitions are read, and we need to clean,
	// build, ...

	// ... those are the lists provided by the user in the Make.target
	// ... or other file...

	BFC::BufferArray	cppSources;
	BFC::BufferArray	mocSources;
	BFC::BufferArray	guiSources;
	BFC::BufferArray	vuiSources;
	BFC::BufferArray	quiSources;

	BFC::BufferArray	cxxDefs;	// from target-cxxdefs
	BFC::BufferArray	cxxFlags;
	BFC::BufferArray	addLibNames;
	BFC::FSPathList		addLibDirs;
	BFC::FSPathList		addInclDirs;
	BFC::Buffer		docDir;
	BFC::Buffer		doxyDir;
	BFC::Buffer		doxyImg;

	BFC::FSPathList		localInclDirs;

	// ... our entry points (lib or exe)

	Atoms::AtomPtr		tempDir;
	Atoms::AtomList		hdrList;
	BFC::Map< BuildConfig, Atoms::AtomPtr >
				tgtRoots;

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_CppProject_H_

// ============================================================================

