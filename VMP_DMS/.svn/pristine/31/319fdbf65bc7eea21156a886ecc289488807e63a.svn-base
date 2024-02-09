// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Debug".
// 
// "BFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Debug.Console.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_Console_H_
#define _BFC_Light_Console_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {

BFC_PTR_DECL( BFC_Light_DLL, Console )

}

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.Debug.ColoredBuffer.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Sends an EOL character to the Console \a __console.
///
/// \ingroup BFC_Light

BFC_Light_DLL BFC::Console & nl(
		BFC::Console	& __console
);

// ============================================================================

/// \brief Provides a uniform access to the console.
///
/// \ingroup BFC_Light

class BFC_Light_DLL Console : virtual public SObject {

protected :

	Console(
	);

public :

	virtual ~Console(
	);

	static ConsolePtr instance(
	);

	Console & operator << (
			Console &	( * pf )( Console & )
	);

	Console & operator << (
		const	Buffer &	buffer
	);

	Console & operator << (
		const	ColoredBuffer &	buffer
	);

protected :

private :

	class BFC_Light_DLL Data;

	Data *		data;	///< Internal data.

	// others...

	Console(
		const	Console &
	);

	Console& operator = (
		const	Console &
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Light_Console_H_

// ============================================================================

