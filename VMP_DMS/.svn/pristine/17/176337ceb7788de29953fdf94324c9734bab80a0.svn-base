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
//	JBS.Command.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Command_H_
#define _JBS_Command_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, Command )

} // namespace JBS

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE Command : virtual public BFC::SObject {

public :

	enum Type {
		DoBuild		= 0,
		DoClean,
		DoDistClean,
		GenPacks,
		GenDoc,
		DumpVars,

		Default		= DoBuild,

		MinType		= DoBuild,
		MaxType		= DumpVars + 1
	};

	Command(
		const	Type		type = Default
	);

	virtual ~Command(
	);

	void convert(
		const	BFC::Buffer&	typeName
	);

	Type getEnum(
	) const {
		return type;
	}

	BFC::Buffer getTypeName(
	) const;

protected :

private :

	static const char *convTbl[];

	Type	type;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Command(
		const	Command&
	);

	Command& operator = (
		const	Command&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_Command_H_

// ============================================================================

