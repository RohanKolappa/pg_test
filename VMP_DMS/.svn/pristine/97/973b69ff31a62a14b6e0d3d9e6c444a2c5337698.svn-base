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
//	BFC.DL.Repository.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Repository_H_
#define _BFC_DL_Repository_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.RepositoryPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.FastCS.h"
#include "BFC.Base.UUId.h"

#include "BFC.Debug.DObject.h"

#include "BFC.DL.FactoryArray.h"
#include "BFC.DL.FactoryPtr.h"
#include "BFC.DL.ObjectPtr.h"
#include "BFC.DL.TypePtr.h"

#include "BFC.Msg.Observable.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL Repository :

	virtual public DObject,
	virtual public Msg::Observable {

protected :

	/// \brief Creates a new Repository.
	///
	/// This new Repository automatically registers itself to the unique
	///	repository DataBase.
	///
	/// \param pRepositoryUUId
	///	The UUId that uniquely identifies this Repository.

	Repository(
		const	UUId &		pRepositoryUUId
	);

public :

	/// \brief Destroys this object.

	virtual ~Repository(
	);

	/// \brief Returns the UUId of this Repository.

	const UUId & getUUId(
	) const {
		return uuid;
	}

	/// \brief Inserts \a pFactory into this Repository.

	virtual void addFactory(
			FactoryPtr	pFactory,
		const	Buffer &	pDllName
	);

	/// \brief Removes \a pFactory from this Repository.

	virtual void delFactory(
			FactoryPtr	pFactory,
		const	Buffer &	pDllName
	);

	/// \brief Returns true iff this Repository contains a Factory capable
	///	of instantiating a class whose type UUId is \a pUUId.

	virtual Bool hasFactoryFor(
		const	UUId &		pUUId
	) const;

	/// \brief Returns a Factory capable of instantiating a class whose
	///	type UUId is \a pUUId.

	virtual FactoryPtr getFactoryFor(
		const	UUId &		pUUId
	) const;

	/// \brief Returns true iff this Repository contains a Factory capable
	///	of instantiating a class whose type name is \a pName.

	virtual Bool hasFactoryFor(
		const	Buffer &	pName
	) const;

	/// \brief Returns a Factory capable of instantiating a class whose
	///	type name is \a pName.

	virtual FactoryPtr getFactoryFor(
		const	Buffer &	pName
	) const;

	/// \brief Returns a list containing all factories currently registered
	///	to this Repository.

	virtual FactoryArray getFactories(
	) const;

	/// \brief Returns the number of factories currently registered to this
	///	Repository.

	Uint32 getNbrFactories(
	) const;

	/// \brief Returns the Factory whose index is \a pIndex.

	FactoryPtr getFactory(
		const	Uint32		pIndex
	);

	/// \brief Returns a list containing all factories that registered to this
	///	Repository since the last call to this method.

	virtual FactoryArray getLastFactories(
	);

	ObjectPtr makeObject(
		const	UUId &		pUUId
	);

protected :

	/// \brief Sends a list of all currently registered factories to
	///	\a pObserver, on its service \a pObsSrvId.

	virtual void onObserverAdded(
			Msg::ObserverPtr	pObserver,
		const	Msg::Observer::SrvId	pObsSrvId
	);

private :

	UUId			uuid;
	FastCS			critSect;
	Map< UUId, FactoryPtr >	loadMap;
	Map< UUId, FactoryPtr >	lastLst;

	/// \brief Forbidden default constructor.

	Repository(
	);

	/// \brief Forbidden copy constructor.

	Repository(
		const	Repository &
	);

	/// \brief Forbidden copy operator.

	Repository & operator = (
		const	Repository &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Repository_H_

// ============================================================================

