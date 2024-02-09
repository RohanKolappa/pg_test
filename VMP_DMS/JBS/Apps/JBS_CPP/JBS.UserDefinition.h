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
//	JBS.UserDefinition.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_UserDefinition_H_
#define _JBS_UserDefinition_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, UserDefinition )

} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE UserDefinition : virtual public BFC::SObject {

public :

	UserDefinition(
		const	BFC::Buffer&	varname
	);

	virtual ~UserDefinition(
	);

	const BFC::Buffer& getName(
	) const {
		return name;
	}

	void reset(
	) {
		values.kill();
	}

	void append(
		const	BFC::Buffer&	value
	) {
		values += value;
	}

	BFC::Uint32 size(
	) const {
		return values.size();
	}

	const BFC::Buffer& operator [] (
		const	BFC::Uint32	index
	) const {
		return values[ index ];
	}

	const BFC::BufferArray& getValues(
	) const {
		return values;
	}

	BFC::Bool isSingleton(
	) const {
		return ( values.size() == 1 );
	}

	BFC::Buffer getSingleton(
	) const {
		return ( values.isEmpty() ? BFC::Buffer() : values[ 0 ] );
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.UserDefinition" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

	BFC::Buffer		name;
	BFC::BufferArray	values;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	UserDefinition(
		const	UserDefinition&
	);

	UserDefinition& operator = (
		const	UserDefinition&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_UserDefinition_H_

// ============================================================================

