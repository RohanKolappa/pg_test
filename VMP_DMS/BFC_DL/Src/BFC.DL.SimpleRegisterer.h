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
//	BFC.DL.SimpleRegisterer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_SimpleRegisterer_H_
#define _BFC_DL_SimpleRegisterer_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.Factory.h"
#include "BFC.DL.Registerer.h"
#include "BFC.DL.RepositoryPtr.h"
#include "BFC.DL.SimpleBuilder.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

template < typename TSimple >
class SimpleRegisterer : public Registerer {

public :

	/// \brief Creates a new SimpleRegisterer.

	SimpleRegisterer(
			RepositoryPtr	pRepository
	) :
		Registerer(
			pRepository,
			new Factory(
				new SimpleBuilder< TSimple > ) )
	{
	}

protected :

private :

	/// \brief Forbidden default constructor.

	SimpleRegisterer(
	);

	/// \brief Forbidden copy constructor.

	SimpleRegisterer(
		const	SimpleRegisterer &
	);

	/// \brief Forbidden copy operator.

	SimpleRegisterer & operator = (
		const	SimpleRegisterer &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_SimpleRegisterer_H_

// ============================================================================

