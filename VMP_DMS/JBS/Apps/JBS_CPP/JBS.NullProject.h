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
//	JBS.NullProject.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_NullProject_H_
#define _JBS_NullProject_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, NullProject )

} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "JBS.Project.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE NullProject : public Project {

public :

	NullProject(
	);

	virtual ~NullProject(
	);

	virtual void buildFromDefs(
			UserDefsMapPtr		defs
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.NullProject" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	NullProject(
		const	NullProject&
	);

	NullProject& operator = (
		const	NullProject&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_NullProject_H_

// ============================================================================

