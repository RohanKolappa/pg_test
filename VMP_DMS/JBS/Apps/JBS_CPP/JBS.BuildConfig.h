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
//	JBS.BuildConfig.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_BuildConfig_H_
#define _JBS_BuildConfig_H_

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "JBS.UserFlag.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE BuildConfig {

public :

	BuildConfig(
	) : dbgMode( DontKnow )
	  , staMode( DontKnow ) {
	}

	BuildConfig(
		const	UserFlag	d,
		const	UserFlag	s
	) : dbgMode( d )
	  , staMode( s ) {
	}

	BuildConfig(
		const	BFC::Bool	d,
		const	BFC::Bool	s
	) : dbgMode( d ? Yes : No )
	  , staMode( s ? Yes : No ) {
	}

	virtual ~BuildConfig(
	) {
	}

	bool operator == (
		const	BuildConfig&	o
	) const {
		return ( dbgMode == o.dbgMode && staMode == o.staMode );
	}

	BFC::Buffer toBuffer(
	) const;

	static BFC::Buffer getVal(
		const	UserFlag	v
	);

	BFC::Bool isCompatibleWith(
		const	BuildConfig&	c
	) const;

	UserFlag	dbgMode;
	UserFlag	staMode;

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_BuildConfig_H_

// ============================================================================

