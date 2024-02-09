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
//	JBS.UserDefsMap.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_UserDefsMap_H_
#define _JBS_UserDefsMap_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, UserDefsMap )

} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.Array.h"

#include "JBS.UserDefinition.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE UserDefsMap : virtual public BFC::SObject {

public :

	UserDefsMap(
	);

	virtual ~UserDefsMap(
	);

	void reset(
		const	BFC::Buffer&		varname
	);

	void append(
		const	BFC::Buffer&		varname,
		const	BFC::Buffer&		varvalue
	);

	UserDefinitionPtr find(
		const	BFC::Buffer&		varname
	) const;

	BFC::Uint32 size(
	) const {
		return defs.size();
	}

	UserDefinitionPtr getDef(
		const	BFC::Uint32		index
	) const {
		return defs[ index ];
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.UserDefsMap" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

	UserDefinitionPtr getDefinition(
		const	BFC::Buffer&		varname
	);

private :

	BFC::Array< UserDefinitionPtr >		defs;
	BFC::Uint32				last;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	UserDefsMap(
		const	UserDefsMap&
	);

	UserDefsMap& operator = (
		const	UserDefsMap&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_UserDefsMap_H_

// ============================================================================

