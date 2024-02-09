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
//	JBS.Project.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Project_H_
#define _JBS_Project_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, Project )

} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSPath.h"
#include "BFC.TL.List.h"

#include "JBS.Command.h"
#include "JBS.UserDefsMap.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE Project : virtual public BFC::DObject {

public :

	Project(
	);

	virtual ~Project(
	);

	virtual void buildFromDefs(
			UserDefsMapPtr		defs
	) = 0;

	virtual void fixDefs(
	) {
	}

	const BFC::FSPath& getTempDir(
	) const {
		return tempDir;
	}

	void setNoBuildFlag(
		const	BFC::Bool		flag
	) {
		noBuildFlag = flag;
	}

	BFC::Bool getNoBuildFlag(
	) const {
		return noBuildFlag;
	}

	void setProjectPath(
		const	BFC::FSPath&		projectPath
	) {
		path = projectPath;
	}

	const BFC::FSPath& getProjectPath(
	) const {
		return path;
	}

	const BFC::Buffer& getProjectName(
	) const {
		return name;
	}

	void doTarget(
		const	Command&		what
	);

	// To access the children...

	/// \brief Appends \a child at the end of our list of children.

	void addChild(
			ProjectPtr		child
	);

	typedef BFC::List< ProjectPtr >		ProjectList;
	typedef ProjectList::Iterator		ChildIterator;

	ChildIterator getFirstChild(
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.Project" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

	void setProjectName(
		const	BFC::Buffer&		projectName
	) {
		name = projectName;
	}

	// Command handlers...

	virtual void doBuild(
	);

	virtual void doClean(
		const	BFC::Bool		distClean
	);

	virtual void doGenPacks(
	);

	virtual void doGenDoc(
	);

private :

	BFC::Bool		noBuildFlag;
	BFC::Buffer		name;
	BFC::FSPath		path;		// project path!
	BFC::FSPath		tempDir;	// relative to 'path'

	ProjectList		children;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Project(
		const	Project&
	);

	Project& operator = (
		const	Project&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_Project_H_

// ============================================================================

