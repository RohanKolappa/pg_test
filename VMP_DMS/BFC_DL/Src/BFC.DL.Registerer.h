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
//	BFC.DL.Registerer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Registerer_H_
#define _BFC_DL_Registerer_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.DL.FactoryPtr.h"
#include "BFC.DL.RepositoryPtr.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief Registers a Factory to a Repository.
///
/// Upon creation, a Registerer will register the Factory parameter to the
/// Repository parameter, and do the opposite upon destruction.
///
/// \ingroup BFC_DL

class BFC_DL_DLL Registerer : virtual public SObject {

public :

	/// \brief Creates a new Registerer, which will automatically register
	///	Factory \a pFactory to Repository \a pRepository.

	Registerer(
			RepositoryPtr	pRepository,
			FactoryPtr	pFactory
	);

	/// \brief Destroys this object, unregistering the previously
	///	registered Factory.

	virtual ~Registerer(
	);

	RepositoryPtr getRepository(
	) const {
		return repo;
	}

	FactoryPtr getFactory(
	) const {
		return fact;
	}

protected :

private :

	RepositoryPtr	repo;
	FactoryPtr	fact;
	Buffer		name;

	/// \brief Forbidden default constructor.

	Registerer(
	);

	/// \brief Forbidden copy constructor.

	Registerer(
		const	Registerer &
	);

	/// \brief Forbidden copy operator.

	Registerer & operator = (
		const	Registerer &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Registerer_H_

// ============================================================================

