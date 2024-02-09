// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Debug".
// 
// "TBFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Debug.Console.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_Console_H_
#define _TBFC_Light_Console_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {

TBFC_PTR_DECL( TBFC_Light_DLL, Console )

}

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.SObject.h"

#include "TBFC.Debug.ColoredBuffer.h"

// ============================================================================

namespace TBFC {

// ============================================================================

class Console;
class ConsoleData;

// ============================================================================

/// \brief Sends an EOL character to the Console \a __console.
///
/// \ingroup TBFC_Light

TBFC_Light_DLL TBFC::Console & nl(
		TBFC::Console	& __console
);

// ============================================================================

/// \brief Provides a uniform access to the console.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL Console : virtual public SObject {

protected :

	Console(
	);

public :

	virtual ~Console(
	);

	static ConsolePtr instance(
	);

	Console & operator << (
			Console		& ( * pf )( Console & )
	);

	Console & operator << (
		const	Buffer		& buffer
	);

	Console & operator << (
		const	ColoredBuffer	& buffer
	);

	Console & operator << (
		const	Exception	& exception
	);

protected :

private :

	ConsoleData	* data;	///< Internal data.

	// others...

	Console(
		const	Console		&
	);

	Console& operator = (
		const	Console		&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_Console_H_

// ============================================================================

