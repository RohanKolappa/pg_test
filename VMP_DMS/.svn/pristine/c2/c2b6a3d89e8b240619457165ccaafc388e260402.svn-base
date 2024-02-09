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
//	JBS.DataBase.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_DataBase_H_
#define _JBS_DataBase_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, DataBase )

} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSPath.h"
#include "BFC.TL.List.h"

#include "JBS.Project.h"
#include "JBS.ProjectBuilder.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE DataBase : virtual public BFC::DObject {

public :

	DataBase(
	);

	virtual ~DataBase(
	);

	/// \brief Reads the top-level definitions file, rebuilding it from
	///	scratch if needed.

	void init(
	);

	/// \brief Rebuilds the top level definitions file, by parsing the
	///	whole tree.

	void rebuildFromScratch(
	);

	ProjectPtr getProjectByName(
		const	BFC::Buffer&		projectName
	);

	ProjectPtr getProjectByPath(
		const	BFC::FSPath&		projectPath
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.DataBase" );

	CLASS_EXCEPTION( NoSuchProject, "No such project" );

protected :

	ProjectPtr rebuildFrom(
		const	BFC::FSPath&		path
	);

	/// \brief Appends \a project, and all its children, to our list
	///	of projects..

	void populateList(
			ProjectPtr		project
	);

private :

	ProjectBuilderPtr	builder;

	typedef BFC::List< ProjectPtr >	ProjectList;
	typedef ProjectList::Iterator	ProjectIterator;

	ProjectList		projects;
	ProjectList		mostUsed;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DataBase(
		const	DataBase&
	);

	DataBase& operator = (
		const	DataBase&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_DataBase_H_

// ============================================================================

