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
//	JBS.Environment.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _JBS_Environment_H_
#define _JBS_Environment_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, Environment )

} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSPath.h"
#include "BFC.TL.Map.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE Environment : virtual public BFC::DObject {

public :

	Environment(
	);

	virtual ~Environment(
	);

	void init(
	);

	BFC::Bool exists(
		const	BFC::Buffer&		variable
	) const;

	BFC::Buffer get(
		const	BFC::Buffer&		variable
	) const;

	BFC::FSPath getPath(
		const	BFC::Buffer&		variable
	) const;

	BFC::Buffer expandLine(
		const	BFC::Buffer&		string
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.Environment" );

	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );

protected :

private :

	BFC::Map< BFC::Buffer, BFC::Buffer >	variables;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Environment(
		const	Environment&
	);

	Environment& operator = (
		const	Environment&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_Environment_H_

// ============================================================================

