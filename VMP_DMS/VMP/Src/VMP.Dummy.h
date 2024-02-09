// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.Dummy.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_Dummy_H_
#define _VMP_Dummy_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.DummyPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL Dummy : virtual public BFC::SObject {

public :

	/// \brief Creates a new Dummy.

	Dummy(
	);

	/// \brief Destroys this object.

	virtual ~Dummy(
	);

protected :

private :

	/// \brief Forbidden copy constructor.

	Dummy(
		const	Dummy &
	);

	/// \brief Forbidden copy operator.

	Dummy & operator = (
		const	Dummy &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_Dummy_H_

// ============================================================================

