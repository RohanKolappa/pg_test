// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.BufferParser.h
// 
// Description:
//	...
// 
// Author(s):
//	Kester Aernoudt
// 
// ============================================================================

#ifndef _BFC_Base_BufferBuilder_H_
#define _BFC_Base_BufferBuilder_H_

// ============================================================================

#include "BFC.Light.DLL.h"

#include <stdarg.h>

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Provides static methods to parse Buffer objects.
///
/// \ingroup BFC_Base

class BFC_Light_DLL BufferBuilder {

public :

	BufferBuilder(
	);

	virtual ~BufferBuilder(
	);

	/// \brief Returns a string formatted like sprintf.

	static Buffer format(
		const	Char *	format,
			...
	);

	/// \brief Returns a string formatted like sprintf

	static Buffer formatV(
		const	Char *	format,
			va_list		argptr
	);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION( "BFC.Base.BufferBuilder" );

	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );

protected :

private :

	// others...

	BufferBuilder(
		const	BufferBuilder&
	);

	BufferBuilder& operator = (
		const	BufferBuilder&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_BufferParser_H_

// ============================================================================

